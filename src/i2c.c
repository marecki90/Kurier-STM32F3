/*
 * i2c.c
 *
 *  Created on: 4 paü 2016
 *      Author: Luke
 */


#include "i2c.h"
#include "defs.h"
#include "stm32f30x_i2c.h"

void I2C_init(){
	//inicjalizowanie zmiennych


	//ustawienie i2c
	I2C_RCC_Configuration();
	I2C_GPIO_Configuration();
	//I2C_NVIC_Configuration();
	I2C_Configuration();
}

void I2C_RCC_Configuration(void){
	/* RCC Configuration */
	/*I2C Peripheral clock enable */
	RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE);

	/*SDA GPIO clock enable */
	RCC_AHBPeriphClockCmd(I2Cx_SDA_GPIO_CLK, ENABLE);

	/*SCL GPIO clock enable */
	RCC_AHBPeriphClockCmd(I2Cx_SCL_GPIO_CLK, ENABLE);

	/* Reset I2Cx IP */
	RCC_APB1PeriphResetCmd(I2Cx_CLK, ENABLE);

	/* Release reset signal of I2Cx IP */
	RCC_APB1PeriphResetCmd(I2Cx_CLK, DISABLE);
}

void I2C_GPIO_Configuration(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* GPIO Configuration */
	/*Configure I2C SCL pin */
	GPIO_InitStructure.GPIO_Pin = I2Cx_SCL_PIN | I2Cx_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);

	/*Configure I2C SDA pin */
//	GPIO_InitStructure.GPIO_Pin = I2Cx_SDA_PIN;
//	GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);

	/* Connect PXx to I2C_SCL */
	GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_SOURCE, I2Cx_SCL_AF);

	/* Connect PXx to I2C_SDA */
	GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_SOURCE, I2Cx_SDA_AF);
}

void I2C_Configuration(void){
	I2C_InitTypeDef  I2C_InitStructure;

	/* I2C Struct Initialize */
	I2C_DeInit(I2Cx);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Disable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = 0xA0;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_Timing = 0x10806291;


	/* I2C Enable and Init */
	I2C_Cmd(I2Cx, ENABLE);
	I2C_Init(I2Cx, &I2C_InitStructure);


	/* WLACZENIE PRZERWANIA??? */
	//I2C_ITConfig(I2Cx, ENABLE); //Part of the STM32 I2C driver
}

void I2C_NVIC_Configuration(void){
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the I2C event priority */
	NVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void SONAR_I2C_StartMeasurement(uint8_t slaveAddr){
	I2C_TransferHandling(I2Cx, slaveAddr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == RESET);
	I2C_SendData(I2Cx, 0x10);

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TCR) == RESET);

	I2C_TransferHandling(I2Cx, slaveAddr, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == RESET);

	I2C_SendData(I2Cx, 0x02);

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == RESET);
	I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);
}
