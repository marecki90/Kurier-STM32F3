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
#include "stm32f30x_dma.h"

#include "defs.h"
#include "USART.h"
#include "i2c.h"

#define SONAR_ADDRESS 0x05

uint8_t I2C_Rx_Buffer[2];
uint16_t temp_distance;

void I2C_DMA_Read(uint8_t slaveAddr, uint8_t readAddr){
	/* Disable DMA channel*/
	DMA_Cmd(DMA1_Channel7, DISABLE);
	/* Set current data number again to 14 for MPu6050, only possible after disabling the DMA channel */
	DMA_SetCurrDataCounter(DMA1_Channel7, 2);

	/* While the bus is busy */
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

	I2C_TransferHandling(I2Cx, SONAR_ADDRESS, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == RESET);

	//Send the address of the register you wish to read
	I2C_SendData(I2Cx, 0x20);

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TC) == RESET);

	I2C_TransferHandling(I2Cx, SONAR_ADDRESS, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

	/* Test on EV6 and clear it */
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET);

	/* Start DMA to receive data from I2C */
	DMA_Cmd(DMA1_Channel7, ENABLE);
	I2C_DMACmd(I2Cx, I2C_DMAReq_Rx, ENABLE);
	// When the data transmission is complete, it will automatically jump to DMA interrupt routine to finish the rest.
	//now go back to the main routine
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0))
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_8);

		EXTI_ClearITPendingBit(EXTI_Line0);
		I2C_DMA_Read(SONAR_ADDRESS, 0x20);
	}
}

void DMA1_Channel7_IRQHandler(void) {
	if (DMA_GetFlagStatus(DMA1_FLAG_TC7)) {
		/* Clear transmission complete flag */
		DMA_ClearFlag(DMA1_FLAG_TC7);

		I2C_DMACmd(I2Cx, I2C_DMAReq_Rx,DISABLE);
		/* Send I2C1 STOP Condition */
		I2C_GenerateSTOP(I2Cx, ENABLE);
		/* Disable DMA channel*/
		DMA_Cmd(I2Cx, DISABLE);

		//read
		temp_distance = I2C_Rx_Buffer[0] + (I2C_Rx_Buffer[1] << 8);
	}
}

int main(void)
{
    USART_init();
    I2C_init();

    int i;

    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    DMA_DeInit(DMA1_Channel7); //reset DMA1 channe1 to default values;

    DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40005424; //=0x40005424 : address of data reading register of I2C1
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2C_Rx_Buffer; //variable to store data
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //channel will be used for peripheral to memory transfer
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	//setting normal mode (non circular)
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	//medium priority
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//Location assigned to peripheral register will be source
    DMA_InitStructure.DMA_BufferSize = 2;	//number of data to be transfered
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //automatic memory increment disable for peripheral
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//automatic memory increment enable for memory
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//source peripheral data size = 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//destination memory data size = 8bit
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn; //I2C1 connect to channel 7 of DMA1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //przerwanie na pin D0 od Sonara
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = 0;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, GPIO_PinSource0);

	EXTI_InitStructure.EXTI_Line = 0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//dioda PE8
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//Initialize pins
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	while(1){
		//SendText("Czesc");
		GPIO_ResetBits(GPIOE, GPIO_Pin_8);
		SONAR_I2C_StartMeasurement(SONAR_ADDRESS);

		for(i = 0; i < 100000; i++);

		char array[6];
		for (int i = 5; i >= 0; i--) {
		    array[i] = temp_distance % 10 + '0';
		    temp_distance /= 10;
		}

		SendText(array);

		for(i = 0; i < 100000; i++);
	}
}

/**************************************************************************************/
