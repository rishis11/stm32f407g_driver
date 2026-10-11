/*
 * stm32_f407g_rcc_drivers.h
 *
 *  Created on: Oct 11, 2026
 *      Author: X30W-J
 */

#ifndef INC_STM32_F407G_RCC_DRIVERS_H_
#define INC_STM32_F407G_RCC_DRIVERS_H_




#include "stm32f407g.h"

//This returns the APB1 clock value
uint32_t RCC_GetPCLK1Value(void);

//This returns the APB2 clock value
uint32_t RCC_GetPCLK2Value(void);


uint32_t  RCC_GetPLLOutputClock(void);
#endif /* INC_STM32_F407G_RCC_DRIVERS_H_ */
