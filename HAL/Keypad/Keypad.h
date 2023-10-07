/*
 * Keypad.h
 *
 *  Created on: Oct 7, 2023
 *      Author: dell
 */

#ifndef KEYPAD_KEYPAD_H_
#define KEYPAD_KEYPAD_H_



#include "GPIO_Driver.h"

#define KEYPAD_PORT	GPIOB

// Rows
#define R0 GPIO_PIN_0
#define R1 GPIO_PIN_1
#define R2 GPIO_PIN_3
#define R3 GPIO_PIN_4
// Columns
#define C0 GPIO_PIN_5
#define C1 GPIO_PIN_6
#define C2 GPIO_PIN_7
#define C3 GPIO_PIN_8

void Keypad_Init(void);
uint8_t Keypad_GetChar(void);


#endif /* KEYPAD_KEYPAD_H_ */
