/*
 * EEPROM_I2C_Slave.h
 *
 *  Created on: May 9, 2024
 *      Author: Mohamed Magdi
 */

#ifndef EEPROM_I2C_SLAVE_EEPROM_I2C_SLAVE_H_
#define EEPROM_I2C_SLAVE_EEPROM_I2C_SLAVE_H_

#include "I2C.h"

#define EEPROM_Slave_Address			0x2A

void EEPROM_Init(void);
uint8_t EEPROM_Write_NBytes(uint32_t memoryAdd, uint8_t* bytes, uint8_t dataLen);
uint8_t EEPROM_Read_Byte(uint32_t address, uint8_t* data, uint8_t dataLen);


#endif /* EEPROM_I2C_SLAVE_EEPROM_I2C_SLAVE_H_ */
