/*
 * I2C.c
 *
 *  Created on: May 4, 2024
 *      Author: Mohamed Magdi
 */

#include "I2C.h"

// ------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------

I2C_Config_t gI2CConfig[2];

// ------------------------------------------------------------------
// Generic Macros
// ------------------------------------------------------------------
#define I2C1_INDEX						0
#define I2C2_INDEX						1

// ------------------------------------------------------------------
// APIs
// ------------------------------------------------------------------


/**************************** GENERIC APIs ********************************/
void I2C_GenerateStart(I2C_TYPE_DEF* I2Cx, Functional_State newState, Start_Condition startState){
	// Check if the start is repeated start.
	if(startState != REPEATED_START){
		// Check if the Bus is idle.
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	}

	if(newState != DISABLE){
		// Generate Start Condition.
		I2Cx ->CR1 |= I2C_CR1_START;
	}else {
		// Disable Start Condition Generation.
		I2Cx ->CR1 &= ~(I2C_CR1_START);
	}
}


void I2C_GenerateStop(I2C_TYPE_DEF* I2Cx, Functional_State stopState){
	if(stopState  != DISABLE){
		// Generate a stop condition.
		I2Cx -> CR1 |= I2C_CR1_STOP;
	}else{
		// Disable Stop Generation.
		I2Cx -> CR1 &= ~(I2C_CR1_STOP);
	}
}


Flag_Status I2C_GetFlagStatus(I2C_TYPE_DEF* I2Cx, Status flag){
	//volatile uint32_t dummyRead;
	uint32_t SR1Flag, SR2Flag, SR1_SR2_Flag;
	Flag_Status bitStatus = RESET;

	switch(flag) {
	case I2C_FLAG_BUSY:
		if((I2Cx->SR2) & (I2C_SR2_BUSY)){
			bitStatus = SET;
		}else {
			bitStatus = RESET;
		}
		break;

	case EV5:
		if((I2Cx->SR1) & (I2C_SR1_SB)){
			bitStatus = SET;
		}else {
			bitStatus = RESET;
		}
		break;

	case EV6:
		if((I2Cx->SR1) & (I2C_SR1_ADDR)){
			bitStatus = SET;
		}else {
			bitStatus = RESET;
		}
		// To clear ADDr Field: cleard by reading SR1 Register followed by reading SR2.
		//dummyRead = I2Cx->SR2;
		break;

	case MASTER_BYTE_TRANSMITTING:
		SR1Flag = I2Cx -> SR1;
		SR2Flag = I2Cx -> SR2;
		SR2Flag = SR2Flag << 16;

		SR1_SR2_Flag = (SR1Flag | SR2Flag) & ((uint32_t)0x00FFFFFF);

		if((SR1_SR2_Flag & I2C_EVENT_MASTER_BYTE_TRANSMITTING) == I2C_EVENT_MASTER_BYTE_TRANSMITTING){
			bitStatus = SET;
		}else {
			bitStatus = RESET;
		}
		break;

	case EV8:
		if((I2Cx->SR1) & (I2C_SR1_TXE)){
			bitStatus = SET;
		}else {
			bitStatus = RESET;
		}
		break;

	case EV7:
		if((I2Cx->SR1) & (I2C_SR1_RXNE)){
			bitStatus = SET;
		}else {
			bitStatus = RESET;
		}
		break;
	}


	return bitStatus;
}

void I2C_SendAddress(I2C_TYPE_DEF* I2Cx, uint16_t address, I2C_Direction direction){
	address = address << 1;

	if(direction != WRITE){
		address |= 1 << 0;
	}else {
		address &= ~(1 << 0);
	}

	I2Cx->DR = address;
}

void I2C_ACKConfig(I2C_TYPE_DEF* I2Cx, Functional_State state){
	if(state == ENABLE){
		I2Cx -> CR1 |= (I2C_CR1_ACK);
	}else if(state == DISABLE){
		I2Cx -> CR1 &= ~(I2C_CR1_ACK);
	}
}




/**************************************************************************/




void MCAL_I2C_Init(I2C_TYPE_DEF* I2Cx, I2C_Config_t* i2cCfg){
	uint16_t tempReg, freqRange, ccrVal;
	uint32_t pclk1 = 8000000;

	if(I2Cx == I2C1){
		gI2CConfig[I2C1_INDEX] = *i2cCfg;
		RCC_I2C1_CLK_EN();
	}else if(I2Cx == I2C2){
		gI2CConfig[I2C2_INDEX] = *i2cCfg;
		RCC_I2C2_CLK_EN();
	}

	if(i2cCfg->mode == I2C_Mode_I2C){
		/*************** Configure Timing ********************/
		// CR2 - Bits 5:0 FREQ[5:0]: Peripheral clock frequency.
		tempReg = I2Cx->CR2;
		// Clear Frequency FREQ[5:0] Bits.
		tempReg &= ~(I2C_CR2_FREQ_Msk);
		// Get pclk1 Frequency Value.
		pclk1 = MCAL_RCC_GET_PCLK1Freq();
		// set Frequency bits depending on pclk1 value.
		freqRange = (uint16_t)(pclk1 / 1000000);

		tempReg |= freqRange;

		I2Cx->CR2 = tempReg;

		// Configure clock Control Register (I2C_CCR).

		// Disable the selected I2C peripheral to configure time.
		I2Cx->CR1 &= ~(I2C_CR1_PE);

		tempReg = 0;

		// Configure Speed in Standard Mode.
		if((i2cCfg -> clkSpeed) == I2C_SCLK_SM_50K || (i2cCfg -> clkSpeed) == I2C_SCLK_SM_100K){
			// Standard Mode Speed Calculations.
			ccrVal = (uint16_t)(pclk1 / ((i2cCfg -> clkSpeed) * 2));
			tempReg |= ccrVal;

			I2Cx->CCR = tempReg;

			// Configure Rise Time (TRISE):
			/* For instance: in Sm mode, the maximum allowed SCL rise time is 1000 ns.
			 * If, in the I2C_CR2 register, the value of FREQ[5:0] bits is equal to 0x08 and TPCLK1 = 125 ns
			 * therefore the TRISE[5:0] bits must be programmed with 09h.
			 * (1000 ns / 125 ns = 8 + 1)
			 */
			I2Cx->TRISE = freqRange + 1;
		}else {
			// Fast Mode Not Supported Yet
		}

		/*************** Configure CR1 ********************/
		tempReg = 0;
		tempReg = I2Cx->CR1;
		tempReg |= (uint16_t)(i2cCfg->I2CACKControl | i2cCfg -> generalCall | i2cCfg -> mode | i2cCfg -> stretchMode);
		I2Cx->CR1 = tempReg;

		/*************** Configure OAR1 & OAR2 ****************/

		tempReg = 0;
		if(i2cCfg->slaveAddCfg.dualAddEn == 1){
			tempReg = I2C_OAR2_ENDUAL;
			tempReg = i2cCfg->slaveAddCfg.secondarySlaveAdd << I2C_OAR2_ADD2_Pos;
			I2Cx->OAR2 = tempReg;
		}

		tempReg = 0;
		tempReg |= i2cCfg->slaveAddCfg.primarySlaveAdd << 1;
		tempReg |= i2cCfg->slaveAddCfg.slaveAddressingMode;
		I2Cx->OAR1 = tempReg;
	}else {
		// SMBUS Mode Is Not Supported.
	}

	// Interrupt Mode (Slave Mode) // check Callback Pointer != NULL
	if(i2cCfg->PSlaveEventCallBack != NULL){
		I2Cx->CR2 |= (I2C_CR2_ITERREN);
		I2Cx->CR2 |= (I2C_CR2_ITEVTEN);
		I2Cx->CR2 |= (I2C_CR2_ITBUFEN);

		if(I2Cx == I2C1){
			NVIC_IRQ31_I2C1_EV_Enable;
			NVIC_IRQ32_I2C1_ER_Enable;
		}else if(I2Cx == I2C2){
			NVIC_IRQ33_I2C2_EV_Enable;
			NVIC_IRQ34_I2C2_ER_Enable;
		}
		I2Cx -> SR1 = 0;
		I2Cx -> SR2 = 0;
	}

	// Enable Selected I2C peripheral.
	I2Cx->CR1 |= I2C_CR1_PE;
}






void MCAL_I2C_DeInit(I2C_TYPE_DEF* I2Cx){
	if(I2Cx == I2C1){
		NVIC_IRQ31_I2C1_EV_Disable;
		NVIC_IRQ32_I2C1_ER_Disable;
		RCC_I2C1_CLK_EN();
	}else if(I2Cx == I2C2){
		NVIC_IRQ33_I2C2_EV_Disable;
		NVIC_IRQ34_I2C2_ER_Disable;
		RCC_I2C1_CLK_Dis();
	}
}




void MCAL_I2C_GPIO_SetPins(I2C_TYPE_DEF* I2Cx){
	GPIO_PinConfig_t pinCfg;

	if(I2Cx == I2C1){
		// PB6: I2C1_SCL
		// PB7: I2C1_SDA
		pinCfg.GPIO_PinNumber = GPIO_PIN_6;
		pinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD;
		pinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &pinCfg);

		pinCfg.GPIO_PinNumber = GPIO_PIN_7;
		pinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD;
		pinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &pinCfg);
	}else if(I2Cx == I2C2){
		// PB10: I2C1_SCL
		// PB11: I2C1_SDA

		pinCfg.GPIO_PinNumber = GPIO_PIN_10;
		pinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD;
		pinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &pinCfg);

		pinCfg.GPIO_PinNumber = GPIO_PIN_11;
		pinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD;
		pinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &pinCfg);
	}
}



//Master APIs ( Polling Mechanism)
void MCAL_I2C_Master_TX(I2C_TYPE_DEF* I2Cx, uint16_t slaveAdd, uint8_t* dataOut, uint32_t dataLen, Stop_Condition stopState, Start_Condition startState){
	int i;

	// 1.Set the start bit in the I2C.CR1 Reg to generate a start condition.
	I2C_GenerateStart(I2Cx, ENABLE, startState);

	// 2. Wait for EV5
	/* Bit 0  SB: Start bit (Master mode)
	 * 0: No Start condition
	 * 1: Start condition generated.
	 * – Set when a Start condition generated.
	 * – Cleared by software by reading the SR1 register followed by writing the DR register, or by
	 * hardware when PE=0
	 */
	while(!I2C_GetFlagStatus(I2Cx, EV5));

	// 3. Send Slave Address.
	I2C_SendAddress(I2Cx, slaveAdd, WRITE);

	// 4. Wait For EV6
	/* Bit 1  ADDR: Address sent (master mode)/matched (slave mode)
	 * This bit is cleared by software reading SR1 register followed reading SR2, or by hardware
	 * when PE=0.
	 * Address matched (Slave)
	 * 0: Address mismatched or not received.
	 * 1: Received address matched.
	 * – Set by hardware as soon as the received slave address matched with the OAR registers
	 * content or a general call or a SMBus Device Default Address or SMBus Host or SMBus Alert
	 * is recognized. (when enabled depending on configuration).
	 */
	while(!I2C_GetFlagStatus(I2Cx, EV6));

	// 5. Wait For Master Byte Transmitting Flags.
	// TRA, BUSY, MSL, TXE Flags .
	while(!I2C_GetFlagStatus(I2Cx, MASTER_BYTE_TRANSMITTING));

	for(i = 0; i < dataLen; i++){
		// Write the DR register.
		I2Cx->DR = dataOut[i];
		// Wait for EV8.
		while(!I2C_GetFlagStatus(I2Cx, EV8));
	}

	// 6. Stop Conidtion.
	if(stopState == WITH_STOP){
		// Send Stop Condition.
		I2C_GenerateStop(I2Cx, ENABLE);
	}
}


void MCAL_I2C_Master_RX(I2C_TYPE_DEF* I2Cx, uint16_t slaveAdd, uint8_t* dataIn, uint32_t dataLen, Stop_Condition stopState, Start_Condition startState){
	int i;
	uint8_t index = I2Cx == I2C1 ? I2C1_INDEX : I2C2_INDEX;
	// 1.Set the start bit in the I2C.CR1 Reg to generate a start condition.
	I2C_GenerateStart(I2Cx, ENABLE, startState);

	// 2. Wait for EV5
	/* Bit 0  SB: Start bit (Master mode)
	 * 0: No Start condition
	 * 1: Start condition generated.
	 * – Set when a Start condition generated.
	 * – Cleared by software by reading the SR1 register followed by writing the DR register, or by
	 * hardware when PE=0
	 */
	while(!I2C_GetFlagStatus(I2Cx, EV5));

	// 3. Send Slave Address.
	I2C_SendAddress(I2Cx, slaveAdd, READ);

	// 4. Wait For EV6
	/* Bit 1  ADDR: Address sent (master mode)/matched (slave mode)
	 * This bit is cleared by software reading SR1 register followed reading SR2, or by hardware
	 * when PE=0.
	 * Address matched (Slave)
	 * 0: Address mismatched or not received.
	 * 1: Received address matched.
	 * – Set by hardware as soon as the received slave address matched with the OAR registers
	 * content or a general call or a SMBus Device Default Address or SMBus Host or SMBus Alert
	 * is recognized. (when enabled depending on configuration).
	 */
	while(!I2C_GetFlagStatus(I2Cx, EV6));

	I2C_ACKConfig(I2Cx, ENABLE);


	if(dataLen){
		for(i = dataLen; i > 1; i--){
			while(!I2C_GetFlagStatus(I2Cx, EV7));
			*dataIn = I2Cx->DR;
			dataIn++;
		}
		I2C_ACKConfig(I2Cx, DISABLE);
	}

	if(stopState == WITH_STOP){
		I2C_GenerateStop(I2Cx, ENABLE);
	}

	if(gI2CConfig[index].I2CACKControl == I2C_ACK_EN){
		I2C_ACKConfig(I2Cx, ENABLE);
	}
}


/**************************** SLAVE APIs ***************************/

void MCAL_I2C_SlaveSendData(I2C_TYPE_DEF* I2Cx, uint8_t data){
	I2Cx->DR = data;
}
uint8_t MCAL_I2C_SlaveReceiveData(I2C_TYPE_DEF* I2Cx){
	return (uint8_t) I2Cx->DR;
}




/******************************** ISRs **********************************/

void I2C1_EV_IRQHandler(void){

}
void I2C1_ER_IRQHandler(void){

}
void I2C2_EV_IRQHandler(void){

}
void I2C2_ER_IRQHandler(void){

}












