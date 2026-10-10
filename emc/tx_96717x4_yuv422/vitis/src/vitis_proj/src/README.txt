Xilinx Common Bare-Metal API

. 这里的 linker 文件仅供参考

. 基本的配置选择集中到 config.h, 按需要修改 bsp.h (一般不需要动)，按需要复制需要的模块。

. 根据 bsp 信息，使能相应的模块或功能，并修改为具体项目使用，尽可能兼容 microblaze/zynq/zynqmp/r5/ppc

. main.c 仅作为示例，实际上不需要的就去掉，这里的宏开关只为耦合需要的模块(展示需要具体模块的调用方法，及方便搜索)

. 中断系统统一放到 platform 模块，但是具体的外设初始化和回调函数设置尽可能放具体外设模块里, 主循环之前需要使能中断系统。

. 库代码设计原则: 各模块尽量解耦，可调用函数接口和全局变量放在头文件(extern声明全局变量)，模块内尽量用static函数, 模块内尽可能给出使用示例。

. dbg_trace.h 提供一个简单的可开关控制打印系统

. bitmanip.h 提供常用宏定义

. lwip网络库应的定时器模块，可以采用 platform 展示的 timer 设置, 也可以换掉 (硬核有定时器，但也可以用axi_timer, 这样有利于代码一致性)

. io 模拟的 emio_i2c、xgpio_i2c是vivado heir对应的代码, xgpio_i2c 等在linux环境使用不如 axi_iic 方便, ps iic 不支持 stretching, i2c应尽可能采用 axi_iic。

. eeprom读写库支持4种接口模式(psiic、xiic、xgpio、emio)，支持跳页连续写。

. qspi flash 的支持自动检测常见器件，具体使用时还需要调整 buffer 指针。没有设计擦除保护，即擦除 sector 之前没有预读，这样的话最后擦除的sector后段数据可能丢失。

. rs485是一个 vivado heir对应的代码，如果仅打印可选这个。modbus_rtu 也对应一个 vivado heir(可以作为主从两用)，从机库会调用rs485、uart_fifo等额外的库，3.5T判断用的 timer 尽量采用 axi_timer。 



TODO：

* clk_wiz 添加给频率自动计算

* vtc 还没有利用xvidc已经定义的时序，可以改进为使用库定义的相关用法

* IAP升级只保留两种简单升级，暂不进行md5校验等升级交互。也暂不进行分区升级和指定地址升级。这样可以直接用 python 脚本或 NetAssist.exe 进行升级，网络环境时应应该比较好。IAP 刷新之后复位控制添加。

* 上位机没有必要做直接地址读写操作，全部采用 msg 消息反馈机制

* 还是有必要做golden区，放一个通用的可升级固件可打印无其他功能的程序

* Modbus(主)，Can 通讯等添加

* 串口打印菜单的设计, 修改 eeprom有关的设置和 配置表，i2c/spi等调试操作

* psuart 来实现 modbus_rtu, 可以动态调整波特率

* 可以保留eeprom有关的设置和 配置表 更新保存编辑什么的

* 使用 RTOS 可以简化设计，专门任务对于需要的模块。

* 
