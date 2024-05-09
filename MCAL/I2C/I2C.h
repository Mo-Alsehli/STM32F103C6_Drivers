/*
 * I2C.h
 *
 *  Created on: May 4, 2024
 *      Author: Mohamed Magdi
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_

//------------------------------------------
// Includes
//------------------------------------------

#include "STM32F103x6.h"
#include "GPIO_Driver.h"
#include "RCC.h"

//------------------------------------------
// User Type Definitions (Structures)
//------------------------------------------

struct S_I2C_Slave_Address {
	uint16_t dualAddEn; // EN: 1, Dis: 0.
	uint16_t primarySlaveAdd;
	uint16_t secondarySlaveAdd;
	uint32_t slaveAddressingMode; // 7-bit or 10-bit.
							// @ref I2C_Addressing_Mode_Slave_define
};

typedef enum {
	I2C_EV_STOP,
	I2C_ERROR_AF,
	I2C_EV_ADDR_MATCHED,
	I2C_EV_DATA_REQ,
	I2C_EV_DATA_RCV
}Slave_State;


// Enumeration for the Stop Condition State.
typedef enum{
	WITH_STOP,
	WITHOUT_STOP
}Stop_Condition;

// Enumeration for the Start Condition Checking if there is a repeated start or not.
typedef enum{
	START,
	REPEATED_START
}Start_Condition;

// Enum EN or DIS For The Start and Stop Condition.
typedef enum {
	DISABLE,
	ENABLE
}Functional_State;

// Enum for Flags Status
typedef enum {
	RESET,
	SET
}Flag_Status;

// Enums for All Flags Status.
typedef enum {
	I2C_FLAG_BUSY = 0,
	EV5,
	EV6,
	EV7,
	MASTER_BYTE_TRANSMITTING,
	EV8
}Status;

// Enum for I2C direction (R/W)
typedef enum {
	WRITE,
	READ
}I2C_Direction;

typedef struct {
	uint32_t clkSpeed; /* Specifies clock frequency
						* This variable must be set @ref I2C_SCLK_define
						*/
	uint32_t stretchMode;// @ref I2C_Stretch_Time
	uint32_t mode;		 /* specifies I2C mode (I2C  - SMBus)
						  * This parameter must be set @ref I2C_Mode_define
						  */
	struct S_I2C_Slave_Address slaveAddCfg;
	uint32_t I2CACKControl;	/* Controls the acknowledgement.
							* This parameter can be a value of @ref I2C_ACK_define
							*/
	uint32_t generalCall;	// @ref I2C_General_Call_define

	void (*PSlaveEventCallBack)(Slave_State state);
}I2C_Config_t;


//------------------------------------------
// Macros Configuration References
//------------------------------------------

/*************************** Generic Macros *****************************/

// In this macro we handles four flags for a specific event (EV8_1) together in one variable.
// They are in different two registers so we devided the variable into two 16bits parts.
// Then we assigned a SET value for each of the variables in the register and get the hex value of it.
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING		((uint32_t)0x00070080) // TRA, BUSY, MSL, TXE flags.

/* @ref I2C_SCLK_define
 * Standard Speed (up to 100 kHz) (Supported In Driver).
 * Fast Speed (up to 400 kHz) (Not Supported In Driver).
 * Bits 5:0 FREQ[5:0]: Peripheral clock frequency
 * The FREQ bits must be configured with the APB clock frequency value (I2C peripheral
 * connected to APB). The FREQ field is used by the peripheral to generate data setup and
 * hold times compliant with the I2C specifications. The minimum allowed frequency is 2 MHz,
 * the maximum frequency is limited by the maximum APB frequency and cannot exceed
 * 50 MHz (peripheral intrinsic maximum limit).
 * 0b000000: Not allowed
 * 0b000001: Not allowed
 * 0b000010: 2 MHz
 * -- We also configure CCR register: Thigh = CCR * Tpclk1.
 * -- Configure Rise Time Depending on TRM Calculations.
 */

#define I2C_SCLK_SM_50K							(50000U)	// Supported in Standard mode from 10 to 100Khz
															// so we have choosed 50Khz for this driver and we
															// we can add more frequencis in between.
#define I2C_SCLK_SM_100K						(100000U)


/*  @ref I2C_Stretch_Time
 * NOSTRETCH: Clock stretching disable (Slave mode)
 * This bit is used to disable clock stretching in slave mode when ADDR or BTF flag is set, until
 * it is reset by software.
 * 0: Clock stretching enabled
 * 1: Clock stretching disabled
 */

#define I2C_Stretch_Mode_EN						(0x00000000U)
#define I2C_Stretch_Mode_Dis					I2C_CR1_NOSTRETCH


/*@ref I2C_Mode_define
 * CR1:
 * Bit 1  SMBUS: SMBus mode
 * 0: I2C mode
 * 1: SMBus mode
 */

#define I2C_Mode_I2C							(0x00000000U)
#define I2C_Mode_SMBus							I2C_CR1_SMBUS

/* @ref I2C_Addressing_Mode_Slave_define
 */

#define I2C_Addressing_Mode_Slave_7Bit			(0x00000000U)
#define I2C_Addressing_Mode_Slave_10Bit			(uint16_t)(1 << 15)


/* @ref I2C_ACK_define
 */

#define I2C_ACK_EN								I2C_CR1_ACK
#define I2C_ACK_DIS								((uint16_t)0x0000)


/* @ref I2C_General_Call_define
 * Bit 6  ENGC: General call enable
 * 0: General call disabled. Address 00h is NACKed.
 * 1: General call enabled. Address 00h is ACKed.
 */

#define I2C_General_Call_EN						I2C_CR1_ENGC
#define I2C_General_Call_DIS					((uint16_t)0x0000)


//------------------------------------------
// APIs Supported By "MCAL I2C  DRIVER"
//------------------------------------------

void MCAL_I2C_Init(I2C_TYPE_DEF* I2Cx, I2C_Config_t* i2cCfg);
void MCAL_I2C_DeInit(I2C_TYPE_DEF* I2Cx);
void MCAL_I2C_GPIO_SetPins(I2C_TYPE_DEF* I2Cx);

//Master APIs ( Polling Mechanism)
void MCAL_I2C_Master_TX(I2C_TYPE_DEF* I2Cx, uint16_t slaveAdd, uint8_t* dataOut, uint32_t dataLen, Stop_Condition stopState, Start_Condition startState);
void MCAL_I2C_Master_RX(I2C_TYPE_DEF* I2Cx, uint16_t slaveAdd, uint8_t* dataIn, uint32_t dataLen, Stop_Condition stopState, Start_Condition startState);

/**************************** SLAVE APIs ***************************/
// Interrupt Mechanism
void MCAL_I2C_SlaveSendData(I2C_TYPE_DEF* I2Cx, uint8_t data);
uint8_t MCAL_I2C_SlaveReceiveData(I2C_TYPE_DEF* I2Cx);



/************************* Generic APIs ***************************/

void I2C_GenerateStart(I2C_TYPE_DEF* I2Cx, Functional_State newState, Start_Condition startState);
Flag_Status I2C_GetFlagStatus(I2C_TYPE_DEF* I2Cx, Status flag);
void I2C_SendAddress(I2C_TYPE_DEF* I2Cx, uint16_t address, I2C_Direction direction);
void I2C_GenerateStop(I2C_TYPE_DEF* I2Cx, Functional_State stopState);
void I2C_ACKConfig(I2C_TYPE_DEF* I2Cx, Functional_State state);



#endif /* I2C_I2C_H_ */
