/*
 * Keypad.c
 *
 *  Created on: Oct 7, 2023
 *      Author: dell
 */




#include "Keypad.h"

uint16_t keypadRow[] = {R0, R1, R2, R3};
uint16_t keypadCol[] = {C0, C1, C2, C3};


void Keypad_Init(){
	GPIO_PinConfig_t	pinCnf;
	pinCnf.GPIO_PinNumber = R0;
	pinCnf.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCnf.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	MCAL_GPIO_Init(KEYPAD_PORT, &pinCnf);

	pinCnf.GPIO_PinNumber = R1;
	pinCnf.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCnf.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	MCAL_GPIO_Init(KEYPAD_PORT, &pinCnf);

	pinCnf.GPIO_PinNumber = R2;
	pinCnf.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCnf.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	MCAL_GPIO_Init(KEYPAD_PORT, &pinCnf);

	pinCnf.GPIO_PinNumber = R3;
	pinCnf.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCnf.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	MCAL_GPIO_Init(KEYPAD_PORT, &pinCnf);

	pinCnf.GPIO_PinNumber = C0;
	pinCnf.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCnf.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	MCAL_GPIO_Init(KEYPAD_PORT, &pinCnf);

	pinCnf.GPIO_PinNumber = C1;
	pinCnf.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCnf.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	MCAL_GPIO_Init(KEYPAD_PORT, &pinCnf);

	pinCnf.GPIO_PinNumber = C2;
	pinCnf.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCnf.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	MCAL_GPIO_Init(KEYPAD_PORT, &pinCnf);

	pinCnf.GPIO_PinNumber = C3;
	pinCnf.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCnf.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	MCAL_GPIO_Init(KEYPAD_PORT, &pinCnf);

	MCAL_GPIO_WritePort(KEYPAD_PORT, GPIO_PORT_HIGH);
}

uint8_t Keypad_GetChar(){
	uint8_t i, j;
	for(i = 0; i < 4; i++){
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypadCol[0], GPIO_PIN_HIGH);
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypadCol[1], GPIO_PIN_HIGH);
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypadCol[2], GPIO_PIN_HIGH);
		MCAL_GPIO_WritePin(KEYPAD_PORT, keypadCol[3], GPIO_PIN_HIGH);

		MCAL_GPIO_WritePin(KEYPAD_PORT, keypadCol[i], GPIO_PIN_LOW);

		for(j = 0; j < 4; j++){
			if((MCAL_GPIO_ReadPin(KEYPAD_PORT, keypadRow[j]) == 0)){
				while((MCAL_GPIO_ReadPin(KEYPAD_PORT, keypadRow[j]) == 0));
				switch(i){
					case 0:
						if     (j == 0) return '7';
						else if(j == 1) return '4';
						else if(j == 2) return '1';
						else if(j == 3) return '!'; // If it's return we will perform clear screen function.
						break;

					case 1:
						if     (j == 0) return '8';
						else if(j == 1) return '5';
						else if(j == 2) return '2';
						else if(j == 3) return '0';
						break;

					case 2:
						if     (j == 0) return '9';
						else if(j == 1) return '6';
						else if(j == 2) return '3';
						else if(j == 3) return '=';
						break;

					case 3:
						if     (j == 0) return '/';
						else if(j == 1) return '*';
						else if(j == 2) return '-';
						else if(j == 3) return '+';
						break;
				}
			}
		}
	}
	return 'A';
}


