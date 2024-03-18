/*
 * TIM.c
 *
 *  Created on: Mar 2, 2024
 *      Author: Mohamed Magdi
 */

#include "TIM.h"

// Global Variables

uint8_t delayFlag = 1;
uint16_t overflowTimsTIM4 = 0;
uint16_t timerTicksTIM4 = 0;
uint16_t overflowTimsTIM3 = 0;
uint16_t timerTicksTIM3 = 0;
volatile uint64_t millis_count = 0;


// Static Functions

static uint32_t counts_time(uint32_t* timx, uint32_t clk){
	uint32_t  x = 0;
	if(timx == TIM4){
		timerTicksTIM4 = TIM4->CNT;
		x = ((timerTicksTIM4 + overflowTimsTIM4 * 64000)) / (clk/1000000); // (time in micro-seconds)
		overflowTimsTIM4 = 0;
		timerTicksTIM4 = 0;
	}else if(timx == TIM3){
		timerTicksTIM3 = TIM3->CNT;
		x = ((timerTicksTIM3 + overflowTimsTIM3 * 64000)) / (clk/1000000); // (time in micro-seconds)
		overflowTimsTIM3 = 0;
		timerTicksTIM3 = 0;
	}
	return x;
}




void delay(uint16_t time, uint8_t unit, uint32_t clk){
	delayFlag = 1;
	// disable NVIC Interrupt
	//NVIC_IRQ28_TIM2_Disable;
	// Enable Clock For Selected Timer
	RCC_TIM2_CLk_EN();

	// Timer off (be sure that is configured from the off state (Optional))
	TIM2->CR1 &= ~(1 << 0);

	uint8_t  userFlag = 1;
	uint32_t userTop  = 0;
	uint32_t userPre  = 1;
	uint32_t timeUnit = 1000;
	uint8_t  increase = 2;

	if(unit == 0){
		timeUnit = 1000;
		if(time > 3000){
			increase = 100;
		}else{
			increase = 10;
		}
	}else {
		timeUnit = 1000000;
		if(time > 3000){
			increase = 10;
		}else {
			increase = 5;
		}
	}

	while(userFlag == 1){

		userTop = (clk/timeUnit*time)/(userPre);
		//7272 = (8000000/1000*time)/10;
		//7272 = (8000 * time)/10;
		//72720 / 8000 =

		if(userTop >= 32000){
			if(userPre > 65530){
				userPre = 65530;
				userFlag = 0;
			}else {
				userPre += increase;
			}
		}else {
			userFlag = 0;
		}
	}

	// Timer Off
	TIM2->CR1 &= ~(1<<CEN);
	/*URS:
	 * Update request source
	 * This bit is set and cleared by software to select the UEV event sources.
	 * 0: Any of the following events generate an update interrupt or DMA request if enabled.
	 * These events can be:
	 * Counter overflow/underflow
	 */
	// Only counter overflow/underflow generates an update
	TIM2->CR1 |= (1 << URS);
	/*
	 * Bit 0  UIE: Update interrupt enable
	 *0: Update interrupt disabled.
	 *1: Update interrupt enabled.
	 */
	// update interrupt enabled
	TIM2->DIER |= (1 << UIE);

	TIM2->ARR = userTop;
	TIM2->PSC = (userPre - 1);
	// Re-Initialize the counter and generates an update of the registers.
	TIM2->EGR |= (1 << 0);
	// Enable Timer
	TIM2->CR1 |= (1 << CEN);

	NVIC_IRQ28_TIM2_Enable; 

	while(delayFlag);
}


uint32_t timeCalc(TIMx_TYPE_DEF* timx, uint32_t clk, uint8_t timerState){
	if(timerState == TIM_START){

		// Enable Clock For Selected Timer
		if(timx == TIM4){
			RCC_TIM4_CLk_EN();
		}else if(timx == TIM3){
			RCC_TIM3_CLk_EN();
		}


		timx -> CR1 &= ~(1<<CEN);
		timx -> CR1 |= (1 << URS);
		timx -> DIER |= (1 << UIE);

		/*
		 * delay = (pre * ticks) / freq.
		 * 8 * 10e-3 = (1 * ticks) / 8*10e6
		 */
		// 1 * 10e-3 = (1 * ticks) / 8*10e6
		timx->ARR = 64000; // Peak value to get delay of 8ms at freq = 8000000hz
		/*
		 * Bit 0  UG: Update generation
		 * This bit can be set by software, it is automatically cleared by hardware.
		 * 0: No action
		 * 1: Re-initialize the counter and generates an update of the registers. Note that the prescaler
		 * counter is cleared too (anyway the prescaler ratio is not affected). The counter is cleared if
		 * the center-aligned mode is selected or if DIR=0 (upcounting), else it takes the auto-reload
		 * value (TIMx_ARR) if DIR=1 (downcounting).
		 */
		timx->PSC = 0;
		timx->EGR |= (1<<0);
		// Enable Timer
		timx->CR1 |= (1 << 0);

		if(timx == TIM3){
			NVIC_IRQ29_TIM3_Enable;
		}else if(timx == TIM4){
			NVIC_IRQ30_TIM4_Enable;
		}

		return 1;
	}else {
		timx -> CR1 &= ~(1 << CEN);
		uint32_t x = counts_time(timx, clk);
		return x;
	}
}



/*
 * This function is used to initialize TIM3 to count every 1 milli-second.
 * It simulates the Millis() function in Arduino
 */
void millisInit(void) {
	// Enable TIM1 clock
	RCC_TIM3_CLk_EN();

	TIM3 -> CR1 &= ~(1<<CEN);
	TIM3 -> CR1 |= (1 << URS);
	TIM3 -> DIER |= (1 << UIE);

	/*
	 * delay = (pre * ticks) / freq.
	 */
	TIM3->ARR = 1000; // Peak value to get delay of 1ms at freq = 8000000hz

	TIM3->PSC = 8;
	TIM3->EGR |= (1<<0);
	// Enable Timer
	TIM3->CR1 |= (1 << 0);


	NVIC_IRQ29_TIM3_Enable;

}

uint64_t Millis(void) {
	return millis_count;
}




// IRQ for delay Function with Timer2
void TIM2_IRQHandler(){
	TIM2->SR &= ~(1 << 0); // Update Interrupt Flag
	delayFlag = 0;

	TIM2->CR1 &= ~(1<<CEN);
}

// IRQ for timeCalc function with Timer4.
void TIM4_IRQHandler(){
	TIM4->SR &= ~(1 << 0);
	overflowTimsTIM4++;
}

void TIM3_IRQHandler(){
	//	TIM3->SR &= ~(1 << 0);
	//	overflowTimsTIM3++;
	// Clear the update interrupt flag
	TIM3->SR &= ~(1 << 0);
	// Increment millis count
	millis_count++;
}









