/*
 * USART.c
 *
 *  Created on: 4 paü 2016
 *      Author: Luke
 */

#include "USART.h"
#include "defs.h"
#include "stm32f30x_usart.h"

void USART_init(){
	isDataReading = 0;
	isDataSending = 0;
	currentIndex = 0;

	RCC_Configuration();
	GPIO_Configuration();
	NVIC_Configuration();
	USART2_Configuration();
}

void SendText(char text[]) {
	if(!isDataSending){
		isDataSending = 1;
		currentIndex = 0;
		int i;
		for(i = 0; i < MAX_BUFFER_SIZE && text[i] != '\0'; i++){
			buffer[i] = text[i];
		}
		buffer[i] = '\r';
		i++;
		buffer[i] = '\n';
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}
}

void clearBuffer() {
	currentIndex = 0;
	for(int i = 0; i < MAX_BUFFER_SIZE; i++){
		buffer[i] = '\0';
	}
}

/**************************************************************************************/

void RCC_Configuration(void) {
	/* --------------------------- System Clocks Configuration -----------------*/
	/* USART2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* GPIOA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
}

/**************************************************************************************/

void GPIO_Configuration(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/*-------------------------- GPIO Configuration ----------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
}

/**************************************************************************************/

void USART2_Configuration(void) {
	USART_InitTypeDef USART_InitStructure;

	/* USARTx configuration ------------------------------------------------------*/
	/* USARTx configured as follow:
	 - BaudRate = 115200 baud
	 - Word Length = 8 Bits
	 - One Stop Bit
	 - No parity
	 - Hardware flow control disabled (RTS and CTS signals)
	 - Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;

	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);

	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

/**************************************************************************************/

void NVIC_Configuration(void) {
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************************/

void USART2_IRQHandler(void) {
	/*  UART RX  */
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		if (!isDataReading) {
			clearBuffer();
		} else {
			isDataReading = 1;
		}

		if (currentIndex < MAX_BUFFER_SIZE) {
			buffer[currentIndex] = USART_ReceiveData(USART2);
			if (USART_GetITStatus(USART2, USART_IT_RXNE) == RESET) {
				//displayText();
				//dodac isDataReading (wyzerowac w tym miejscu)
			}
		}
	}

	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
		USART_SendData(USART2, buffer[currentIndex]);
		currentIndex++;

		if (currentIndex >= MAX_BUFFER_SIZE
				|| buffer[currentIndex + 1] == '\0') {
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			isDataSending = 0;
		}
	}
}
