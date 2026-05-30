/*
 * stm32f407g.h
 *
 *  Created on: Feb 26, 2025
 *      Author: Rishi
 */
//This header file contains the details and addresses of various registers

/*
 * ------NOTE-----
 * THE PHERIPHERALS THAT ARE USED IN THIS PROJECT ARE **ONLY** INCLUDED.
 * e.g. THE UART ADDRESSES OF UART4,5 ARE ONLY USED ONLY THEY ARE INCLUDED. UART6,7 ARE NOT USED HENCE NOT DEFINED
 */
#include<stddef.h>
#include<stdint.h>

#ifndef INC_STM32F407G_H_
#define INC_STM32F407G_H_


#define __vo 				volatile

/**********************************START:Processor Specific Details **********************************/
/*
 * ARM Cortex Mx Processor NVIC ISERx register Addresses
 */

#define NVIC_ISER0          ( (__vo uint32_t*)0xE000E100 )
#define NVIC_ISER1          ( (__vo uint32_t*)0xE000E104 )
#define NVIC_ISER2          ( (__vo uint32_t*)0xE000E108 )
#define NVIC_ISER3          ( (__vo uint32_t*)0xE000E10c )


/*
 * ARM Cortex Mx Processor NVIC ICERx register Addresses
 */
#define NVIC_ICER0 			((__vo uint32_t*)0XE000E180)
#define NVIC_ICER1			((__vo uint32_t*)0XE000E184)
#define NVIC_ICER2  		((__vo uint32_t*)0XE000E188)
#define NVIC_ICER3			((__vo uint32_t*)0XE000E18C)


/*
 * ARM Cortex Mx Processor Priority Register Address Calculation
 */
#define NVIC_PR_BASE_ADDR 	((__vo uint32_t*)0xE000E400)

/*
 * ARM Cortex Mx Processor number of priority bits implemented in Priority Register
 */
#define NO_PR_BITS_IMPLEMENTED  4

//base addresses of flash and SRAM
#define FLASH_BADDR 		0x08000000U  //U defines that the number is unsigned-integer.
#define SRAM1_BADDR			0x20000000U
#define SRAM2_BADDR			0x2001C000U
#define ROM					0x1FFF0000U  //THIS IS THE SYSTEM MEMORY BASE ADDRESS
#define SRAM 				SRAM1_BADDR


// Base addresses Of Peripherals bus addresses such as AHBx and APBx
#define PERIPHERAL_BADDR		(__vo uint32_t*)0x40000000U
#define APB1_BADDR				(__vo uint32_t*)PERIPHERAL_BADDR
#define APB2_BADDR				(__vo uint32_t*)0x40010000U
#define AHB1_BADDR				(__vo uint32_t*)0x40020000U
#define AHB2_BADDR				(__vo uint32_t*)0x50000000U
//#define AHB3_BADDR				0xA0000000U

//BASE ADDRESSES OF GPIO PERIPHERALS - ALL LIE IN AHB1 BUS
#define GPIOA_BADDR				(AHB1_BADDR + 0x0000)
#define GPIOB_BADDR				(AHB1_BADDR + 0x0400)
#define GPIOC_BADDR				(AHB1_BADDR + 0x0800)
#define GPIOD_BADDR				(AHB1_BADDR + 0x0C00)
#define GPIOE_BADDR				(AHB1_BADDR + 0x1000)
#define GPIOF_BADDR				(AHB1_BADDR + 0x1400)
#define GPIOG_BADDR				(AHB1_BADDR + 0x1800)
#define GPIOH_BADDR				(AHB1_BADDR + 0x1C00)
#define GPIOI_BADDR				(AHB1_BADDR + 0x2000)
#define GPIOJ_BADDR				(AHB1_BADDR + 0x2400)
#define GPIOK_BADDR				(AHB1_BADDR + 0x2800)

#define RCC_BADDR				(AHB1_BADDR + 0x3800)
//BASE ADDRESSES OF COMMUNICATION PERIPHERALS
//PERIPHERALS THAT LIE IN APB1 BUS
#define I2C1_BADDR				(APB1_BADDR + 0x5400)
#define I2C2_BADDR				(APB1_BADDR + 0x5800)
#define I2C3_BADDR				(APB1_BADDR + 0x5C00)

#define CAN1_BADDR				(APB1_BADDR + 0x6400)
#define CAN2_BADDR				(APB1_BADDR + 0x6800)

#define SPI1_BADDR				(APB2_BADDR + 0x3000)
#define SPI2_BADDR				(APB1_BADDR + 0x3800)
#define SPI3_BADDR				(APB1_BADDR + 0x3C00)
#define SPI4_BADDR				(APB2_BADDR + 0x3400)

#define UART4_BADDR				(APB1_BADDR + 0x4C00)
#define UART5_BADDR				(APB1_BADDR + 0x5000)

#define USART2_BADDR			(APB1_BADDR + 0x4400)
#define USART3_BADDR			(APB1_BADDR + 0x4800)


//PERIPHERALS THAT LIE IN APB2 BUS
#define SPI1_BADDR				(APB2_BADDR + 0x3000)

#define USART1_BADDR			(APB2_BADDR + 0x1000)
#define USART6_BADDR			(APB2_BADDR + 0x1400)

#define EXTI_BADDR				(APB2_BADDR + 0x3C00)
#define SYSCFG_BADDR			(APB2_BADDR + 0x3800)


//PERIPHERALS REGISTER STRUCTER DEFINATIONS:

//STRUCTER FOR GPIOs
typedef struct
{
	__vo uint32_t MODER;                        //MODE REGISTER
	__vo uint32_t OTYPER;
	__vo uint32_t OSPEEDR;
	__vo uint32_t PUPDR;
	__vo uint32_t IDR;
	__vo uint32_t ODR;
	__vo uint32_t BSRR;
	__vo uint32_t LCKR;
	__vo uint32_t AFR[2];					 /*!< AFR[0] : GPIO alternate function low register, AF[1] : GPIO alternate function high register    		Address offset: 0x20-0x24 */
}GPIO_RegDef_t;


//STRUCTER FOR SPIx
typedef struct
{
	__vo uint32_t CR1;        /*!< TODO,     										Address offset: 0x00 */
	__vo uint32_t CR2;        /*!< TODO,     										Address offset: 0x04 */
	__vo uint32_t SR;         /*!< TODO,     										Address offset: 0x08 */
	__vo uint32_t DR;         /*!< TODO,     										Address offset: 0x0C */
	__vo uint32_t CRCPR;      /*!< TODO,     										Address offset: 0x10 */
	__vo uint32_t RXCRCR;     /*!< TODO,     										Address offset: 0x14 */
	__vo uint32_t TXCRCR;     /*!< TODO,     										Address offset: 0x18 */
	__vo uint32_t I2SCFGR;    /*!< TODO,     										Address offset: 0x1C */
	__vo uint32_t I2SPR;      /*!< TODO,     										Address offset: 0x20 */
} SPI_RegDef_t;


//StRUCTER FOR RCC REGISTERS
typedef struct
{
  __vo uint32_t CR;            /*!< TODO,     										Address offset: 0x00 */
  __vo uint32_t PLLCFGR;       /*!< TODO,     										Address offset: 0x04 */
  __vo uint32_t CFGR;          /*!< TODO,     										Address offset: 0x08 */
  __vo uint32_t CIR;           /*!< TODO,     										Address offset: 0x0C */
  __vo uint32_t AHB1RSTR;      /*!< TODO,     										Address offset: 0x10 */
  __vo uint32_t AHB2RSTR;      /*!< TODO,     										Address offset: 0x14 */
  __vo uint32_t AHB3RSTR;      /*!< TODO,     										Address offset: 0x18 */
  uint32_t      RESERVED0;     /*!< Reserved, 0x1C                                                       */
  __vo uint32_t APB1RSTR;      /*!< TODO,     										Address offset: 0x20 */
  __vo uint32_t APB2RSTR;      /*!< TODO,     										Address offset: 0x24 */
  uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                  */
  __vo uint32_t AHB1ENR;       /*!< TODO,     										Address offset: 0x30 */
  __vo uint32_t AHB2ENR;       /*!< TODO,     										Address offset: 0x34 */
  __vo uint32_t AHB3ENR;       /*!< TODO,     										Address offset: 0x38 */
  uint32_t      RESERVED2;     /*!< Reserved, 0x3C                                                       */
  __vo uint32_t APB1ENR;       /*!< TODO,     										Address offset: 0x40 */
  __vo uint32_t APB2ENR;       /*!< TODO,     										Address offset: 0x44 */
  uint32_t      RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                  */
  __vo uint32_t AHB1LPENR;     /*!< TODO,     										Address offset: 0x50 */
  __vo uint32_t AHB2LPENR;     /*!< TODO,     										Address offset: 0x54 */
  __vo uint32_t AHB3LPENR;     /*!< TODO,     										Address offset: 0x58 */
  uint32_t      RESERVED4;     /*!< Reserved, 0x5C                                                       */
  __vo uint32_t APB1LPENR;     /*!< TODO,     										Address offset: 0x60 */
  __vo uint32_t APB2LPENR;     /*!< RTODO,     										Address offset: 0x64 */
  uint32_t      RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                  */
  __vo uint32_t BDCR;          /*!< TODO,     										Address offset: 0x70 */
  __vo uint32_t CSR;           /*!< TODO,     										Address offset: 0x74 */
  uint32_t      RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                  */
  __vo uint32_t SSCGR;         /*!< TODO,     										Address offset: 0x80 */
  __vo uint32_t PLLI2SCFGR;    /*!< TODO,     										Address offset: 0x84 */
  __vo uint32_t PLLSAICFGR;    /*!< TODO,     										Address offset: 0x88 */
  __vo uint32_t DCKCFGR;       /*!< TODO,     										Address offset: 0x8C */
  __vo uint32_t CKGATENR;      /*!< TODO,     										Address offset: 0x90 */
  __vo uint32_t DCKCFGR2;      /*!< TODO,     										Address offset: 0x94 */

} RCC_RegDef_t;

//STRUCTER FOR EXTI
typedef struct
{
	__vo uint32_t IMR;                        //MODE REGISTER
	__vo uint32_t EMR;
	__vo uint32_t RTSR;
	__vo uint32_t FTSR;
	__vo uint32_t SWIER;
	__vo uint32_t PR;
}EXTI_RegDef_t;

//SYSCFG STRUCTER
typedef struct
{
	__vo uint32_t MEMRMP;       /*!< Give a short description,                    Address offset: 0x00      */
	__vo uint32_t PMC;          /*!< TODO,     									  Address offset: 0x04      */
	__vo uint32_t EXTICR[4];    /*!< TODO , 									  Address offset: 0x08-0x14 */
	uint32_t      RESERVED1[2];  /*!< TODO          							  Reserved, 0x18-0x1C    	*/
	__vo uint32_t CMPCR;        /*!< TODO         								  Address offset: 0x20      */
	uint32_t      RESERVED2[2];  /*!<                                             Reserved, 0x24-0x28 	    */
	__vo uint32_t CFGR;         /*!< TODO                                         Address offset: 0x2C   	*/
} SYSCFG_RegDef_t;


//StRUCTER FOR RCC REGISTERS

//PERIPHERALS DEFINATIONS BY TYPECASTING THE ADDRESS OF PERIPHERAL TO THE REGISTER'S STRUCTURE
#define GPIOA		(GPIO_RegDef_t*)GPIOA_BADDR
#define GPIOB		(GPIO_RegDef_t*)GPIOB_BADDR
#define GPIOC		(GPIO_RegDef_t*)GPIOC_BADDR
#define GPIOD		(GPIO_RegDef_t*)GPIOD_BADDR
#define GPIOE		(GPIO_RegDef_t*)GPIOE_BADDR
#define GPIOF		(GPIO_RegDef_t*)GPIOF_BADDR
#define GPIOG		(GPIO_RegDef_t*)GPIOG_BADDR
#define GPIOH		(GPIO_RegDef_t*)GPIOH_BADDR
#define GPIOI		(GPIO_RegDef_t*)GPIOI_BADDR
#define GPIOJ		(GPIO_RegDef_t*)GPIOJ_BADDR
#define GPIOK		(GPIO_RegDef_t*)GPIOK_BADDR

#define RCC			((RCC_RegDef_t*)RCC_BADDR)
#define EXTI		((EXTI_RegDef_t*)EXTI_BADDR)
#define SYSCFG		((SYSCFG_RegDef_t*)SYSCFG_BADDR)

#define SPI1  				((SPI_RegDef_t*)SPI1_BADDR)
#define SPI2  				((SPI_RegDef_t*)SPI2_BADDR)
#define SPI3  				((SPI_RegDef_t*)SPI3_BADDR)
#define SPI4  				((SPI_RegDef_t*)SPI4_BADDR)


//MACROS FOR ENABLING CLOCK FOR GPIOs
#define GPIOA_PCLK_EN()			(RCC->AHB1ENR |=(1<<0))
#define GPIOB_PCLK_EN()			(RCC->AHB1ENR |=(1<<1))
#define GPIOC_PCLK_EN()			(RCC->AHB1ENR |=(1<<2))
#define GPIOD_PCLK_EN()			(RCC->AHB1ENR |=(1<<3))
#define GPIOE_PCLK_EN()			(RCC->AHB1ENR |=(1<<4))
#define GPIOF_PCLK_EN()			(RCC->AHB1ENR |=(1<<5))
#define GPIOG_PCLK_EN()			(RCC->AHB1ENR |=(1<<6))
#define GPIOH_PCLK_EN()			(RCC->AHB1ENR |=(1<<7))
#define GPIOI_PCLK_EN()			(RCC->AHB1ENR |=(1<<8))


//MACROS FOR ENABLING CLOCK FOR I2C

#define I2C1_PCLK_EN() (RCC->APB1ENR |= (1 << 21))
#define I2C2_PCLK_EN() (RCC->APB1ENR |= (1 << 22))
#define I2C3_PCLK_EN() (RCC->APB1ENR |= (1 << 23))

//MACROS FOR ENABLING CLOCK FOR SPI
#define SPI1_PCLK_EN() (RCC->APB2ENR |= (1 << 12))
#define SPI2_PCLK_EN() (RCC->APB1ENR |= (1 << 14))
#define SPI3_PCLK_EN() (RCC->APB1ENR |= (1 << 15))
#define SPI4_PCLK_EN() (RCC->APB2ENR |= (1 << 13))

//MACROS FOR ENABLING CLOCK FOR USART
#define USART1_PCCK_EN() (RCC->APB2ENR |= (1 << 4))
#define USART2_PCCK_EN() (RCC->APB1ENR |= (1 << 17))
#define USART3_PCCK_EN() (RCC->APB1ENR |= (1 << 18))

//MACROS FOR ENABLING CLOCK FOR UART
#define UART4_PCCK_EN()  (RCC->APB1ENR |= (1 << 19))
#define UART5_PCCK_EN()  (RCC->APB1ENR |= (1 << 20))
#define USART6_PCCK_EN() (RCC->APB1ENR |= (1 << 5))

//MACROS FOR ENABLING CLOCK FOR SYSCFG
#define SYSCFG_PCLK_EN() (RCC->APB2ENR |= (1 << 14))



//MACROS FOR DISABLING CLOCK FOR GPIOs
#define GPIOA_PCLK_DI()			(RCC->AHB1ENR &=~(1<<0))
#define GPIOB_PCLK_DI()			(RCC->AHB1ENR &=~(1<<1))
#define GPIOC_PCLK_DI()			(RCC->AHB1ENR &=~(1<<2))
#define GPIOD_PCLK_DI()			(RCC->AHB1ENR &=~(1<<3))
#define GPIOE_PCLK_DI()			(RCC->AHB1ENR &=~(1<<4))
#define GPIOF_PCLK_DI()			(RCC->AHB1ENR &=~(1<<5))
#define GPIOG_PCLK_DI()			(RCC->AHB1ENR &=~(1<<6))
#define GPIOH_PCLK_DI()			(RCC->AHB1ENR &=~(1<<7))
#define GPIOI_PCLK_DI()			(RCC->AHB1ENR &=~(1<<8))


//MACROS FOR DISABLING CLOCK FOR I2C

#define I2C1_PCLK_DI() (RCC->APB1ENR &=~ (1 << 21))
#define I2C2_PCLK_DI() (RCC->APB1ENR &=~ (1 << 22))
#define I2C3_PCLK_DI() (RCC->APB1ENR &=~ (1 << 23))

//MACROS FOR DISABLING CLOCK FOR SPI
#define SPI1_PCLK_DI() (RCC->APB2ENR &=~ (1 << 12))
#define SPI2_PCLK_DI() (RCC->APB1ENR &=~ (1 << 14))
#define SPI3_PCLK_DI() (RCC->APB1ENR &=~ (1 << 15))
#define SPI4_PCLK_DI() (RCC->APB2ENR &=~ (1 << 13))

//MACROS FOR DISABLING CLOCK FOR USART
#define USART1_PCCK_DI() (RCC->APB2ENR &=~ (1 << 4))
#define USART2_PCCK_DI() (RCC->APB1ENR &=~ (1 << 17))
#define USART3_PCCK_DI() (RCC->APB1ENR &=~ (1 << 18))

//MACROS FOR DISABLING CLOCK FOR UART
#define UART4_PCLK_DI()  (RCC->APB1ENR &=~ (1 << 19))
#define UART5_PCLK_DI()  (RCC->APB1ENR &=~ (1 << 20))
#define USART6_PCLK_DI() (RCC->APB1ENR &=~ (1 << 5))

//MACROS FOR DISABLING CLOCK FOR SYSCFG
#define SYSCFG_PCLK_DI() (RCC->APB2ENR &=~ (1 << 14))

//MACROS FOR RESETTING GPIOs
#define GPIOA_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOB_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); }while(0)
#define GPIOC_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); }while(0)
#define GPIOD_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); }while(0)
#define GPIOE_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); }while(0)
#define GPIOF_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 5)); (RCC->AHB1RSTR &= ~(1 << 5)); }while(0)
#define GPIOG_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 6)); (RCC->AHB1RSTR &= ~(1 << 6)); }while(0)
#define GPIOH_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 7)); (RCC->AHB1RSTR &= ~(1 << 7)); }while(0)
#define GPIOI_REG_RESET()               do{ (RCC->AHB1RSTR |= (1 << 8)); (RCC->AHB1RSTR &= ~(1 << 8)); }while(0)

#define GPIO_BASEADDR_TO_CODE(x)      ( (x == GPIOA)?0:\
										(x == GPIOB)?1:\
										(x == GPIOC)?2:\
										(x == GPIOD)?3:\
								        (x == GPIOE)?4:\
								        (x == GPIOF)?5:\
								        (x == GPIOG)?6:\
								        (x == GPIOH)?7: \
								        (x == GPIOI)?8:0)

//@EXTI_NO
#define IRQ_NO_EXTI0 		6
#define IRQ_NO_EXTI1 		7
#define IRQ_NO_EXTI2 		8
#define IRQ_NO_EXTI3 		9
#define IRQ_NO_EXTI4 		10
#define IRQ_NO_EXTI9_5 		23
#define IRQ_NO_EXTI15_10 	40

//GENRIC MACROS DEFINITION

#define ENABLE 				1
#define DISABLE 			0
#define SET 				ENABLE
#define RESET 				DISABLE
#define GPIO_PIN_SET        SET
#define GPIO_PIN_RESET      RESET

#include "stm32_f407g_gpio_driver.h"
#include "stm32_f407g_spi_drivers.h"
#endif /* INC_STM32F407G_H_ */

