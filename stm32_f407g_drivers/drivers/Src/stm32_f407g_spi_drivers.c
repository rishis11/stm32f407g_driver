/*
 * stm32_f407g_spi_drivers.c
 *
 *  Created on: Mar 6, 2025
 *      Author: Rishi
 */
#include "stm32f407g.h"


//helper func prototypes
static void spi_txe_interrupt_handle(SPI_Handle_t *pHandle);
static void spi_rxe_interrupt_handle(SPI_Handle_t *pHandle);
static void spi_ovr_error_interrupt_handle(SPI_Handle_t *pHandle);


//PERIPHERAL CLK SETUP
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
		if(EnorDi == ENABLE){
			if(pSPIx == SPI1)
				SPI1_PCLK_EN();
			else if(pSPIx == SPI2)
				SPI2_PCLK_EN();
			else if(pSPIx == SPI3)
				SPI3_PCLK_EN();
			else if(pSPIx == SPI4)
				SPI4_PCLK_EN();

		}
		else{
			if(pSPIx == SPI1)
				SPI1_PCLK_DI();
			else if(pSPIx == SPI2)
				SPI2_PCLK_DI();
			else if(pSPIx == SPI3)
				SPI3_PCLK_DI();
			else if(pSPIx == SPI4)
				SPI4_PCLK_DI();
		}
}

void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |=  (1 << SPI_CR1_SPE);
	}else
	{
		pSPIx->CR1 &=  ~(1 << SPI_CR1_SPE);
	}


}

//CHECK STATUS FLAG

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if( pSPIx->SR & FlagName )
		return FLAG_SET;
	else
		return FLAG_RESET;

}
//INIT & DE INIT
void SPI_Init(SPI_Handle_t *pSPIHandle)
{

	uint32_t tempreg = 0;

	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//Clear BIDI
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
	}

	else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// SET BIDI
		tempreg |= 1 << SPI_CR1_BIDIMODE;
	}

	else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
		{
			// Clear BIDI
			// SET RXONLY
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		tempreg |= 1 << SPI_CR1_RXONLY;

		}
	//SCLK SPEED
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;
	//DFF
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;
	//CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;
	//CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;
	//SSM
	tempreg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;
}


void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
		SPI1_REG_RESET();
	else if (pSPIx == SPI2)
		SPI2_REG_RESET();
	else if (pSPIx == SPI3)
		SPI3_REG_RESET();
	else if (pSPIx == SPI4)
		SPI4_REG_RESET();
}

//DATA SEND/RECIEVE
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while(Len != 0)
	{
		if(SPI_GetFlagStatus(pSPIx,SPI_TXE_FLAG) == FLAG_SET)
		{
			if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
			{
				//16-BIT DFF
				pSPIx->DR = *(uint16_t*)pTxBuffer;
				Len-=2;
				(uint16_t*)pTxBuffer++; //16-bit data format naturally increments by 2
			}
			else
			{
				//8-BIT DFF
				pSPIx->DR = *pTxBuffer;
				Len--;
				pTxBuffer++; //8-bit data format increments only by 1
			}
		}

	}
}



void SPI_ReceiveData(SPI_RegDef_t *pSPIx,uint8_t *pRxBuffer, uint32_t Len)
{
	while(Len != 0)
	{
		if(SPI_GetFlagStatus(pSPIx,SPI_RXNE_FLAG) == FLAG_SET)
		{
			if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
			{
				//16-BIT DFF
				*(uint16_t*)pRxBuffer = pSPIx->DR;
				Len-=2;
				(uint16_t*)pRxBuffer++; //16-bit data format naturally increments by 2
			}
			else
			{
				//8-BIT DFF
				*pRxBuffer = pSPIx->DR;
				Len--;
				pRxBuffer++; //8-bit data format increments only by 1
			}
		}

	}
}


void  SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |=  (1 << SPI_CR1_SSI);
	}else
	{
		pSPIx->CR1 &=  ~(1 << SPI_CR1_SSI);
	}


}

void  SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR2 |=  (1 << SPI_CR2_SSOE);
	}else
	{
		pSPIx->CR2 &=  ~(1 << SPI_CR2_SSOE);
	}


}



//DATA SEND/RECIEVE NON-BLOCKING API (INTERRUPT)
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		//SAVE TXBUFFER ADDR AND LEN
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;
		//MARK SPI STATE AS BUSY
		pSPIHandle->TxState = SPI_BUSY_IN_TX;
		//ENABLE TXIE INTERRUPT BIT BY CHECKING TXE IN CR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXNEIE);
		//TRANSACTION
	}

	return state;
}


uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle,uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
		//SAVE TXBUFFER ADDR AND LEN
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;
		//MARK SPI STATE AS BUSY
		pSPIHandle->RxState = SPI_BUSY_IN_RX;
		//ENABLE TXIE INTERRUPT BIT BY CHECKING TXE IN CR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
		//TRANSACTION
	}

	return state;
}



//IRG CONFIG FOR SPI
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{//TO GET THE ADDRESS OF NVIC REGISTERS & IT'S MODES YOU HAVE REFER TO ARM CORTEX-M4 MANUAL INSTEAD OF VENDOR BASED MANUAL

	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//program ISER0 register
			*NVIC_ISER0 |= ( 1 << IRQNumber );

		}else if(IRQNumber > 31 && IRQNumber < 64 ) //32 to 63
		{
			//program ISER1 register
			*NVIC_ISER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber >= 64 && IRQNumber < 96 )
		{
			//program ISER2 register //64 to 95
			*NVIC_ISER2 |= ( 1 << (IRQNumber % 64) );
		}
	}else
	{
		if(IRQNumber <= 31)
		{
			//program ICER0 register
			*NVIC_ICER0 |= ( 1 << IRQNumber );
		}else if(IRQNumber > 31 && IRQNumber < 64 )
		{
			//program ICER1 register
			*NVIC_ICER1 |= ( 1 << (IRQNumber % 32) );
		}
		else if(IRQNumber >= 64 && IRQNumber < 96 )
		{
			//program ICER2 register
			*NVIC_ICER2 |= ( 1 << (IRQNumber % 64) );
		}
	}

}


void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. first lets find out the ipr register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section  = IRQNumber %4 ;

	uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED) ;

	*(  NVIC_PR_BASE_ADDR + iprx ) |=  ( IRQPriority << shift_amount );
}


void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
	uint8_t temp1, temp2;
	//CHECKS TX INTERRUPT
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_TXNEIE);

	if(temp1 && temp2)
	{
		//TX HANDLE
		spi_txe_interrupt_handle(pHandle);
	}

	//CHECKS RX INTERRUPT
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2)
	{
		//RX HANDLE
		spi_rxe_interrupt_handle(pHandle);
	}

	//CHECKS OVR INTERRUPT
	temp1 = pHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2)
	{
		//OVR HANDLE
		spi_ovr_error_interrupt_handle(pHandle);
	}


}


static void spi_txe_interrupt_handle(SPI_Handle_t *pHandle)
{
	if(pHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	 {
					//16-BIT DFF
		pHandle->pSPIx->DR = *(uint16_t*)pHandle->pTxBuffer;
		pHandle->TxLen-=2;
		(uint16_t*)pHandle->pTxBuffer++; //16-bit data format naturally increments by 2
	 }
	else
	 {
		//8-BIT DFF
		pHandle->pSPIx->DR = *pHandle->pTxBuffer;
		pHandle->TxLen--;
		pHandle->pTxBuffer++; //8-bit data format increments only by 1
	 }

	if(! pHandle->TxLen)
	{
		//RESET TXEIE INTERRUPT BIT AND RESET THE TX RELATED VARIABLES IN HANDLE STRUCT
		SPI_CloseTransmission(pHandle);
		SPI_ApplicationEventCallBack(pHandle,SPI_EVENT_TX_CMPLT);
	}
}
static void spi_rxe_interrupt_handle(SPI_Handle_t *pHandle)
{
	if(pHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		//16-BIT DFF
		*(uint16_t*)pHandle->pRxBuffer = pHandle->pSPIx->DR;
		pHandle->RxLen-=2;
		(uint16_t*)pHandle->pRxBuffer++; //16-bit data format naturally increments by 2
	}
	else
	{
		//8-BIT DFF
		*pHandle->pRxBuffer = pHandle->pSPIx->DR;
		pHandle->RxLen--;
		pHandle->pRxBuffer++; //8-bit data format increments only by 1
	}
	if(! pHandle->RxLen)
		{
			//RESET rXEIE INTERRUPT BIT AND RESET THE rX RELATED VARIABLES IN HANDLE STRUCT
		    SPI_CloseReception(pHandle);
			SPI_ApplicationEventCallBack(pHandle,SPI_EVENT_RX_CMPLT);
		}
}

static void spi_ovr_error_interrupt_handle(SPI_Handle_t *pHandle)
{
	uint8_t temp;
	if(pHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pHandle->pSPIx->DR;
		temp = pHandle->pSPIx->SR;
	}
	(void)temp;
	SPI_ApplicationEventCallBack(pHandle,SPI_EVENT_OVR_ERR);

}

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXNEIE);
	pSPIHandle->TxLen = 0;
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxState = SPI_READY;
}
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->RxLen = 0;
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxState = SPI_READY;
}

__attribute__((weak)) void SPI_ApplicationEventCallBack(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
{
//weak implementation
}
