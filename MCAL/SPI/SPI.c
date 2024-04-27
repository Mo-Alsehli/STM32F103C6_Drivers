/*
 * SPI.c
 *
 *  	Created: Apr 15, 2024
 *      Author: Mohamed Magdi
 */

#include "SPI.h"

// ------------------------------------------------------------------
// Generic Variables
// ------------------------------------------------------------------
SPI_Config* gSPIConfig[2] = {NULL, NULL};


// ------------------------------------------------------------------
// Generic Macros
// ------------------------------------------------------------------

#define SPI1_INDEX			0
#define SPI2_INDEX			1

#define SPI_SR_TXE 			((uint8_t)0x02) // Transmit buffer empty.
#define SPI_SR_RXNE			((uint8_t)0x01) // Recieve buffer not empty.

// ------------------------------------------------------------------
// APIs
// ------------------------------------------------------------------

/*
 * Supports for SPI Full Duplex Master/Slave only & NSS HW/SW.
 * In Case of Master you have to configure pin and drive it.
 */
void MCAL_SPI_Init(SPI_TYPE_DEF* spix, SPI_Config* spiCfg){
	/*
	 * Safty for Registers.
	 * So that we insure that We make the configuration after enabling the SPI
	 */
	uint16_t tmpRegCR1 = 0;
	uint16_t tmpRegCR2 = 0;

	if(spix == SPI1){
		gSPIConfig[SPI1_INDEX] = spiCfg;
		RCC_SPI1_CLK_EN();
	}else if(spix == SPI2){
		gSPIConfig[SPI2_INDEX] = spiCfg;
		RCC_SPI2_CLK_EN();
	}

	// Enable SPI CR1: Bit 6 SPE
	tmpRegCR1 = (0x1U << 6);

	// Master || Slave
	tmpRegCR1 |= spiCfg -> deviceMode;

	// Communication Mode
	tmpRegCR1 |= spiCfg -> communicationMode;

	// Frame Format
	tmpRegCR1 |= spiCfg -> frameFormat;

	// Data Size
	tmpRegCR1 |= spiCfg -> dataSize;

	// clock polarity
	tmpRegCR1 |= spiCfg -> CLKPolarity;

	// Clock Phase
	tmpRegCR1 |= spiCfg -> CLKPhase;

	// --------- NSS -----------

	if(spiCfg ->NSS == SPI_NSS_HW_Master_SS_En){
		tmpRegCR2  |= spiCfg -> NSS;
	}else if(spiCfg ->NSS == SPI_NSS_HW_Master_SS_En){
		tmpRegCR2  &= spiCfg -> NSS;
	}else{
		tmpRegCR1 |= spiCfg -> NSS;
	}

	// Prescaller
	tmpRegCR1 |= spiCfg -> SPIBaudratePrescaller;

	// Interrupt Configuration
	if(spiCfg -> IRQ_Enable != SPI_IRQ_Enable_NONE){
		tmpRegCR2 |= spiCfg -> IRQ_Enable;

		if(spix == SPI1){
			NVIC_IRQ35_SPI1_Enable;
		}else if(spix == SPI2){
			NVIC_IRQ36_SPI2_Enable;
		}
	}

	spix->CR1 = tmpRegCR1;
	spix->CR2 = tmpRegCR2;
}



void MCAL_SPI_DeInit(SPI_TYPE_DEF* spix){
	if(spix == SPI1){
		NVIC_IRQ35_SPI1_Disable;
		RCC_SPI1_CLK_Dis();
	}else if(spix == SPI2){
		NVIC_IRQ36_SPI2_Disable;
		RCC_SPI2_CLK_Dis();
	}
}


void MCAL_SPI_SendData(SPI_TYPE_DEF* spix, uint16_t* pTxBuffer, enum pollingMechanism polling){
	if(polling == pollingEn)
		while(!(spix->SR & SPI_SR_TXE));

	spix->DR = *pTxBuffer;

}



void MCAL_SPI_ReceiveData(SPI_TYPE_DEF* spix, uint16_t* pRxBuffer, enum pollingMechanism polling){
	if(polling == pollingEn)
		while(!(spix->SR & SPI_SR_RXNE));

	*pRxBuffer = spix->DR;
}



void MCAL_SPI_TX_RX(SPI_TYPE_DEF* spix, uint16_t* pBuffer, enum pollingMechanism polling){
	if(polling == pollingEn)
		while(!(spix->SR & SPI_SR_TXE));
	spix->DR = *pBuffer;

	if(polling == pollingEn)
		while(!(spix->SR & SPI_SR_RXNE));
	*pBuffer = spix->DR;
}

void MCAL_SPI_GPIO_SetPins(SPI_TYPE_DEF* spix){
	GPIO_PinConfig_t gpioPinCfg;

	if(spix == SPI1){
		// PA4: NSS, PA5: SCK, PA6: MISO, PA7: MOSI
		if(gSPIConfig[SPI1_INDEX] -> deviceMode == SPI_Device_Mode_Master){
			// NSS configuration
			switch(gSPIConfig[SPI1_INDEX]->NSS){
			case SPI_NSS_HW_Master_SS_Dis:
				gpioPinCfg.GPIO_PinNumber = GPIO_PIN_4;
				gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_AF;
				MCAL_GPIO_Init(GPIOA, &gpioPinCfg);
				break;
			case SPI_NSS_HW_Master_SS_En:
				gpioPinCfg.GPIO_PinNumber = GPIO_PIN_4;
				gpioPinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
				gpioPinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
				MCAL_GPIO_Init(GPIOA, &gpioPinCfg);
				break;
			}

			// clock pin configuration
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_5;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			gpioPinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOA, &gpioPinCfg);

			// MISO configuration (Supported only fully dublex)
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_6;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOA, &gpioPinCfg);

			// MOSI configuration
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_7;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			gpioPinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOA, &gpioPinCfg);

		}else { // Slave
			// NSS configuration
			if(gSPIConfig[SPI1_INDEX] == SPI_NSS_HW_Slave){
				gpioPinCfg.GPIO_PinNumber = GPIO_PIN_4;
				gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
				MCAL_GPIO_Init(GPIOA, &gpioPinCfg);
			}

			// clock pin configuration
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_5;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOA, &gpioPinCfg);


			// MISO configuration (Supported only fully dublex)
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_6;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			gpioPinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOA, &gpioPinCfg);

			// MOSI configuration
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_7;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOA, &gpioPinCfg);

		}
	}else if(spix == SPI2){
		// PB12: NSS, PB13: SCK, PB14: MISO, PB15: MOSI
		if(gSPIConfig[SPI2_INDEX] -> deviceMode == SPI_Device_Mode_Master){
			// NSS configuration
			switch(gSPIConfig[SPI2_INDEX]->NSS){
			case SPI_NSS_HW_Master_SS_Dis:
				gpioPinCfg.GPIO_PinNumber = GPIO_PIN_12;
				gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_AF;
				MCAL_GPIO_Init(GPIOB, &gpioPinCfg);
				break;
			case SPI_NSS_HW_Master_SS_En:
				gpioPinCfg.GPIO_PinNumber = GPIO_PIN_12;
				gpioPinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
				gpioPinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
				MCAL_GPIO_Init(GPIOB, &gpioPinCfg);
				break;
			}

			// clock pin configuration
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_13;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			gpioPinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOB, &gpioPinCfg);

			// MISO configuration (Supported only fully dublex)
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_14;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOB, &gpioPinCfg);

			// MOSI configuration
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_15;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			gpioPinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOB, &gpioPinCfg);

		}else { // Slave
			// NSS configuration
			if(gSPIConfig[SPI2_INDEX] == SPI_NSS_HW_Slave){
				gpioPinCfg.GPIO_PinNumber = GPIO_PIN_12;
				gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
				MCAL_GPIO_Init(GPIOB, &gpioPinCfg);
			}

			// clock pin configuration
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_13;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOB, &gpioPinCfg);


			// MISO configuration (Supported only fully dublex)
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_14;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			gpioPinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOB, &gpioPinCfg);

			// MOSI configuration
			gpioPinCfg.GPIO_PinNumber = GPIO_PIN_15;
			gpioPinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOB, &gpioPinCfg);
		}
	}
}




	// ------------------------------------------------------------------
	// IRQs
	// ------------------------------------------------------------------

	void SPI1_IRQHandler() {
		struct S_IRQ_SRC irqSrc;
		irqSrc.TXE = ((SPI1->SR & ( 1 << 1)) >> 1);
		irqSrc.RXNE = ((SPI1->SR & ( 1 << 0)) >> 0);
		irqSrc.ERRI = ((SPI1->SR & ( 1 << 4)) >> 4);
		gSPIConfig[SPI1_INDEX]->P_IRQ_Callback(irqSrc);
	}

	void SPI2_IRQHandler() {
		struct S_IRQ_SRC irqSrc;
		irqSrc.TXE = ((SPI2->SR & ( 1 << 1)) >> 1);
		irqSrc.RXNE = ((SPI2->SR & ( 1 << 0)) >> 0);
		irqSrc.ERRI = ((SPI2->SR & ( 1 << 4)) >> 4);
		gSPIConfig[SPI2_INDEX]->P_IRQ_Callback(irqSrc);
	}




