/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "../STM32F30x_I2C_CPAL_Driver/inc/stm32f30x_i2c_cpal.h"

#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f30x_usart.h"

#include "defs.h"
#include "USART.h"
#include "i2c.h"

#define SONAR_ADDRESS 0x05

int main(void)
{
    USART_init();

    uint64_t i;
    uint16_t temp_distance;

    //inicjalizacja transfer structures
    CPAL_TransferTypeDef sonarRxStructure, sonarTxStructure;
    uint8_t sonarRxBuffer[2], sonarTxBuffer[2];

    sonarRxStructure.pbBuffer = sonarRxBuffer;
    sonarRxStructure.wAddr1 = 0;
    sonarRxStructure.wAddr2 = 0;

    sonarTxStructure.pbBuffer = sonarTxBuffer;
    sonarTxStructure.wAddr1 = SONAR_ADDRESS;
    sonarRxStructure.wAddr2 = 0;

    RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

	/* Configure the peripheral structure */
	CPAL_I2C_StructInit(&I2C1_DevStructure); /* Set all fields to default values */
	I2C1_DevStructure.CPAL_Mode = CPAL_MODE_MASTER;
	I2C1_DevStructure.wCPAL_Options = CPAL_OPT_NO_MEM_ADDR;
	I2C1_DevStructure.CPAL_ProgModel = CPAL_PROGMODEL_DMA;
	I2C1_DevStructure.pCPAL_I2C_Struct->I2C_Timing = 0x10806291;
	I2C1_DevStructure.pCPAL_TransferRx = & sonarRxStructure;
	I2C1_DevStructure.pCPAL_TransferTx = & sonarTxStructure;
//	/* Initialize CPAL peripheral with the selected parameters */
	CPAL_I2C_Init(&I2C1_DevStructure);

	while(1){
		//SendText("Czesciczolem");
		for(i = 0; i < 10000; i++);

		sonarTxBuffer[0] = 0x10;
		sonarTxBuffer[1] = 0x02;
		sonarTxStructure.wNumData = 2; /* Number of data to be written */
		if (CPAL_I2C_Write(&I2C1_DevStructure) != CPAL_PASS)
		{
			USART_SendData(USART2, 'a');
		/* I2C bus or peripheral is not able to start communication: Error management */
		}

		for(i = 0; i < 10000; i++);

		sonarTxBuffer[0] = 0x20;
		sonarTxStructure.wNumData = 1; /* Number of data to be written */
		if (CPAL_I2C_Write(&I2C1_DevStructure) != CPAL_PASS)
		{
			USART_SendData(USART2, 'b');
		/* I2C bus or peripheral is not able to start communication: Error management */
		}


		while(I2C1_DevStructure.CPAL_State != CPAL_STATE_READY)
		{
		/* Application may perform other tasks while CPAL read operation is ongoing */
		}

		sonarRxStructure.wNumData = 2; /* Number of data to be read */
		if (CPAL_I2C_Read(&I2C1_DevStructure) != CPAL_PASS)
		{
			USART_SendData(USART2, 'c');
		/* I2C bus or peripheral is not able to start communication: Error management */
		}

		/* Wait the end of transfer */
		while(I2C1_DevStructure.CPAL_State != CPAL_STATE_READY)
		{
		/* Application may perform other tasks while CPAL read operation is ongoing */
		}
		/* At this point, data has been received, they can be used by the application
		(compare, process…) */

		temp_distance = sonarRxBuffer[1] + (sonarRxBuffer[0] << 8);

		char array[6];
		for (int i = 5; i >= 0; i--) {
			array[i] = temp_distance % 10 + '0';
			temp_distance /= 10;
		}
		SendText(array);
	}
}

/**************************************************************************************/
