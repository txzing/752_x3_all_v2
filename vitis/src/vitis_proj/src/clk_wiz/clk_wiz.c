#include "../bsp.h"

#if defined (XPAR_XCLK_WIZ_NUM_INSTANCES)

#if (XPAR_XCLK_WIZ_NUM_INSTANCES >= 1U)
XClk_Wiz ClkWiz_Dynamic0;
#endif
#if (XPAR_XCLK_WIZ_NUM_INSTANCES >= 2U)
XClk_Wiz ClkWiz_Dynamic1;
#endif
#if (XPAR_XCLK_WIZ_NUM_INSTANCES >= 3U)
XClk_Wiz ClkWiz_Dynamic3;
#endif

/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
*
* This is the Wait_For_Lock function, it will wait for lock to settle change
* frequency value
*
* @param	CfgPtr_Dynamic provides pointer to clock wizard dynamic config
*
* @return
*		- Error 0 for pass scenario
*		- Error > 0 for failure scenario
*
* @note		None
*
******************************************************************************/
int Wait_For_Lock(XClk_Wiz_Config *CfgPtr_Dynamic)
{
	u32 Count = 0;
	u32 Error = 0;

	while(!(*(u32 *)(CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK)) {
		if(Count == 10000) {
			Error++;
			break;
		}
		Count++;
        }
    return Error;
}

/*****************************************************************************/
/**
 *
 * XClk_Wiz wizard dynamic reconfig.
 *
 * @param	None.
 *
 * @return
 *		- XST_SUCCESS if XClk_Wiz dynamic reconfig ran successfully.
 *		- XST_FAILURE if XClk_Wiz dynamic reconfig failed.
 *
 * @note		None.
 *
 ****************************************************************************/
int XClk_Wiz_dynamic_reconfig(XClk_Wiz * ClkWizInstance, u32 DeviceId)
{
    XClk_Wiz_Config *CfgPtr_Dynamic;
    u32 Count = 0;
    u32 Divide = 0;
    u32 Multiply_Int = 0;
    u32 Multiply_Frac = 0;
    u32 Divide0_Int = 0;
    u32 Divide0_Frac = 0;
    u32 Divide1_Int = 0;
    u32 Divide1_Frac = 0;
    u32 Divide2_Int = 0;
    u32 Divide2_Frac = 0;
    u32 Divide3_Int = 0;
    u32 Divide3_Frac = 0;
    int Status;

    /*
     * Get the CLK_WIZ Dynamic reconfiguration driver instance
     */
    CfgPtr_Dynamic = XClk_Wiz_LookupConfig(DeviceId);
    if(!CfgPtr_Dynamic)
    {
        return XST_FAILURE;
    }

    /*
     * Initialize the CLK_WIZ Dynamic reconfiguration driver
     */
    Status = XClk_Wiz_CfgInitialize(ClkWizInstance, CfgPtr_Dynamic, CfgPtr_Dynamic->BaseAddr);
    if(Status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }

    Status = Wait_For_Lock(CfgPtr_Dynamic);
    if(Status)
    {
    	bsp_printf(TXT_RED "\n ERROR: Clock is not locked for default frequency"
                   " : 0x%x\n\r"  TXT_RST,
                   *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK);
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
//    	return XST_FAILURE;
    }

    /* SW reset applied */
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x00) = 0xA;

    if(*(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK)
    {
    	bsp_printf(TXT_RED "\n ERROR: Clock is locked : 0x%x \t expected "
                   "0x00\n\r" TXT_RST,
                   *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK);
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
//    	return XST_FAILURE;
    }

    /* Wait cycles after SW reset */
    for(Count = 0; Count < 2000; Count++)
        ;

    Status = Wait_For_Lock(CfgPtr_Dynamic);
    if(Status)
    {
    	bsp_printf(TXT_RED "\n ERROR: Clock is not locked after SW reset :"
                   "0x%x \t Expected  : 0x1\n\r" TXT_RST,
                   *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK);
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
//    	return XST_FAILURE;
    }


#if (defined R1080)
    // in=200
    // out1 148.5
    Multiply_Int = 37;
    Multiply_Frac = 125;
    Divide = 5;
    Divide0_Int = 10;
    Divide0_Frac = 0;

#elif (defined R4K)
    //in=200
    //out1 297
    Multiply_Int = 37;
    Multiply_Frac = 125;
    Divide = 5;
    Divide0_Int = 5;
    Divide0_Frac = 0;


#else
    Multiply_Int = 37;
    Multiply_Frac = 125;
    Divide = 5;
    Divide0_Int = 10;
    Divide0_Frac = 0;

#endif


    /* Configuring Multiply and Divide values */
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x200) = (Multiply_Frac << 16) | (Multiply_Int << 8) | Divide;
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x204) = 0x00;

    /* Configuring Multiply and Divide values */
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x208) = (Divide0_Frac << 8) | Divide0_Int;
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x20C) = 0x00;

    /* Configuring Multiply and Divide values */
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x214) = (Divide1_Frac << 8) | Divide1_Int;
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x218) = 0x00;

    /* Configuring Multiply and Divide values */
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x220) = (Divide2_Frac << 8) | Divide2_Int;
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x224) = 0x00;

    /* Configuring Multiply and Divide values */
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x22C) = (Divide3_Frac << 8) | Divide3_Int;
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x230) = 0x00;

    /* Load Clock Configuration Register values */
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x25C) = 0x07;

    if(*(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK)
    {
    	bsp_printf("\n ERROR: Clock is locked : 0x%x \t expected "
                   "0x00\n\r",
                   *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK);
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
    }

    /* Clock Configuration Registers are used for dynamic reconfiguration */
    *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x25C) = 0x02;

    Status = Wait_For_Lock(CfgPtr_Dynamic);
    if(Status)
    {
    	bsp_printf(TXT_RED "\n ERROR: Clock is not locked : 0x%x \t Expected "
                   ": 0x1\n\r" TXT_RST,
                   *(u32 *) (CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK);
    	bsp_printf(TXT_RED "\r\n__FILE__:%s, __LINE__:%d\r\n" TXT_RST,__FILE__, __LINE__);
    }

    return XST_SUCCESS;
}

int clkwiz_config(void)
{
	int Status;

    // dynamic config clkwiz
    Status = XClk_Wiz_dynamic_reconfig(&ClkWiz_Dynamic0, XPAR_CLK_WIZ_0_DEVICE_ID);
    if (Status != XST_SUCCESS)
    {
    	bsp_printf(TXT_RED "XClk_Wiz0 dynamic reconfig failed.\r\n" TXT_RST);
    	return XST_FAILURE;
    }
    bsp_printf("XClk_Wiz0 dynamic reconfig ok\n\r");

    return XST_SUCCESS;
}


int clkwiz_config_setrate(XClk_Wiz *ClkWizInstance, u32 DeviceId, float OutputFreq)
{
    XClk_Wiz_Config *CfgPtr_Dynamic;
    u32 Multiply_Int = 0, Multiply_Frac = 0, Divide = 1;  // 固定输入分频
    u32 Divide0_Int = 1, Divide0_Frac = 0;
    u32 best_mult_int = 0, best_mult_frac = 0;
    u32 best_div_int = 0, best_div_frac = 0;
    u32 best_generated_freq_int = 0;  // 放大 1000 倍表示生成频率
    u32 target_freq_int = (u32)(OutputFreq * 1000);  // 目标频率放大 1000 倍
    u32 best_error = 0xFFFFFFFF;  // 初始化为最大值
    int Status;

    // 参数校验
    if (OutputFreq <= 0)
    {
        xil_printf("ERROR: Invalid Output Frequency. TargetFreq: %d.%03d MHz\r\n",
                   target_freq_int / 1000, target_freq_int % 1000);
        return XST_FAILURE;
    }

    // 获取动态重配置驱动实例
    CfgPtr_Dynamic = XClk_Wiz_LookupConfig(DeviceId);
    if (!CfgPtr_Dynamic) {
        return XST_FAILURE;
    }

    // 初始化动态重配置驱动
    Status = XClk_Wiz_CfgInitialize(ClkWizInstance, CfgPtr_Dynamic, CfgPtr_Dynamic->BaseAddr);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 等待初始时钟锁定
    Status = Wait_For_Lock(CfgPtr_Dynamic);
    if (Status) {
        bsp_printf("ERROR: Clock not locked at default frequency.\r\n");
        return XST_FAILURE;
    }

    // 遍历所有可能的倍频和分频值，寻找最佳配置
    for (u32 mult_int = 2; mult_int <= 64; mult_int++)
    {  // 倍频范围：2~64
        for (u32 mult_frac = 0; mult_frac <= 875; mult_frac++)
        {  // 倍频小数范围：0~875
            for (u32 div_int = 1; div_int <= 128; div_int++)
            {  // 输出整数分频范围：1~128
                for (u32 div_frac = 0; div_frac <= 875; div_frac++)
                {  // 输出小数分频范围：0~875
                    // 计算当前配置生成的频率（放大 1000 倍以避免浮点运算）
                    u32 mult = mult_int * 1000 + mult_frac;  // 倍频（整数 + 小数）
                    u32 div = div_int * 1000 + div_frac;     // 分频（整数 + 小数）
                    u32 generated_freq_int = (INPUT_FREQ_INT * mult) / div;

                    // 计算误差
                    u32 error = (generated_freq_int > target_freq_int) ?
                                (generated_freq_int - target_freq_int) :
                                (target_freq_int - generated_freq_int);

                    // 检查是否为最佳配置
                    if (error < best_error)
                    {
                        best_error = error;
                        best_generated_freq_int = generated_freq_int;
                        best_mult_int = mult_int;
                        best_mult_frac = mult_frac;
                        best_div_int = div_int;
                        best_div_frac = div_frac;

                        // 如果误差为 0，提前退出
                        if (best_error == 0)
                        {
                            goto configuration_found;
                        }
                    }
                }
            }
        }
    }

configuration_found:

// 如果 Divide 的整数部分为 1，自动调整参数
  if (best_div_int == 1)
  {
      best_mult_int *= 2;
      best_mult_frac *= 2;
      best_div_int *= 2;
      best_div_frac *= 2;

      // 处理小数部分溢出
      if (best_mult_frac >= 1000)
      {
          best_mult_int += best_mult_frac / 1000;
          best_mult_frac %= 1000;
      }
      if (best_div_frac >= 1000)
      {
          best_div_int += best_div_frac / 1000;
          best_div_frac %= 1000;
      }

      xil_printf("INFO: Divide integer part was 1, parameters adjusted:\r\n");
      xil_printf("!! Multiply: %d.%03d\r\n", best_mult_int, best_mult_frac);
      xil_printf("!! Divide: %d.%03d\r\n", best_div_int, best_div_frac);
  }
    // 打印最佳配置
    bsp_printf("Best Configuration:\r\n");
    bsp_printf("  Multiply: %d.%03d\r\n", best_mult_int, best_mult_frac);
    bsp_printf("  Divide: %d.%03d\r\n", best_div_int, best_div_frac);
    bsp_printf("  Generated Frequency: %d.%03d MHz\r\n",
               best_generated_freq_int / 1000, best_generated_freq_int % 1000);
    bsp_printf("  Target Frequency: %d.%03d MHz\r\n",
               target_freq_int / 1000, target_freq_int % 1000);


    // 软件复位
    volatile u32 *reset_reg = (volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x00);
    *reset_reg = 0xA;

    // 延时等待
    for (volatile u32 i = 0; i < 2000; i++)
        ;

    // 再次检查时钟锁定
    Status = Wait_For_Lock(CfgPtr_Dynamic);
    if (Status) {
        bsp_printf("ERROR: Clock not locked after reset.\r\n");
        return XST_FAILURE;
    }

    // 配置倍频和分频值
    volatile u32 *cfg_reg = (volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x200);
    *cfg_reg = (best_mult_frac << 16) | (best_mult_int << 8) | Divide;
    *(cfg_reg + 1) = 0x00;

    volatile u32 *out_div_reg = (volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x208);
    *out_div_reg = (best_div_frac << 8) | best_div_int;
    *(out_div_reg + 1) = 0x00;

    // 加载时钟配置寄存器
    volatile u32 *load_reg = (volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x25C);
    *load_reg = 0x07;

    // 检查是否锁定
    if (*(volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK) {
        bsp_printf("ERROR: Clock is locked unexpectedly.\r\n");
        return XST_FAILURE;
    }

    // 使用动态配置寄存器
    *load_reg = 0x02;

    // 最终锁定检查
    Status = Wait_For_Lock(CfgPtr_Dynamic);
    if (Status) {
        bsp_printf("ERROR: Clock not locked after dynamic reconfiguration.\r\n");
        return XST_FAILURE;
    }

    bsp_printf("SUCCESS: Clock configured successfully.\r\n");
    return XST_SUCCESS;
}

/**
 * clkwiz_config_setrate_multi - 配置多路输出时钟频率
 *
 * @ClkWizInstance: 指向时钟配置实例的指针
 * @DeviceId: 时钟设备 ID，用于获取配置
 * @OutputFreqs: 输出时钟目标频率数组（单位 MHz）
 * @NumOutputs: 输出时钟的数量
 *
 * 功能:
 * 该函数根据输入的目标频率数组配置多个输出时钟，并动态调整倍频和分频参数。
 * 如果发现某一路输出的分频器整数部分为 1，会自动调整倍频和分频参数以确保硬件稳定运行。
 *
 * 返回值:
 * - XST_SUCCESS: 配置成功
 * - XST_FAILURE: 配置失败或参数无效
 *
 * 使用案列，未实际测试是否可惜
 *   // 输出时钟目标频率数组
    float OutputFreqs[] = {150.0, 180.0, 220.0};
    int NumOutputs = sizeof(OutputFreqs) / sizeof(OutputFreqs[0]);

    // 配置时钟
    int Status = clkwiz_config_setrate_multi(&ClkWizInstance, DeviceId, OutputFreqs, NumOutputs);
    if (Status == XST_SUCCESS) {
        xil_printf("All clocks configured successfully.\r\n");
    } else {
        xil_printf("Clock configuration failed.\r\n");
    }
 */

int clkwiz_config_setrate_multi(XClk_Wiz *ClkWizInstance, u32 DeviceId, float OutputFreqs[], int NumOutputs) {
    XClk_Wiz_Config *CfgPtr_Dynamic;
	u32 Multiply_Int = 0, Multiply_Frac = 0; // 当前倍频整数和小数部分
	u32 Divide_Ints[NumOutputs];            // 每一路输出的分频整数部分
	u32 Divide_Fracs[NumOutputs];           // 每一路输出的分频小数部分
	u32 Best_Multiply_Int = 0, Best_Multiply_Frac = 0; // 最佳倍频值（整数和小数）
	u32 Best_Divide_Ints[NumOutputs];                  // 最佳分频整数部分
	u32 Best_Divide_Fracs[NumOutputs];                 // 最佳分频小数部分
	u32 Best_Error = 0xFFFFFFFF;           // 初始化为最大误差
	u32 Best_Generated_Freqs[NumOutputs];  // 每一路输出的生成频率
	u32 Target_Freq_Ints[NumOutputs];      // 每一路目标频率（放大 1000 倍）
	u32 InputFreqInt = INPUT_FREQ_INT;     // 输入频率（单位 kHz，放大 1000 倍）
	int Status;

    // 参数校验
    for (int i = 0; i < NumOutputs; i++) {
        if (OutputFreqs[i] <= 0) {
            xil_printf("ERROR: Invalid Output Frequency for Output[%d]. TargetFreq: %.3f MHz\r\n", i, OutputFreqs[i]);
            return XST_FAILURE;
        }
        Target_Freq_Ints[i] = (u32)(OutputFreqs[i] * 1000); // 将频率放大1000倍，避免浮点运算
    }

    // 获取动态重配置驱动实例
    CfgPtr_Dynamic = XClk_Wiz_LookupConfig(DeviceId);
    if (!CfgPtr_Dynamic) {
        return XST_FAILURE;
    }

    // 初始化动态重配置驱动
    Status = XClk_Wiz_CfgInitialize(ClkWizInstance, CfgPtr_Dynamic, CfgPtr_Dynamic->BaseAddr);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // 等待初始时钟锁定
    Status = Wait_For_Lock(CfgPtr_Dynamic);
    if (Status) {
        xil_printf("ERROR: Clock not locked at default frequency.\r\n");
        return XST_FAILURE;
    }

    // 遍历所有可能的倍频值，寻找最佳配置
    xil_printf("INFO: Searching for the best configuration...\r\n");
    for (u32 mult_int = 2; mult_int <= 64; mult_int++) {       // 倍频范围：2~64
        for (u32 mult_frac = 0; mult_frac <= 875; mult_frac++) {  // 小数步长为 1
            u32 Total_Error = 0;
            u32 Generated_Freqs[NumOutputs];

            // 对每个输出计算分频值
            for (int i = 0; i < NumOutputs; i++) {
                u32 target_freq = Target_Freq_Ints[i];
                u32 mult = mult_int * 1000 + mult_frac; // 当前倍频
                u32 div = (InputFreqInt * mult) / target_freq;

                // 分离整数和小数部分
                Divide_Ints[i] = div / 1000;
                Divide_Fracs[i] = div % 1000;

                // 计算生成的频率
                Generated_Freqs[i] = (InputFreqInt * mult) /
                                     (Divide_Ints[i] * 1000 + Divide_Fracs[i]);

                // 计算误差
                u32 error = (Generated_Freqs[i] > target_freq) ?
                            (Generated_Freqs[i] - target_freq) :
                            (target_freq - Generated_Freqs[i]);
                Total_Error += error;
            }

            // 检查是否为最佳配置
            if (Total_Error < Best_Error) {
                Best_Error = Total_Error;
                Best_Multiply_Int = mult_int;
                Best_Multiply_Frac = mult_frac;

                // 保存最佳分频值
                for (int i = 0; i < NumOutputs; i++) {
                    Best_Divide_Ints[i] = Divide_Ints[i];
                    Best_Divide_Fracs[i] = Divide_Fracs[i];
                    Best_Generated_Freqs[i] = Generated_Freqs[i];
                }

                // 如果误差为 0，提前退出
                if (Best_Error == 0) {
                    goto configuration_found;
                }
            }
        }
    }

    configuration_found:
    // 在最佳配置中，如果任何一个 Divide 的整数部分为 1，调整参数
    for (int i = 0; i < NumOutputs; i++) {
        if (Best_Divide_Ints[i] == 1) { // 检查分频整数部分是否为 1
            Best_Multiply_Int *= 2; // 倍频值乘 2
            Best_Multiply_Frac *= 2; // 倍频小数部分乘 2
            Best_Divide_Ints[i] *= 2; // 分频整数部分乘 2
            Best_Divide_Fracs[i] *= 2; // 分频小数部分乘 2

            // 处理小数部分溢出
            if (Best_Multiply_Frac >= 1000) {
                Best_Multiply_Int += Best_Multiply_Frac / 1000;
                Best_Multiply_Frac %= 1000;
            }
            if (Best_Divide_Fracs[i] >= 1000) {
                Best_Divide_Ints[i] += Best_Divide_Fracs[i] / 1000;
                Best_Divide_Fracs[i] %= 1000;
            }

            xil_printf("INFO: Divide integer part was 1 for Output[%d], parameters adjusted:\r\n", i);
            xil_printf("!! Multiply: %d.%03d\r\n", Best_Multiply_Int, Best_Multiply_Frac);
            xil_printf("!! Divide: %d.%03d\r\n", Best_Divide_Ints[i], Best_Divide_Fracs[i]);
        }
    }

        // 打印最佳配置
        xil_printf("Best Configuration:\r\n");
        xil_printf("  Multiply: %d.%03d\r\n", Best_Multiply_Int, Best_Multiply_Frac);
        for (int i = 0; i < NumOutputs; i++) { // 确保遍历所有输出时钟
            xil_printf("  Output[%d] Divide: %d.%03d, Generated Frequency: %d.%03d MHz, Target Frequency: %.3f MHz\r\n",
                       i, Best_Divide_Ints[i], Best_Divide_Fracs[i],
                       Best_Generated_Freqs[i] / 1000, Best_Generated_Freqs[i] % 1000,
                       OutputFreqs[i]);
        }


    // 配置倍频值
    volatile u32 *cfg_reg = (volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x200);
    *cfg_reg = (Best_Multiply_Frac << 16) | (Best_Multiply_Int << 8);
    *(cfg_reg + 1) = 0x00;

    // 配置每个输出时钟的分频值
    for (int i = 0; i < NumOutputs; i++) {
        volatile u32 *out_div_reg = (volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x208 + i * 0xC);
        *out_div_reg = (Best_Divide_Fracs[i] << 8) | Best_Divide_Ints[i];
        *(out_div_reg + 1) = 0x00;
    }

    // 加载时钟配置寄存器
    volatile u32 *load_reg = (volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x25C);
    *load_reg = 0x07;

    // 检查是否锁定
    if (*(volatile u32 *)(CfgPtr_Dynamic->BaseAddr + 0x04) & CLK_LOCK) {
        bsp_printf("ERROR: Clock is locked unexpectedly.\r\n");
        return XST_FAILURE;
    }

    // 使用动态配置寄存器
    *load_reg = 0x02;
    // 检查是否锁定
    Status = Wait_For_Lock(CfgPtr_Dynamic);
    if (Status) {
        xil_printf("ERROR: Clock not locked after dynamic reconfiguration.\r\n");
        return XST_FAILURE;
    }

    xil_printf("SUCCESS: All clocks configured successfully.\r\n");
    return XST_SUCCESS;
}


#if defined (XPAR_XVTC_NUM_INSTANCES)
int clkwiz_vtc_cfg(void)
{
    u32 Status;
    // dynamic config clkwiz
    Status = XClk_Wiz_dynamic_reconfig(&ClkWiz_Dynamic0, XPAR_CLK_WIZ_0_DEVICE_ID);
    if (Status != XST_SUCCESS)
    {
      xil_printf("XClk_Wiz0 dynamic reconfig failed.\r\n");
      return XST_FAILURE;
    }
    xil_printf("XClk_Wiz0 dynamic reconfig ok\n\r");

    // vtc configuration
    XVtc_Config *VtcConfig0;
    VtcConfig0 = XVtc_LookupConfig(XPAR_VTC_0_DEVICE_ID);
    Status = XVtc_CfgInitialize(&VtcInst0, VtcConfig0, VtcConfig0->BaseAddress);
    if(Status != XST_SUCCESS)
    {
      xil_printf("VTC0 Initialization failed %d\r\n", Status);
      return(XST_FAILURE);
    }
    xil_printf("VTC0 Initialization ok\r\n");

#if (defined R1080)
    vtiming_gen_run(&VtcInst0, VIDEO_RESOLUTION_1080P, 0);
#elif  (defined R4K)
    vtiming_gen_run(&VtcInst0, VIDEO_RESOLUTION_4K, 0);
#else
    vtiming_gen_run(&VtcInst0, VIDEO_RESOLUTION_1920X720, 0);
#endif
/*******************************************************************************/
    xil_printf("clkwiz_vtc_cfg done!\r\n");

    return XST_SUCCESS;
}
#endif

#endif // XPAR_XCLK_WIZ_NUM_INSTANCES


