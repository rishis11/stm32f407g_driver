/*
 * 001led_toggle.c
 *
 *  Created on: Mar 7, 2025
 *      Author: Rishi
 */
#include "stm32f407g.h"



int main(void)
{

	GPIO_Handle_t GpioLed;
	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	int y=0;
		GPIO_PeriClockControl(GPIOD,ENABLE);

		GPIO_Init(&GpioLed);
		//uint32_t *pgpioportdoutput = (uint32_t*)0x40020C14;
		//*pgpioportdoutput = *pgpioportdoutput|0x1000;
		GPIO_ToggleOutputPin(GPIOD,GPIO_PIN_NO_12);
		for(int i=0;i<5;i++){
			for( volatile uint32_t x = 0 ; x < 5 ; x ++)
			{
				y+=x;
				//*pgpioportdoutput &= ~0x1000;
			}
			if(y%20==0){
				GPIO_ToggleOutputPin(GPIOD,GPIO_PIN_NO_12);

							//*pgpioportdoutput &= ~0x1000;
						}
		}



	return 0;
}

