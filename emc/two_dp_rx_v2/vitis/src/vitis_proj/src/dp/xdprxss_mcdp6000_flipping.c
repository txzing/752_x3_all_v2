/***************************** Include Files *********************************/
#include "xdprxss_mcdp6000_flipping.h"
#include "xdprxss.h"
#ifdef XPAR_XIIC_NUM_INSTANCES
#include "xiic.h"
#endif
#ifdef XPAR_XIICPS_NUM_INSTANCES
#include "xiicps.h"
#endif
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"

/************************** Constant Definitions *****************************/



/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/************************** Function Definitions *****************************/

/*****************************************************************************/
void XDpRxSs_McDp6000_init_flipping(void *InstancePtr)
{
	/* Verify argument.*/
	Xil_AssertVoid(InstancePtr != NULL);

	XDpRxSs *DpRxSsPtr = (XDpRxSs *)InstancePtr;
	XDpRxSs_MCDP6000_DpInit_flipping(DpRxSsPtr,
				XDPRXSS_MCDP6000_IIC_SLAVE);
}

int XDpRxSs_MCDP6000_DpInit_flipping(XDpRxSs *DpRxSsPtr, u8 I2CSlaveAddress)
{
	int Result;
	u32 MCDP6000_BS;
	u32 MCDP6000_IC_Rev;

	/* Verify argument.*/
	Xil_AssertNonvoid(DpRxSsPtr != NULL);

	MCDP6000_IC_Rev = XDpRxSs_MCDP6000_GetRegister(DpRxSsPtr, I2CSlaveAddress, 0x0510) & 0xFF00;
	MCDP6000_BS = XDpRxSs_MCDP6000_GetRegister(DpRxSsPtr, I2CSlaveAddress, 0x0510) & 0x1C;

	/* According to AppNote DP Retimer Use Case rev 1.0.0, June 6th, 2017
	 * Table 1, pg7; Exit from disabled state to DP 4 lane with normal
	 * plug orientation. The data lanes need to be swapped so use the
	 * inverted plug orientation instead.
	 */
	/*AUX Setting to add latency for data forwarding*/

	if (MCDP6000_IC_Rev == 0x2100) {
		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0350, 0x0000001F);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}


		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0504, 0x0000704E);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x278C, 0x00000190);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}


		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x010C, 0x0F0F2D24);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0504, 0x0000714E);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0504, 0x0000704E);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x2614, 0x1A070F0F);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x01A0, 0xCC884444);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x01C0, 0x2C00A81E);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x01D0, 0x0000C360);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0178, 0x13471480);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/* Setting MC to be fully transparent mode */
		/* Need to set bit10 for Xilinx fixed length mode. */
		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0908, 0x0C00);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0B00, 0x0000);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0B04, 0x0000);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x090C, 0x02020000);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0A00, 0x55801E10);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*AUX Swing Adjustment - For lower DP1.4 swing guidance of 400 mV*/
		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x1608, 0x00748404);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}
		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x2608, 0x00748404);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}
	} else if (MCDP6000_IC_Rev==0x3100) {
		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0350, 0x0000001F);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0504, 0x0001704E);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x01C0, 0x2C002C9E);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x092C, 0x5555A5A5);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0900, 0x04010506);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0178, 0x13471480);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x01A0, 0xCC884444);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x2614, 0x19890F0F);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		if (MCDP6000_BS == 0x18) {
			Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
						      0x2340, 0x00000500);
			if (Result != XST_SUCCESS) {
				return XST_FAILURE;
			}

			Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
						      0x2540, 0x00000500);
			if (Result != XST_SUCCESS) {
				return XST_FAILURE;
			}
		} else if (MCDP6000_BS == 0x8) {
			Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
						      0x2240, 0x00000500);
			if (Result != XST_SUCCESS) {
				return XST_FAILURE;
			}

			Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
						      0x2440, 0x00000500);
			if (Result != XST_SUCCESS) {
				return XST_FAILURE;
			}
		}
		/*AUX Swing Adjustment - For lower DP1.4 swing guidance of 400 mV*/
		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x1608, 0x00748404);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}
		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x2608, 0x00748404);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}
	} else if (MCDP6000_IC_Rev==0x3200) {
		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0350, 0x0000001F);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0504, 0x0001704E);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x2614, 0x19890F0F);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x01D8, 0x00000601);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0660, 0x00005011);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x067C, 0x00000001);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x0908, 0x00000866);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
					      0x090C, 0x04020000);
		if (Result != XST_SUCCESS) {
			return XST_FAILURE;
		}

		if (MCDP6000_BS == 0x18) {
			Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
						      0x2340, 0x00000500);
			if (Result != XST_SUCCESS) {
				return XST_FAILURE;
			}

			Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
						      0x2540, 0x00000500);
			if (Result != XST_SUCCESS) {
				return XST_FAILURE;
			}
		} else if (MCDP6000_BS == 0x8) {
			Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
						      0x2240, 0x00000500);
			if (Result != XST_SUCCESS) {
				return XST_FAILURE;
			}

			Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
						      0x2440, 0x00000500);
			if (Result != XST_SUCCESS) {
				return XST_FAILURE;
			}
		}
	} else {
		Result = XST_SUCCESS;
	}

	return Result;
}

int XDpRxSs_MCDP6000_ResetDpPath_flipping(XDpRxSs *DpRxSsPtr, u8 I2CSlaveAddress)
{
	int Result;

	/* Verify argument.*/
	Xil_AssertNonvoid(DpRxSsPtr != NULL);

	/* Set Reset bits : This is actually getting out from DP mode.
	 * Eventually reset the chip. */
	Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
				      0x0504, 0x1714E);
	if (Result != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Clear Reset bits  */
	Result = XDpRxSs_MCDP6000_SetRegister(DpRxSsPtr, I2CSlaveAddress,
				      0x0504, 0x1704E);
	if (Result != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return Result;
}

