# 752_x3_all_v2

ZynqMP 相关工程：**Vivado 硬件** + **Vitis 软件**，涵盖多路视频 / LVDS / 以太网控制等功能的开发与迭代版本。

## 仓库结构

| 路径 | 说明 |
|------|------|
| `vivado/` | Vivado 工程脚本、BD、IP 仓库（`ip_repo`）、约束与 Makefile 流程 |
| `vitis/` | Vitis 应用工程脚本、源码（如 `src/vitis_proj`）、`xsa` 等 |
| 根目录下若干中文目录名 | 不同功能阶段或实验分支的说明 / 快照（按目录名区分） |

更细的脚本用法见：

- `vivado/readme.txt` — Linux / Windows 下 `proj_gen`、`proj_build`、导出 BD、复制 XSA 等
- `vitis/readme.txt` — Windows / Linux 下 `proj_gen`、`proj_build`、`flash` 等

## 环境与工具链

- **Vivado / Vitis：2020.1**（文档中默认路径示例：`D:\Xilinx\Vivado\2020.1` 或 `/opt/Xilinx/Vivado/2020.1`）
- Windows 下若使用脚本中的 Bash 流程，需配合 **MSYS2** 等环境（参见 `vitis/readme.txt`）

## 克隆后本地需生成的目录（不纳入 Git）

以下目录体积大或由工具自动生成，已在 **`.gitignore`** 中排除，**克隆本仓库后需在本地重新生成**：

- `vivado_proj/` — Vivado 工程输出目录  
- `ip_cache/` — Vivado IP 缓存  
- `sdk_workspace/` — Vitis 工作区  

按 `vivado/readme.txt`、`vitis/readme.txt` 中的 `proj_gen` / `proj_build` 等脚本在本地创建工程即可。

## 简要构建流程

1. **硬件**：在 `vivado/` 下按 readme 执行 `export_bd.sh` → `proj_gen.sh` / `proj_build.sh`（或 `make` 流程），得到 bitstream 与 XSA。  
2. **同步 XSA**：将生成的 XSA 拷贝到 Vitis 侧（如 `cp_xsa_to_vitis.sh` / 项目约定路径）。  
3. **软件**：在 `vitis/` 下按 readme 生成并编译应用，按需 `flash`。

镜像烧写地址等说明见 `vitis/readme.txt`（例如 `0x0`、`0x5000000`）。

## Git 与远程仓库

- 默认分支：`main`  
- 远程示例：`git@github.com:txzing/752_x3_all_v2.git`（SSH）或 HTTPS 等价地址  

推送前请确保本机已配置 GitHub SSH 公钥或使用 HTTPS + Personal Access Token。

## 变更记录

| 日期 | 说明 |
|------|------|
| 2026-05 | 初始化 Git 版本控制；关联 GitHub 远程仓库；增加根目录 `README.md`；通过 `.gitignore` 排除 `vivado_proj/`、`ip_cache/`、`sdk_workspace/`，减小仓库体积并避免将生成物误提交。 |

---

*仓库维护：txzing*
