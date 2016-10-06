/*
 * USART.h
 *
 *  Created on: 4 paü 2016
 *      Author: Luke
 */

#ifndef USART_H_
#define USART_H_

#include "stm32f30x.h"

#define MAX_BUFFER_SIZE 50

volatile char buffer[MAX_BUFFER_SIZE];
uint8_t isDataReading;
uint8_t isDataSending;
volatile uint8_t currentIndex;


void USART_init();

void SendText(char text[]);
void clearBuffer();

/**************************************************************************************/

void RCC_Configuration(void);

/**************************************************************************************/

void GPIO_Configuration(void);

/**************************************************************************************/

void USART2_Configuration(void);

/**************************************************************************************/

void NVIC_Configuration(void);

/**************************************************************************************/

void USART2_IRQHandler(void);


#endif /* USART_H_ */
