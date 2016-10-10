/*
 * i2c.h
 *
 *  Created on: 4 paü 2016
 *      Author: Luke
 */

#ifndef I2C_H_
#define I2C_H_

#include "stm32f30x.h"

//#define I2Cx                          I2C1
//#define I2Cx_CLK                      RCC_APB1Periph_I2C1
//#define I2Cx_SDA_GPIO_CLK             RCC_AHBPeriph_GPIOB
//#define I2Cx_SDA_PIN                  GPIO_Pin_9
//#define I2Cx_SDA_GPIO_PORT            GPIOB
//#define I2Cx_SDA_SOURCE               GPIO_PinSource9
//#define I2Cx_SDA_AF                   GPIO_AF_I2C1
//
//#define I2Cx_SCL_GPIO_CLK             RCC_AHBPeriph_GPIOB
//#define I2Cx_SCL_PIN                  GPIO_Pin_6
//#define I2Cx_SCL_GPIO_PORT            GPIOB
//#define I2Cx_SCL_SOURCE               GPIO_PinSource6
//#define I2Cx_SCL_AF                   GPIO_AF_I2C1

//#define CPAL_I2C1_SCL_GPIO_PORT         GPIOB                  (X : Name of the GPIO PORT  (A,B,C,....))
//#define CPAL_I2C1_SCL_GPIO_CLK          RCC_APB2Periph_GPIOB   (X : Name of the GPIO PORT  (A,B,C,....))
//#define CPAL_I2C1_SCL_GPIO_PIN          GPIO_Pin_6             (X : Pin number (1,2,3,....))
//#define CPAL_I2C1_SCL_GPIO_PINSOURCE    GPIO_PinSource6        (X : Pin number (1,2,3,....))
//
//#define CPAL_I2C1_SDA_GPIO_PORT         GPIOB                  (X : Name of the GPIO PORT  (A,B,C,....))
//#define CPAL_I2C1_SDA_GPIO_CLK          RCC_APB2Periph_GPIOB   (X : Name of the GPIO PORT  (A,B,C,....))
//#define CPAL_I2C1_SDA_GPIO_PIN          GPIO_Pin_9             (X : Pin number (1,2,3,....))
//#define CPAL_I2C1_SDA_GPIO_PINSOURCE    GPIO_PinSource9        (X : Pin number (1,2,3,....))


/**************************************************************************************/


/**************************************************************************************/

void I2C_init();

/**************************************************************************************/

void I2C_RCC_Configuration(void);

/**************************************************************************************/

void I2C_GPIO_Configuration(void);

/**************************************************************************************/

void I2C_Configuration(void);

/**************************************************************************************/

void I2C_NVIC_Configuration(void);

/**************************************************************************************/


#endif /* I2C_H_ */
