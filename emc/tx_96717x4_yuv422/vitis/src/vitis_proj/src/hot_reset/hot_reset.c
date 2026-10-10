#include "../bsp.h"

#if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)

#if defined (PLATFORM_ZYNQMP)
#define MPSOC_RESET_REASON_REG 0xFF5E0220  //16bit
#define MPSOC_RESET_CTRL_REG   0xFF5E0218 //寄存器RESET_CTRL
#define MPSOC_SYSTEM_REST      0x10      //复位bit4 (Width=8)
//#define PMU_GLOBAL_BASEADDR      0XFFD80000U
//#define PMU_GLOBAL_PERS_GLOB_GEN_STORAGE4    ( ( PMU_GLOBAL_BASEADDR ) + 0X00000060U )

u8 reset_pl = 0;

void PsSoftwareReset(void)
{
	Xil_Out32(0x00FFD80064, 0x80000000);
    Xil_Out8(MPSOC_RESET_CTRL_REG, MPSOC_SYSTEM_REST);  //复位
}
void getResetReason(void)
{
	u16 status;

	status = Xil_In16(MPSOC_RESET_REASON_REG);
	bsp_printf("\n\rResetReason = 0x%.8X\n\r", status);
	if(status & 0x40)
		bsp_printf("6 Software Debugger Reset\n\r");
	if(status & 0x20)
		bsp_printf("5 Software System Reset\n\r");
	if(status & 0x10)
		bsp_printf("4 External System Reset\n\r");
	if(status & 0x08)
		bsp_printf("3 PS-only Reset\n\r");
	if(status & 0x04)
		bsp_printf("2 Internal System Reset\n\r");
	if(status & 0x02)
		bsp_printf("1 A system error triggered a POR reset\n\r");
	if(status & 0x01)
		bsp_printf("0 the PS_POR_B reset signal pin was asserted\n\r");
	bsp_printf("\n\r");
}

#if 1
void pl_resetn(void)
{ //调用该函数会将mpsoc的pl_resetn0引脚输出0后置1，即低电平复位
	bsp_printf(TXT_GREEN "\r\npl_resetn\r\n" TXT_RST);
	Xil_Out32(0XFF0A0054, 0x00000000U);
	usleep(1000);
	Xil_Out32(0XFF0A0054, 0x80000000U);
}
#else

#define GPIO_MASK_DATA_5_MSW_OFFSET                                                0XFF0A002C
#define GPIO_DIRM_5_OFFSET                                                         0XFF0A0344
#define GPIO_OEN_5_OFFSET                                                          0XFF0A0348
#define GPIO_DATA_5_OFFSET                                                         0XFF0A0054

static void PSU_Mask_Write(unsigned long offset, unsigned long mask,
	unsigned long val)
{
	unsigned long RegVal = 0x0;

	RegVal = Xil_In32(offset);
	RegVal &= ~(mask);
	RegVal |= (val & mask);
	Xil_Out32(offset, RegVal);
}

void pl_resetn(void)
{
    // : PS PL RESET SEQUENCE
    // : FABRIC RESET USING EMIO
    /*Register : MASK_DATA_5_MSW @ 0XFF0A002C</p>
    Operation is the same as MASK_DATA_0_LSW[MASK_0_LSW]
    PSU_GPIO_MASK_DATA_5_MSW_MASK_5_MSW 0x8000
    Maskable Output Data (GPIO Bank5, EMIO, Upper 16bits)
    (OFFSET, MASK, VALUE) (0XFF0A002C, 0xFFFF0000U ,0x80000000U)
    RegMask = (GPIO_MASK_DATA_5_MSW_MASK_5_MSW_MASK | 0 );
    RegVal = ((0x00008000U << GPIO_MASK_DATA_5_MSW_MASK_5_MSW_SHIFT | 0 ) & RegMask); */

    PSU_Mask_Write (GPIO_MASK_DATA_5_MSW_OFFSET ,0xFFFF0000U ,0x80000000U);


    /*############################################################################################################################ */
    /*Register : DIRM_5 @ 0XFF0A0344</p>
    Operation is the same as DIRM_0[DIRECTION_0]
    PSU_GPIO_DIRM_5_DIRECTION_5 0x80000000
    Direction mode (GPIO Bank5, EMIO)
    (OFFSET, MASK, VALUE) (0XFF0A0344, 0xFFFFFFFFU ,0x80000000U)
    RegMask = (GPIO_DIRM_5_DIRECTION_5_MASK | 0 );
    RegVal = ((0x80000000U << GPIO_DIRM_5_DIRECTION_5_SHIFT | 0 ) & RegMask); */

    PSU_Mask_Write (GPIO_DIRM_5_OFFSET ,0xFFFFFFFFU ,0x80000000U);

    /*############################################################################################################################ */
    /*Register : OEN_5 @ 0XFF0A0348</p>
    Operation is the same as OEN_0[OP_ENABLE_0]
    PSU_GPIO_OEN_5_OP_ENABLE_5 0x80000000
    Output enable (GPIO Bank5, EMIO)
    (OFFSET, MASK, VALUE) (0XFF0A0348, 0xFFFFFFFFU ,0x80000000U)
    RegMask = (GPIO_OEN_5_OP_ENABLE_5_MASK | 0 );
    RegVal = ((0x80000000U << GPIO_OEN_5_OP_ENABLE_5_SHIFT | 0 ) & RegMask); */

    PSU_Mask_Write (GPIO_OEN_5_OFFSET ,0xFFFFFFFFU ,0x80000000U);

    /*############################################################################################################################ */
    /*Register : DATA_5 @ 0XFF0A0054</p>
    Output Data
    PSU_GPIO_DATA_5_DATA_5 0x80000000
    Output Data (GPIO Bank5, EMIO)
    (OFFSET, MASK, VALUE) (0XFF0A0054, 0xFFFFFFFFU ,0x80000000U)
    RegMask = (GPIO_DATA_5_DATA_5_MASK | 0 );
    RegVal = ((0x80000000U << GPIO_DATA_5_DATA_5_SHIFT | 0 ) & RegMask); */

    PSU_Mask_Write (GPIO_DATA_5_OFFSET ,0xFFFFFFFFU ,0x80000000U);

    /*############################################################################################################################ */

    usleep(1);

    /*############################################################################################################################ */

    // : FABRIC RESET USING DATA_5 TOGGLE
    /*Register : DATA_5 @ 0XFF0A0054</p>
    Output Data
    PSU_GPIO_DATA_5_DATA_5 0X00000000
    Output Data (GPIO Bank5, EMIO)
    (OFFSET, MASK, VALUE) (0XFF0A0054, 0xFFFFFFFFU ,0x00000000U)
    RegMask = (GPIO_DATA_5_DATA_5_MASK | 0 );
    RegVal = ((0x00000000U << GPIO_DATA_5_DATA_5_SHIFT | 0 ) & RegMask); */

    PSU_Mask_Write (GPIO_DATA_5_OFFSET ,0xFFFFFFFFU ,0x00000000U);

    /*############################################################################################################################ */

    usleep(1);

    /*############################################################################################################################ */

    // : FABRIC RESET USING DATA_5 TOGGLE
    /*Register : DATA_5 @ 0XFF0A0054</p>
    Output Data
    PSU_GPIO_DATA_5_DATA_5 0x80000000
    Output Data (GPIO Bank5, EMIO)
    (OFFSET, MASK, VALUE) (0XFF0A0054, 0xFFFFFFFFU ,0x80000000U)
    RegMask = (GPIO_DATA_5_DATA_5_MASK | 0 );
    RegVal = ((0x80000000U << GPIO_DATA_5_DATA_5_SHIFT | 0 ) & RegMask); */

    PSU_Mask_Write (GPIO_DATA_5_OFFSET ,0xFFFFFFFFU ,0x80000000U);

    /*############################################################################################################################ */
}
#endif

#define CSU_BASEADDR      0XFFCA0000U
#define CSU_CSU_MULTI_BOOT    ( ( CSU_BASEADDR ) + 0X00000010U )
#define APP1_LOCATION 0
#define APP2_LOCATION 0x60

void ModifyMultiBoot(void)
{
	Xil_Out32(CSU_CSU_MULTI_BOOT, APP2_LOCATION);//更改MULTIBOOT
	/* make sure every thing completes */
	dsb();
	isb();
}

#endif // #if defined (PLATFORM_ZYNQMP)



#if defined (PLATFORM_ZYNQ)

/*
 * SLCR Registers
 */
#define PS_RST_CTRL_REG			(XPS_SYS_CTRL_BASEADDR + 0x200)
#define FPGA_RESET_REG			(XPS_SYS_CTRL_BASEADDR + 0x240)
#define RESET_REASON_REG		(XPS_SYS_CTRL_BASEADDR + 0x250)
#define RESET_REASON_CLR		(XPS_SYS_CTRL_BASEADDR + 0x254)
#define REBOOT_STATUS_REG		(XPS_SYS_CTRL_BASEADDR + 0x258)
#define BOOT_MODE_REG			(XPS_SYS_CTRL_BASEADDR + 0x25C)
#define PS_LVL_SHFTR_EN			(XPS_SYS_CTRL_BASEADDR + 0x900)

#define XSLCR_LOCK_ADDR				    (XPS_SYS_CTRL_BASEADDR + 0x00000004U)
#define XSLCR_UNLOCK_ADDR				(XPS_SYS_CTRL_BASEADDR + 0x00000008U)
#define XSLCR_FPGA_RST_CTRL_ADDR        (XPS_SYS_CTRL_BASEADDR + 0x00000240U)

#define PSS_RST_CTRL_REG 	(XPS_SYS_CTRL_BASEADDR + 0x00000200U) //PSS_RST_CTRL寄存器
#define SLCR_UNLOCK_ADDR 	(XPS_SYS_CTRL_BASEADDR + 0x00000008U) //SLCR_UNLOCK寄存器，绝对地址，
#define UNLOCK_KEY 			0xDF0D //使能码
#define PSS_RST_MASK 		0x01 //复位码

#define XDCFG_MULTIBOOT_ADDR_OFFSET_REG 0xF800702C //MULTIBOOT寄存器，绝对地址，
#define XDCFG_UNLOCK_OFFSET_ADDR 0xF8007034 //XDCFG_UNLOCK_寄存器，绝对地址，
#define UNLOCK_XDCFG_KEY 0x757BDF0D //使能码

#define APP1_LOCATION 0
#define APP2_LOCATION 0x60

void ModifyMultiBoot(void)
{
	Xil_Out32(XDCFG_UNLOCK_OFFSET_ADDR , UNLOCK_XDCFG_KEY ); //写使能
	Xil_Out32(XDCFG_MULTIBOOT_ADDR_OFFSET_REG , APP2_LOCATION ); //更改MULTIBOOT
}

void PsSoftwareReset(void)
{
    Xil_Out32(SLCR_UNLOCK_ADDR, UNLOCK_KEY); //写使能
    Xil_Out32(REBOOT_STATUS_REG, 0x1<<24);
    Xil_Out32(XDCFG_MULTIBOOT_ADDR_OFFSET_REG , APP1_LOCATION ); //更改MULTIBOOT
    Xil_Out32(PSS_RST_CTRL_REG, PSS_RST_MASK); //复位
}


void getResetReason(void)
{
	u16 status;

	status =((Xil_In32(REBOOT_STATUS_REG) >> 16) & 0xFF);
	bsp_printf("\n\rResetReason = 0x%.8X\n\r", status);
	if(status & 0x40)
		bsp_printf("6 Power-On Reset (POR), PS_POR_B signal\n\r");
	if(status & 0x20)
		bsp_printf("5 System Reset (SRST_B), SRST_B signal\n\r");
	if(status & 0x10)
		bsp_printf("4 Debug Reset (DBG_RST)\n\r");
	if(status & 0x08)
		bsp_printf("3 SLC soft Reset (SLC_RST)\n\r");
	if(status & 0x04)
		bsp_printf("2 CPU 1 Watchdog Reset (AWDT1_RST)\n\r");
	if(status & 0x02)
		bsp_printf("1 CPU 0 Watchdog Reset (AWDT0_RST)\n\r");
	if(status & 0x01)
		bsp_printf("0 System Watchdog Reset (SWDT_RST)\n\r");
	bsp_printf("\n\r");
}


// assert FPGA Reset Signal
//#define XSLCR_BASEADDR					0xF8000000U
//#define XSLCR_LOCK_ADDR				    (XSLCR_BASEADDR + 0x00000004U)
//#define XSLCR_UNLOCK_ADDR				(XSLCR_BASEADDR + 0x00000008U)
//#define XSLCR_FPGA_RST_CTRL_ADDR        (XSLCR_BASEADDR + 0x00000240U)

/**< SLCR unlock code */
#define XSLCR_LOCK_CODE		    0x0000767BU
#define XSLCR_UNLOCK_CODE		0x0000DF0DU
void pl_resetn(void)
{
	Xil_Out32(XSLCR_UNLOCK_ADDR, XSLCR_UNLOCK_CODE);
	Xil_Out32(XSLCR_FPGA_RST_CTRL_ADDR, 0x0F);

	// do stuff
	usleep(1000);

	// and release the FPGA Reset Signal
	Xil_Out32(XSLCR_FPGA_RST_CTRL_ADDR, 0x00);
	Xil_Out32(XSLCR_LOCK_ADDR, XSLCR_LOCK_CODE);
}



#endif // #if defined (PLATFORM_ZYNQ)


#endif // #if defined (PLATFORM_ZYNQ) || defined (PLATFORM_ZYNQMP)
