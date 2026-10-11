/*
 *
 * stm32_f407g_i2c_drivers.c
 *
 *  Created on: Jul 12, 2026
 *      Author: X30W-J
 */
#include "stm32f407g.h"
#include "stm32_f407g_rcc_drivers.h"
uint16_t AHPB_PreScaler[4] = {2,4,8,16};
//uint16_t APB1_PreScaler[9] = {2,4,8,16,32,64,128,256,512};

//PRIVATE/HELPER FUNCTION PROTOTYPES
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
static void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);
//PRIVATE/HELPER FUNCTION DEFINITIONS

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == I2C_ACKENABLE)
	{
		//enable the ack
		pI2Cx->CR1 |= ( 1 << I2C_CR1_ACK);
	}else
	{
		//disable the ack
		pI2Cx->CR1 &= ~( 1 << I2C_CR1_ACK);
	}
}


static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= 1; //SlaveAddr is Slave address + r/nw bit=1
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= ~(1); //SlaveAddr is Slave address + r/nw bit=1
	pI2Cx->DR = SlaveAddr;
}




static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
	{
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1)
			{
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
				uint32_t dummyread = pI2CHandle->pI2Cx->SR1;
				dummyread = pI2CHandle->pI2Cx->SR2;
				(void)dummyread;
			}
			else
			{
				uint32_t dummyread = pI2CHandle->pI2Cx->SR1;
				dummyread = pI2CHandle->pI2Cx->SR2;
				(void)dummyread;
			}
		}
	}
	else
	{
		uint32_t dummyread = pI2CHandle->pI2Cx->SR1;
		dummyread = pI2CHandle->pI2Cx->SR2;
		(void)dummyread;

	}



}

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}


static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->RxSize == 1)
	{
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxSize > 1)
	{
		if(pI2CHandle->RxLen == 2)
		{
			I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
		}
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxLen == 0)
	{
		//CLOSE RECEPTION
		if(pI2CHandle->Sr == I2C_DISABLE_SR)
		{
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		I2C_CloseReceiveData(pI2CHandle);

		//NOTIFY APPLICATION ABOUT THE RECEPTION COMPLETION
		I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_RX_CMPLT);

	}
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->TxLen > 0)
	{
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);
		pI2CHandle->TxLen--;
		pI2CHandle->pTxBuffer++;
	}
}

static void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	// ALL INTRREUPT CONTROL BITS TO STOP GETIING AY INTERRUPTS ANYMORE
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVFEN);
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//RESET ALL THE HANDLE REG'S FIELDS
	pI2CHandle->RxLen=0;
	pI2CHandle->RxSize=0;
	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;

}


static void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	// ALL INTRREUPT CONTROL BITS TO STOP GETIING AY INTERRUPTS ANYMORE
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVFEN);
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//RESET ALL THE HANDLE REG'S FIELDS
	pI2CHandle->TxLen=0;
	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;

}


//uint32_t RCC_GetPLLOutputClock(void)
//{
//	//calculate PLL OUTPUT
//	return 0;
//}
//GET PLCK1 CLCK SPEED

//uint32_t RCC_GetPCLK1Value(void)
//{
//	uint32_t pclk1,SystemClk;
//	uint8_t clksrc,temp,ahbp,apb1;
//
//	clksrc = ((RCC->CFGR >> 2) & 0x3);
//	if(clksrc == 0)
//	{
//		SystemClk = 16000000;
//	}
//	else if(clksrc == 1)
//	{
//		SystemClk = 8000000;
//	}
//	else if(clksrc == 2)
//	{
//		SystemClk = RCC_GetPLLOutputClock();
//	}
//	temp = ((RCC->CFGR >> 4) & 0xF);
//	if(temp < 8)
//		ahbp = 1;
//	else
//		ahbp = AHPB_PreScaler[temp-8];
//
//	temp = ((RCC->CFGR >> 10) & 0x7);
//	if(temp < 8)
//		apb1 = 1;
//	else
//		apb1 = APB1_PreScaler[temp-4];
//
//	pclk1 = (SystemClk / ahbp) / apb1;
//
//	return pclk1;
//}

//CHECK STATUS REGISTER FUNCTION
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
		if( pI2Cx->SR1 & FlagName )
			return FLAG_SET;
		else
			return FLAG_RESET;

}

void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
		//pI2cBaseAddress->CR1 |= I2C_CR1_PE_Bit_Mask;
	}else
	{
		pI2Cx->CR1 &= ~(1 << 0);
	}

}

//PERIPHERAL CLK SETUP
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
		if(EnorDi == ENABLE){
			if(pI2Cx == I2C1)
				I2C1_PCLK_EN();
			else if(pI2Cx == I2C2)
				I2C2_PCLK_EN();
			else if(pI2Cx == I2C3)
				I2C3_PCLK_EN();
		}
		else{
			if(pI2Cx == I2C1)
				I2C1_PCLK_DI();
			else if(pI2Cx == I2C2)
				I2C2_PCLK_DI();
			else if(pI2Cx == I2C3)
				I2C3_PCLK_DI();
		}
}



//INIT
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp =0;
	//CR1 REG
	temp |= pI2CHandle->I2CConfig.I2C_ACKControl << 10 ; //Configure ACK bit in CR1 reg
	pI2CHandle->pI2Cx->CR1 = temp;

	//CR2 REG
	temp=0;
	temp |= RCC_GetPCLK1Value() / 1000000U ;
	pI2CHandle->pI2Cx->CR2 = temp & 0x3F;

	//OAR1 REF
	temp = 0;
	temp = pI2CHandle->I2CConfig.I2C_DeviceAddr << 1;
	temp = (1 << 14);
	pI2CHandle->pI2Cx->OAR1 = temp;

	//CCR REG
	uint16_t ccr;
	temp = 0;
	if(pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCLSPEED_SM)
	{
		ccr = RCC_GetPLLOutputClock() / (2*pI2CHandle->I2CConfig.I2C_SCLSpeed);
		temp |= (ccr & 0xFFF);
	}
	else
	{
		temp |= (1 << 15);
		temp |= (pI2CHandle->I2CConfig.I2C_FMDutyCyle << 14);
		if(pI2CHandle->I2CConfig.I2C_FMDutyCyle == I2C_FM_DUTY_2)
		ccr = RCC_GetPLLOutputClock() / (3*pI2CHandle->I2CConfig.I2C_SCLSpeed);
		else if(pI2CHandle->I2CConfig.I2C_FMDutyCyle == I2C_FM_DUTY_16_9)
		ccr = RCC_GetPLLOutputClock() / (25*pI2CHandle->I2CConfig.I2C_SCLSpeed);

		temp |= (ccr & 0xFFF);
	}

	pI2CHandle->pI2Cx->CCR = temp;

	//TRISE REG PENDING

	if(pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCLSPEED_SM)
		{
			//mode is standard mode

			temp = (RCC_GetPCLK1Value() /1000000U) + 1 ;

		}else
		{
			//mode is fast mode
			temp = ( (RCC_GetPCLK1Value() * 300) / 1000000000U ) + 1;

		}

		pI2CHandle->pI2Cx->TRISE = (temp & 0x3F);
}



//DE-INIT
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if(pI2Cx == I2C1)
		I2C1_REG_RESET();
	else if (pI2Cx == I2C2)
		I2C2_REG_RESET();
	else if (pI2Cx == I2C3)
		I2C3_REG_RESET();
}



void I2C_MasterSendData(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB))
	{
		I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);
	}

	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR))
	I2C_ClearADDRFlag(pI2CHandle);

	while(Len > 0)
	{
		while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
		pI2CHandle->pI2Cx->DR = *pTxbuffer;
		pTxbuffer++;
		Len--;
	}

	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));

	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
}

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr,uint8_t Sr)
{

	//1. Generate the START condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//2. confirm that start generation is completed by checking the SB flag in the SR1
	//   Note: Until SB is cleared SCL will be stretched (pulled to LOW)
	while( !  I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_SB)   );

	//3. Send the address of the slave with r/nw bit set to R(1) (total 8 bits )
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx,SlaveAddr);

	//4. wait until address phase is completed by checking the ADDR flag in teh SR1
	while( !  I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_ADDR)   );


	//procedure to read only 1 byte from slave
	if(Len == 1)
	{
		//Disable Acking
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACKDISABLE);


		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//wait until  RXNE becomes 1
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_RXNE) );

		//generate STOP condition
		if(Sr == I2C_DISABLE_SR )
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

		//read data in to buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;

	}


    //procedure to read data from slave when Len > 1
	if(Len > 1)
	{
		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//read the data until Len becomes zero
		for ( uint32_t i = Len ; i > 0 ; i--)
		{
			//wait until RXNE becomes 1
			while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_FLAG_RXNE) );

			if(i == 2) //if last 2 bytes are remaining
			{
				//Disable Acking
				I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACKDISABLE);

				//generate STOP condition
				if(Sr == I2C_DISABLE_SR )
					I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

			}

			//read the data from data register in to buffer
			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			//increment the buffer address
			pRxBuffer++;

		}

	}

	//re-enable ACKing
	if(pI2CHandle->I2CConfig.I2C_ACKControl == I2C_ACKENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACKENABLE);
	}

}


//SEND & RECEIVE DATA - NON-BLOCKING API
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pTxbuffer, uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxbuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition

		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);
		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVFEN);

		//Implement the code to enable ITERREN Control Bit

		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);
	}

	return busystate;

}
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr,uint8_t Sr)
{

	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition

		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);
		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVFEN);

		//Implement the code to enable ITERREN Control Bit



	}

	return busystate;
}


//IRG CONFIG FOR I2C
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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


void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. first lets find out the ipr register
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section  = IRQNumber %4 ;

	uint8_t shift_amount = ( 8 * iprx_section) + ( 8 - NO_PR_BITS_IMPLEMENTED) ;

	*(  NVIC_PR_BASE_ADDR + iprx ) |=  ( IRQPriority << shift_amount );
}



void I2C_EV_IRQHnadling(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp1, temp2, temp3;
	temp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVFEN);
	temp2 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3 = pI2CHandle->pI2Cx->SR1 |= ( 1 << I2C_SR1_SB);

	if(temp1 && temp3)
	{
		//SB
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);

		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
	}

	temp3 = pI2CHandle->pI2Cx->SR1 |= ( 1 << I2C_SR1_ADDR);

	if(temp1 && temp3)
		{
			//ADDR
		I2C_ClearADDRFlag(pI2CHandle);
		}

	temp3 = pI2CHandle->pI2Cx->SR1 |= ( 1 << I2C_SR1_BTF);

	if(temp1 && temp3)
	{
			//BTF
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			if(pI2CHandle->pI2Cx->SR1 |= ( 1 << I2C_SR1_TXE))
			{
				if(pI2CHandle->TxLen == 0)
				{
					//BTF SET AND TXE IS 1, SO BEGIN TERMINATION OF TRANSMISSON
					//GENERATE STOP CONDITION
					if(pI2CHandle->Sr == I2C_DISABLE_SR)
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

					//RESET ALL MEMBER REGISTERS TO END I2C PHERIPHERAL TRANSMISSION
					I2C_CloseSendData(pI2CHandle);

					//NOTIFY APPLICATION ABOUT THE TRANSMISSION COMPLETION
					I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_TX_CMPLT);
				}
			}
		}

		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			;
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 |= ( 1 << I2C_SR1_STOPF);

	if(temp1 && temp3)
		{
			//STOPF
			//READ SR1 REG AND THEN WRITE IS CR1 REG TO CLEAR THE STOPF FLAG
			//SR1 ALREADY READ IN TEMP VAR, NOW WRTIE IN CR1
			pI2CHandle->pI2Cx->CR1 |= 0x0000; //You not writing everything here you are simply rewritting what is written already
			//NOTIFY APPLICATION ABOUT THE STOP FLAG GEN
			I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_STOP);

		}

	temp3 = pI2CHandle->pI2Cx->SR1 |= ( 1 << I2C_SR1_TXE);

	if(temp1 && temp2 && temp3)
		{
			//TXE IS SET MEANING DR IS EMPTY. SO LOAD DATA INTO DR.
			//CHECK I2C BUSY IN TX AND IF THE MCU IS IN MASTER MODE BY CHECKING MSL BIT IN SR2
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}
		else
		{
			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))
			{
				I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_DATA_REQ);
			}
		}

		}

	temp3 = pI2CHandle->pI2Cx->SR1 |= ( 1 << I2C_SR1_RXNE);

	if(temp1 && temp2 && temp3)
	{
			//RXNE
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			{
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		}
		else
		{
			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))
			{
				I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_DATA_RCV);
			}
		}
	}

}

void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx,uint8_t data)
{
	pI2Cx->DR = data;
}
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx)
{
	return (uint8_t)pI2Cx->DR;
}



void I2C_ER_IRQHnadling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
		//Implement the code to clear the arbitration lost error flag
			pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);
			pI2CHandle->pI2Cx->SR2 &= (1 << I2C_SR2_MSL);
		//Implement the code to notify the application about the error
			I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);
		}
	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);
		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);
		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);
		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}

}


__attribute__((weak)) void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{
//weak implementation
}
