/*
 * SPI.h
 *
 *  	Created : Apr 15, 2024
 *      Author:   Mohamed Magdi
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_

#include "STM32F103x6.h"
#include "GPIO_Driver.h"
#include "EXTI_Driver.h"

// Interrupt Source Struct
struct S_IRQ_SRC {
	uint8_t TXE:1; // Tx buffer empty interrupt
	uint8_t RXNE:1; // RX buffer not empty interrupt
	uint8_t ERRI:1; // Error Interrupt
	uint8_t Reserved:5;
};

// Configuration Structure
typedef struct {
	uint16_t deviceMode; // specifies the SPI operating mode @ref SPI_Device_Mode
	uint16_t communicationMode; // Specifies SPI bidirectional mode state @ref SPI_Communication_Mode.
	uint16_t frameFormat; // Specifies LSB or MSB @ref SPI_Frame_Format
	uint16_t dataSize; // @ref SPI_DataSize
	uint16_t CLKPolarity; // @ref SPI_CLKPoLarity
	uint16_t CLKPhase; // @ref SPI_CLKPhase
	uint16_t NSS; // Specifies whether the NSS signal is managed by hardware or by software using SII bitenable @ref SPI_NSS
	uint16_t SPIBaudratePrescaller; /* Specifies the baudrate prescaler value which will be used to configure the transmit and receive SCK clock.
									*  This parameter can be a value of @ref_SPI_BaudratePrescaller
									*  @note the communication clock is derived from the master clock.
									*  The slave clock does not need to be set.*/
	uint16_t IRQ_Enable;
	void (*P_IRQ_Callback)(struct S_IRQ_SRC irqSource);
}SPI_Config;

enum pollingMechanism {
	pollingDis,
	pollingEn
};


//------------------------------------------
// Macros Configuration References
//------------------------------------------
/*@ref SPI_Device_Mode
 */
#define SPI_Device_Mode_Slave				(0x00000000U)
#define SPI_Device_Mode_Master				(0x1U << 2) // CR1.MSTR = 1

/*@ref SPI_Communication_Mode
 */
#define SPI_Direction_2LINES				(0x00000000U)
#define SPI_Direction_2LINES_RXONLY			(0x1U << 10) // CR1 Bit 10 set
#define SPI_Direction_1LINES_RXONLY			(0x1U << 15)
#define SPI_Direction_1LINES_TXONLY			(0x1U << 15 | 0x1U << 14)

/*@ref SPI_Frame_Format
 */
#define SPI_Frame_Foramt_MSB				(0x00000000U)
#define SPI_Frame_Format_LSB				(0x1U << 7)

/*@ref Data_Size
 */
#define SPI_Data_Size_8Bit					(0x00000000U)
#define SPI_Data_Size_16Bit					(0x1U<<11)

/*@ref SPI_Clock_Polarity
 */
#define SPI_CLK_Polarity_Low_When_Idle		(0x0000000U)
#define SPI_CLK_Polarity_High_When_Idle		(0x1U << 1)

/*@ref SPI_CLK_Phase
 */
#define SPI_CLK_Phase_1Edge					(0x00000000U)
#define SPI_CLK_Phase_2Edge					(0x1U << 0)

/*@ref SPI_NSS
 */
// HW (master - slave) - CR2 Register Bits
#define SPI_NSS_HW_Slave					(0x00000000U)
#define SPI_NSS_HW_Master_SS_En				(0x1U << 2)
#define SPI_NSS_HW_Master_SS_Dis			(0x00000000U) // It's prefered to assign only 2nd bit to zero.

// SW - CR1 Register Bits
#define SPI_NSS_SW_Internal_Reset			(0x1U << 9)
#define SPI_NSS_SW_Internal_Set				((0x1U << 9) | (0x1U << 8))

/* @ref SPI_BaudRate_Prescaller
 */
#define SPI_BaudRate_Prescaller_2			(0x00000000U)
#define SPI_BaudRate_Prescaller_4			(0b001U << 3)
#define SPI_BaudRate_Prescaller_8			(0b010U << 3)
#define SPI_BaudRate_Prescaller_16			(0b011U << 3)
#define SPI_BaudRate_Prescaller_32			(0b100U << 3)
#define SPI_BaudRate_Prescaller_64			(0b101U << 3)
#define SPI_BaudRate_Prescaller_128			(0b110U << 3)
#define SPI_BaudRate_Prescaller_256			(0b111U << 3)


/*@ref SPI_IRQ_Enable
 */
#define SPI_IRQ_Enable_NONE					(uint32_t)(0)
#define SPI_IRQ_Enable_TXEIE				(uint32_t)(1 << 7)
#define SPI_IRQ_Enable_RXNEIE				(uint32_t)(1 << 6)
#define SPI_IRQ_Enable_ERRIE				(uint32_t)(1 << 5)



//------------------------------------------
// APIs Supported By "MCAL SPI DRIVER"
//------------------------------------------

void MCAL_SPI_Init(SPI_TYPE_DEF* spix, SPI_Config* spiCfg);
void MCAL_SPI_GPIO_SetPins(SPI_TYPE_DEF* spix);
void MCAL_SPI_DeInit(SPI_TYPE_DEF* spix);
void MCAL_SPI_SendData(SPI_TYPE_DEF* spix, uint16_t* pTxBuffer, enum pollingMechanism polling);
void MCAL_SPI_ReceiveData(SPI_TYPE_DEF* spix, uint16_t* pRxBuffer, enum pollingMechanism polling);
void MCAL_SPI_TX_RX(SPI_TYPE_DEF* spix, uint16_t* pBuffer, enum pollingMechanism polling);











#endif /* SPI_SPI_H_ */
