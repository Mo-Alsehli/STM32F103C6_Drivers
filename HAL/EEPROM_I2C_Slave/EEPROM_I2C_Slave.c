/*
 * EEPROM_I2C_Slave.c
 *
 *  Created on: May 9, 2024
 *      Author: Mohamed Magdi
 */

#include "EEPROM_I2C_Slave.h"

void EEPROM_Init(void){
	// PB6: I2C1_SCL
	// PB7: I2C1_SDA

	I2C_Config_t I2C1Cfg;

	I2C1Cfg.generalCall = I2C_General_Call_EN;
	I2C1Cfg.I2CACKControl = I2C_ACK_EN;
	I2C1Cfg.clkSpeed = I2C_SCLK_SM_100K;
	I2C1Cfg.mode = I2C_Mode_I2C;
	I2C1Cfg.PSlaveEventCallBack = NULL;
	I2C1Cfg.stretchMode = I2C_Stretch_Mode_EN;

	MCAL_I2C_GPIO_SetPins(I2C1);
	MCAL_I2C_Init(I2C1, &I2C1Cfg);
}
uint8_t EEPROM_Write_NBytes(uint32_t memoryAdd, uint8_t* bytes, uint8_t dataLen){
	uint8_t i = 0;

	uint8_t buffer[256];

	buffer[0] = (uint8_t)(memoryAdd >> 8);
	buffer[1] = (uint8_t)(memoryAdd);

	for(i = 2; i < (dataLen + 2); i++){
		buffer[i] = bytes[i-2];
	}

	MCAL_I2C_Master_TX(I2C1, EEPROM_Slave_Address, buffer, (dataLen + 2), WITH_STOP, START);

	return 0;
}
uint8_t EEPROM_Read_Byte(uint32_t address, uint8_t* data, uint8_t dataLen){
	uint8_t buffer[2];
	buffer[0] = (uint8_t)(address >> 8);
	buffer[1] = (uint8_t)(address);

	MCAL_I2C_Master_TX(I2C1, EEPROM_Slave_Address, buffer, 2, WITHOUT_STOP, START);

	MCAL_I2C_Master_RX(I2C1, EEPROM_Slave_Address, (uint8_t*)data, dataLen, WITH_STOP, REPEATED_START);

	return 0;
}
