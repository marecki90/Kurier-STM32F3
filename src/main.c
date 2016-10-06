/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


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
    I2C_init();

    int i;
    uint8_t data[2];
    uint16_t temp_distance;


	while(1){
		//SendText("Czesc");


		I2C_TransferHandling(I2Cx, SONAR_ADDRESS, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == RESET);
		I2C_SendData(I2Cx, 0x10);

		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TCR) == RESET);

		I2C_TransferHandling(I2Cx, SONAR_ADDRESS, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == RESET);

		I2C_SendData(I2Cx, 0x02);

		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == RESET);
		I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);

		for(i = 0; i < 100000; i++);

		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET);

		I2C_TransferHandling(I2Cx, SONAR_ADDRESS, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == RESET);

		//Send the address of the register you wish to read
		I2C_SendData(I2Cx, 0x20);

		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TC) == RESET);

		I2C_TransferHandling(I2Cx, SONAR_ADDRESS, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

        while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
	    data[0] = I2C_ReceiveData(I2C1);

	    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);
	    data[1] = I2C_ReceiveData(I2C1);

	    //Wait for the stop condition to be sent
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == RESET);

		//Clear the stop flag for next transfers
		I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);


		temp_distance = data[1] + (data[0] << 8);

		char array[6];
		for (int i = 5; i >= 0; i--) {
		    array[i] = temp_distance % 10 + '0';
		    temp_distance /= 10;
		}

		//SendText(array);

		for(i = 0; i < 100000; i++);
	}
}

/**************************************************************************************/
