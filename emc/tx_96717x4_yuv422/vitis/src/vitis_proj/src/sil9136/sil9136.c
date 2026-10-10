#include "../bsp.h"

#if defined (XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID) && defined (SIL9136)

void sil9136_config(void)
{
//    // for SiI9134
//    xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x08, 0x35, STRETCH_OFF);
//    xgpio_i2c_reg8_write(I2C_NO_0, 0x7a>>1, 0x2f, 0x00, STRETCH_OFF);

	// for sil1136/sil9136-3 @    /参考资料/spjk视频输出接口转换
	//                       @    /开发板资料/lattice/Embedded_Vision_DevelopmentKit/EmbeddedVisionDevelopmentKit/UserManual/HDMI-VIP-Output-Bridge-Board
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0xC7, 0x00, STRETCH_OFF); // HDMI configuration
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x1E, 0x00, STRETCH_OFF); // Power Up Transmitter
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x08, 0x60, STRETCH_OFF); // Input Bus/Pixel Repetition (default)
#ifdef SIL9136_1080P60
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x00, 0x02, STRETCH_OFF); // Pixel Clock
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x01, 0x3A, STRETCH_OFF); // Pixel Clock
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x02, 0x70, STRETCH_OFF); // Frame Rate * 100
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x03, 0x17, STRETCH_OFF); // Frame Rate * 100
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x04, 0x98, STRETCH_OFF); // Pixels
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x05, 0x08, STRETCH_OFF); // Pixels
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x06, 0x65, STRETCH_OFF); // Lines
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x07, 0x04, STRETCH_OFF); // Lines
#endif
#ifdef SIL9136_4k30
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x00, 0x04, STRETCH_OFF); // Pixel Clock
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x01, 0x74, STRETCH_OFF); // Pixel Clock
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x02, 0xB8, STRETCH_OFF); // Frame Rate * 100
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x03, 0x0B, STRETCH_OFF); // Frame Rate * 100
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x04, 0x30, STRETCH_OFF); // Pixels
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x05, 0x11, STRETCH_OFF); // Pixels
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x06, 0xCA, STRETCH_OFF); // Lines
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x07, 0x08, STRETCH_OFF); // Lines
#endif

	//xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x09, 0x00, STRETCH_OFF); //
	//xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x0A, 0x00, STRETCH_OFF); //

	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x1A, 0x00, STRETCH_OFF); //
}

#endif // XPAR_XGPIO_I2C_0_AXI_GPIO_0_DEVICE_ID && SIL9136


/*
usage:
assume you have a SIL9136, and pclk have been set in some clk_wiz
call follows before main_loop
```
#if defined(__SIL9136_H__)
    sil9136_config();
#endif
```
*/
