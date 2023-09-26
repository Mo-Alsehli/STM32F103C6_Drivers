/*
 * STM32F103x6.h
 *
 *  Created on: Sep 23, 2023
 *      Author: Mohamed Magdi
 */

#ifndef INC_STM32F103X6_H_
#define INC_STM32F103X6_H_


// <---------------------------------------------- DEVICE HEADER ------------------------------------------------>


//---------------------------------------------------
//Includes
//---------------------------------------------------

#include <stdlib.h>
#include <stdint.h>

//---------------------------------------------------
//Base addresses for memories
//---------------------------------------------------

#define FLASH_MEM_BASE					0x08000000UL
#define SYS_MEM_BASE 					0x1FFFF000UL
#define SRAM_BASE 						0x20000000UL

#define PERIPHERALS_BASE 				0x40000000UL
#define CORTEXM3_PERIPHERAL_BASE 		0xE0000000UL

//---------------------------------------------------
//Base addresses for Bus AHB Peripherals.
//---------------------------------------------------

#define RCC_BASE						0x40021000UL	// OR (PERIPHERALS_BASE + 0x00021000UL)

//---------------------------------------------------
//Base addresses for Bus APB2 Peripherals.
//---------------------------------------------------
// fully included in LQFP48 package
#define GPIOA_BASE						0x40010800UL
#define GPIOB_BASE						0x40010C00UL
// partially included in LQFP48 package
#define GPIOC_BASE						0x40011000UL
#define GPIOD_BASE						0x40011400UL
// Not included in LQFP48 package
#define GPIOE_BASE						0x40011800UL


// EXTI
#define EXTI_BASE						0x40010400UL

// AFIO
#define AFIO_BASE						0x40010000UL

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Peripheral register:	GPIO
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

typedef struct {
	volatile uint32_t CRL;
	volatile uint32_t CRH;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t BRR;
	volatile uint32_t LCKR;

}GPIO_TYPE_DEF;

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Peripheral register:	RCC
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

typedef struct {
	volatile uint32_t CR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t APB1RSTR;
	volatile uint32_t AHBENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t APB1ENR;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t AHBSTR;
	volatile uint32_t CFGR2;
}RCC_TYPE_DEF;

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Peripheral register:	EXTI
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

typedef struct {
	volatile uint32_t IMR;
	volatile uint32_t EMR;
	volatile uint32_t RTSR;
	volatile uint32_t FTSR;
	volatile uint32_t SWIER;
	volatile uint32_t PR;
}EXTI_TYPE_DEF;

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Peripheral register:	AFIO
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

typedef struct {
	volatile uint32_t EVCR;
	volatile uint32_t MAPR;
	volatile uint32_t EXTICR1;
	volatile uint32_t EXTICR2;
	volatile uint32_t EXTICR3;
	volatile uint32_t EXTICR4;
	uint32_t 		  RESERVED0;
	volatile uint32_t MAPR2;
}AFIO_TYPE_DEF;

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Peripheral Instants:
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#define GPIOA			                ((GPIO_TYPE_DEF*) GPIOA_BASE)
#define GPIOB			                ((GPIO_TYPE_DEF*) GPIOB_BASE)
#define GPIOC			                ((GPIO_TYPE_DEF*) GPIOC_BASE)
#define GPIOD			                ((GPIO_TYPE_DEF*) GPIOD_BASE)
#define GPIOE			                ((GPIO_TYPE_DEF*) GPIOE_BASE)

#define RCC				                ((RCC_TYPE_DEF*)  RCC_BASE)

#define EXTI			                ((EXTI_TYPE_DEF*) EXTI_BASE)

#define AFIO			                ((AFIO_TYPE_DEF*) AFIO_BASE)

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Clock Enable Macros:
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#define RCC_GPIOA_CLk_EN()				(RCC->APB2ENR |= 1 << 2)
#define RCC_GPIOB_CLk_EN()				(RCC->APB2ENR |= 1 << 3)
#define RCC_GPIOC_CLk_EN()				(RCC->APB2ENR |= 1 << 4)
#define RCC_GPIOD_CLk_EN()				(RCC->APB2ENR |= 1 << 5)
#define RCC_GPIOE_CLk_EN()				(RCC->APB2ENR |= 1 << 6)

#define AFIO_CLK_EN()					(RCC->APB2ENR |= 1 << 0)


//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Generic Macros:
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*




#endif /* INC_STM32F103X6_H_ */
