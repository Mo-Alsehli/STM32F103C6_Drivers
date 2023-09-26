/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Mohamed Magdi
 * @brief          : Main program body
 ****************************************************************************** */

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif
<<<<<<< HEAD
<<<<<<< HEAD

=======
// Here after Pulling.
>>>>>>> a7c680c3190bdd1a6420d1e76049e5c1a00dd4d8
=======
// Here after Pulling and Then with a small change.
>>>>>>> d462df7c3629d7d7645dcadb6b6755682f532c4b
#include "STM32F103x6.h"
#include "GPIO_Driver.h"

void clockInit(){
	RCC_GPIOA_CLk_EN();
	RCC_GPIOB_CLk_EN();
}

void gpioInit(){
	GPIO_PinConfig_t pinCfg;

	// PA Flaoting Input PIN 1.
	pinCfg.GPIO_PinNumber = GPIO_PIN_1;
	pinCfg.GPIO_MODE = GPIO_MODE_INPUT_PU;
	// PA Init
	MCAL_GPIO_Init(GPIOA, &pinCfg);

	// PB Output Push-pull PIN 1.
	pinCfg.GPIO_PinNumber = GPIO_PIN_1;
	pinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	// PB Init
	MCAL_GPIO_Init(GPIOB, &pinCfg);

	// PA Flaoting Input PIN 13.
	pinCfg.GPIO_PinNumber = GPIO_PIN_13;
	pinCfg.GPIO_MODE = GPIO_MODE_INPUT_PD;
	// PA Init
	MCAL_GPIO_Init(GPIOA, &pinCfg);

	// PB Output Push-pull PIN 13.
	pinCfg.GPIO_PinNumber = GPIO_PIN_13;
	pinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	pinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
	// PB Init
	MCAL_GPIO_Init(GPIOB, &pinCfg);
}

void wait(uint32_t time){
	uint32_t i, j;
	for(i = 0; i < time; i++){
		for(j = 0; j < 255; j++);
	}
}

int main(void)
{
	clockInit();
	gpioInit();

	while(1){
		// Single Press
		if(MCAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 1){
			MCAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
			while(MCAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 1);
		}
		// Mutliple Press
		if(MCAL_GPIO_ReadPin(GPIOA, GPIO_PIN_13) == 1){
			MCAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
		}
		wait(1);
	}
}
