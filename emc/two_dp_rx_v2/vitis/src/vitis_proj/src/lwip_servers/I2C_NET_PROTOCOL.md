# 以太网 I2C 协议：命令 0x20（读）/ 0x21（写）

面向**上位机开发**。板端实现：`lwip_common.c` → `i2c_net_cmd()`，由 `msg_cmd_0x20` / `msg_cmd_0x21` 调用。

**设计目标：上位机只维护一套应答解析器。**  
无论成功、`PARAM`、`BAD_FRAME` 或总线失败，应答布局固定；用 `xfer` / `done` / `byte_status[]`（读另加 `data[]`）表达结果，**没有**第二种“失败回显原请求长度”的形态。

---

## 1. 传输

| 项 | 值 |
|----|-----|
| 板端 IP（默认） | `192.168.1.10` |
| 协议 | UDP |
| 端口 | `5555`（`UDP_CMD_SVR_PORT`） |
| 应答 | 回发到**本次请求的源 IP:源端口**（`udp_sendto`） |

约束：

- 上位机 **MUST 串行**：未收到对应 `seq` 的应答或未超时前，不要发下一笔。
- 不要多客户端并行抢同一 CMD 端口。
- UDP 载荷长度 **必须等于** 帧内 `msg_len`（小端，含头与 checksum）。

TCP CMD（同端口号定义）也可走到相同 `msg_cmd_0x20/0x21`；下文以 UDP 为主。

---

## 2. 公共帧格式

| 偏移 | 字段 | 字节序 | 说明 |
|------|------|--------|------|
| 0..1 | `msg_len` | LE | 整帧长度，含本字段与末尾 checksum |
| 2..3 | `seq` | LE | 事务标签；上位机填写，板端**恒回显**（失败也不改） |
| 4 | `cmd` | — | `0x20` 读 / `0x21` 写 |
| 5 .. | payload | — | 见下 |
| 末字节 | `checksum` | — | `checksum = (~sum(字节[0 .. msg_len-2])) & 0xFF` |

双方 **MUST** 校验 checksum。板端校验失败 → `xfer=BAD_FRAME(6)`，**不上 I2C 总线**。

---

## 3. 请求

### 3.1 公共请求头（offset 5 起）

| 偏移 | 字段 | 说明 |
|------|------|------|
| 5 | `ch` | I2C 总线号（`i2c_no`，须 `< I2C_NO_BUTT`） |
| 6 | `addr7` | **7-bit** 从机地址，`0 .. 0x7F`（不是 8-bit 读写地址） |
| 7 | `len` | 数据字节数，**1 .. 64** |
| 8..9 | `reg` | 起始寄存器地址（**大端**） |
| 10 | `opt` | 见下 |

### 3.2 `opt`

| bit | 掩码 | 含义 |
|-----|------|------|
| 0 | `0x01` | `1`=16-bit 寄存器地址；`0`=8-bit 寄存器地址 |
| 1 | `0x02` | `1`=允许 clock stretch；`0`=关闭 |
| 其余 | — | **必须为 0**，否则 `PARAM` |

常用：`opt = 0x03`（reg16 + stretch）。

### 3.3 读请求 `cmd=0x20`

- 固定：`msg_len = 12`
- 无写数据区；末字节为 checksum
- 帧布局：`[msg_len LE][seq LE][0x20][ch][addr7][len][reg BE][opt][csum]`

### 3.4 写请求 `cmd=0x21`

- `msg_len = 12 + len`
- 写数据从 offset **11** 起，连续 `len` 字节，其后为 checksum
- 布局：`[msg_len LE][seq LE][0x21][ch][addr7][len][reg BE][opt][data×len][csum]`

### 3.5 参数硬约束（板端检查，失败不上总线）

| 条件 | `xfer` |
|------|--------|
| `len==0` 或 `len>64` | `PARAM(4)`，应答按 **`len=1`** 组包，且应答 `[7]=1` |
| `ch` 非法 / `addr7>0x7F` / `opt` 保留位非 0 | `PARAM(4)` |
| reg8（`opt bit0=0`）且 `reg + len - 1 > 0xFF` | `PARAM(4)` |
| UDP 长度 ≠ `msg_len`，或长度与期望不符，或 checksum 错，或包过短 | `BAD_FRAME(6)` |

期望请求长度：

- 读：`msg_len == 12` 且 UDP 载荷长度 == 12
- 写：`msg_len == 12 + len` 且 UDP 载荷长度 == `12 + len`

优先级：先判 `len` 非法 → `PARAM`；`len` 合法再判帧/校验 → `BAD_FRAME`。

---

## 4. 应答（统一布局）

应答 `[0:1]` 是**应答自身**的 `msg_len`（不是请求长度）。  
`[2:3]` 回显请求 `seq`。  
请求 ≥11 字节时，先回显请求前 11 字节，再覆盖：

- `[7] =` 实际用于组包的 `len`（非法 len 时为 `1`）
- `[0:1]` 再写成应答 `msg_len`

### 4.1 读应答 `0x20` — 恒为 `msg_len = 14 + 2*len`

| 偏移 | 字段 |
|------|------|
| 11 | `xfer` |
| 12 | `done` |
| 13 .. 12+len | `byte_status[0 .. len-1]` |
| 13+len .. 12+2*len | `data[0 .. len-1]` |
| 末 | checksum |

- 无效/未执行的数据字节板端填 `0xFF`。**禁止用 `data==0xFF` 判错**，只信 `byte_status` / `xfer`。
- 例：`len=1` → `msg_len=16`；`len=4` → `msg_len=22`。

### 4.2 写应答 `0x21` — 恒为 `msg_len = 14 + len`

| 偏移 | 字段 |
|------|------|
| 11 | `xfer` |
| 12 | `done` |
| 13 .. 12+len | `byte_status[0 .. len-1]` |
| 末 | checksum |

- 例：`len=1` → `msg_len=15`；`len=4` → `msg_len=18`。

### 4.3 短请求时的回显

若收到的请求 `< 11` 字节：应答头清零；若 ≥4 字节仍回显 `seq`；`cmd` 填本命令（`0x20`/`0x21`）；`len` 按组包规则（通常为 1）。

---

## 5. 状态码（`xfer` 与 `byte_status[i]`）

| 值 | 名称 | 含义 |
|----|------|------|
| 0 | OK | 成功 |
| 1 | NODEV | 7-bit 地址字节 NACK（无器件/地址错） |
| 2 | XFER | 地址已 ACK 后的 NACK（寄存器/数据阶段） |
| 3 | TIMEOUT | clock stretch 超时 |
| 4 | PARAM | 参数非法（不上总线） |
| 5 | SKIPPED | 本笔已 STOP，该数据字节未执行（多字节尾部） |
| 6 | BAD_FRAME | 帧长/校验错误（不上总线） |

### 不变式（上位机建议校验；违反可判协议异常）

- `xfer == 0` ⇔ `done == len` 且 `byte_status` 全 0
- `xfer != 0` ⇒ `done` = 已成功数据前缀长度；若 `done < len`，则 `byte_status[done] == xfer`，其后为 `SKIPPED(5)`（多字节）
- **读**：失败时通常 `done ∈ {0, len}`（数据阶段无 Slave NACK；地址阶段失败多为 `done=0`）
- **写**（多字节）：可在第 `i` 个数据字节失败，此时 `done == i`

`PARAM` / `BAD_FRAME`：`done=0`，`byte_status` 全为对应错误码（非法 `len` 时仅 1 字节状态）。

---

## 6. 板端总线语义（与 `len` 分派）

| `len` | 读 API | 写 API |
|-------|--------|--------|
| `1` | `xgpio_i2c_reg8/16_read` | `xgpio_i2c_reg8/16_write` |
| `>1` | `xgpio_i2c_reg*_continuous_read` | `xgpio_i2c_reg*_continuous_write` |

- **连续写**：每个数据字节需 Slave ACK；任一 NACK/TIMEOUT → STOP，后续字节 `SKIPPED`。
- **连续读**：前 `len-1` 字节 Master ACK，末字节 Master NACK 后 STOP。
- `opt.bit0` 选择 8/16-bit 寄存器寻址；`opt.bit1` 选择 stretch。

---

## 7. 上位机推荐实现（唯一解析路径）

```
1. 收 UDP；len(payload) 必须 == 应答 msg_len
2. 校验 checksum
3. 按请求 cmd：
     读: assert msg_len == 14 + 2*L
     写: assert msg_len == 14 + L
   其中 L 优先用应答字节[7]（非法请求 len 时板端会改成 1）
4. 匹配 seq；不匹配则丢弃（迟到包）
5. 读 xfer：
     ==0 → 成功；读命令采信全部 data[]
     !=0 → 看 done / byte_status[]；仅 byte_status[i]==0 的 data[i] 可信
6. 写命令若需重传：换新 seq，或接受可能双写；板端无幂等缓存
```

伪代码 checksum：

```python
def checksum(frame_without_last: bytes) -> int:
    return (~sum(frame_without_last) & 0xFF) & 0xFF
```

---

## 8. 示例（十六进制）

约定：`ch=1`，`addr7=0x48`（即 8-bit 地址 `0x90` 的 7-bit 形式），`reg=0x0000`，`opt=0x03`，设备 ID 读回 `0x90`。

### 8.1 单字节读（`len=1`，`seq=0x1234`）

请求（12 字节）：

```text
0c 00 | 34 12 | 20 | 01 48 01 | 00 00 | 03 | <csum>
```

成功应答（16 字节，`msg_len=14+2`）：

```text
10 00 | 34 12 | 20 | 01 48 01 | 00 00 | 03 | xfer=00 done=01 | bstat=00 | data=90 | <csum>
```

### 8.2 连续读 4 字节（`len=4`）

请求 `msg_len=12`；成功应答 `msg_len=22`：

```text
... | xfer=00 done=04 | bstat=00 00 00 00 | data=90 .. .. .. | <csum>
```

### 8.3 单字节写（写回 `0x90`）

请求（13 字节，`msg_len=12+1`）：

```text
0d 00 | .. .. | 21 | 01 48 01 | 00 00 | 03 | 90 | <csum>
```

成功应答（15 字节）：

```text
0f 00 | .. .. | 21 | ... | xfer=00 done=01 | bstat=00 | <csum>
```

### 8.4 连续写 4 字节

请求 `msg_len=16`（`12+4`）；成功应答 `msg_len=18`：

```text
12 00 | ... | xfer=00 done=04 | bstat=00 00 00 00 | <csum>
```

### 8.5 空地址失败（读 `len=4`，`addr7=0x01`）

应答仍为 22 字节标准形：

```text
xfer=01(NODEV) done=00 | bstat=01 05 05 05 | data=FF FF FF FF
```

### 8.6 坏 checksum

请求其余字段合法但末字节翻错 → 应答标准形，`xfer=06(BAD_FRAME)`，不上总线；`seq` 仍回显。

### 8.7 非法 `len`（如 `0` 或 `>64`）

应答按 `len=1` 组包：读 `msg_len=16` / 写 `msg_len=15`，`xfer=04(PARAM)`，`[7]=01`。

---

## 9. 与旧版差异（对接时勿混用）

| 旧行为 | 当前行为 |
|--------|----------|
| 失败可能走 `ack_fail`，回显请求长度、无 `xfer` | **一律**标准布局 + `xfer` |
| 失败可能把保留字段改成 `0xFFFF` | `seq` **恒回显** |
| 板端可不验 checksum | **必须**验；失败 `BAD_FRAME`，不上总线 |

---

## 10. 实现对照（便于联调）

| 项 | 板端 |
|----|------|
| 入口 | `msg_cmd_0x20` / `msg_cmd_0x21` → `i2c_net_cmd(is_write)` |
| 文件 | `vitis/.../lwip_servers/lwip_common.c` |
| UDP | `udp_cmd.c`，端口 `5555` |
