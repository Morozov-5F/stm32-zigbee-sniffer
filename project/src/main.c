#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "MRF24J40_Driver.h"

/* ----- main() -------------------------------------------------------------*/

/* Sample pragmas to cope with warnings. Please note the related line at
 the end of this function, used to pop the compiler diagnostics status. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/* Global variables */
SPI_HandleTypeDef spih;
MRF24J40_HandleTypeDef mrf_handle;

uint8_t current_channel = 0;

int main(int argc, char* argv[])
{
	/* Configure SPI handle to work with radio module */
	mrf_handle.spi_handle.Instance = SPI1;
	mrf_handle.spi_handle.Init.Mode = SPI_MODE_MASTER;
	mrf_handle.spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
	mrf_handle.spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;
	mrf_handle.spi_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	mrf_handle.spi_handle.Init.CLKPolarity = SPI_PHASE_1EDGE;
	mrf_handle.spi_handle.Init.NSS = SPI_NSS_SOFT;
	mrf_handle.spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	mrf_handle.spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	mrf_handle.spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;
	mrf_handle.spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	mrf_handle.spi_handle.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&mrf_handle.spi_handle);

	/* Configure GPIO interrupt from radio and button */
	GPIO_InitTypeDef gpio_init;
	gpio_init.Mode = GPIO_MODE_IT_FALLING;
	gpio_init.Pin = GPIO_PIN_0 | GPIO_PIN_3;
	gpio_init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio_init);
	/* User button EXTI priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0x03, 0x00);
	/* Radio EXTI priority */
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0x02, 0x00);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	MRF24J40_InitializeChip(&mrf_handle);
	while (1)
	{

	}
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	/* Enable SPI1 interface clock */
	__HAL_RCC_SPI1_CLK_ENABLE()
	;
	/* Configure GPIO pins A4-A7 for SPI1 */
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	GPIO_InitTypeDef gpio_init;
	gpio_init.Alternate = GPIO_AF5_SPI1;
	gpio_init.Mode = GPIO_MODE_AF_PP;
	gpio_init.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;
	gpio_init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio_init);
	gpio_init.Alternate = 0x0;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pin = GPIO_PIN_4;
	gpio_init.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &gpio_init);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	/* Configure NVIC for SPI interrupts: priority and interrupt itself */
	HAL_NVIC_SetPriority(SPI1_IRQn, 0x01, 0x00);
	HAL_NVIC_EnableIRQ(SPI1_IRQn);
}

void SPI1_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&mrf_handle.spi_handle);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	uint8_t write_seq, *pTx = (hspi->pTxBuffPtr - hspi->TxXferSize);
	/* Long address case */
	if (*pTx & 0x80)
	{
		write_seq = (*(pTx + 1) & 0x10) >> 4;
	}
	else
	{
		write_seq = *(pTx) & 0x01;
	}

	if (write_seq)
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

/* MCU received something */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	uint8_t * pRx = hspi->pRxBuffPtr - hspi->RxXferSize;
	trace_printf("0x%x\n", *pRx);
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI3_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_0)
	{
		if (MRF24J40_SetChannel(&mrf_handle,
				((++current_channel % 0x10) + 11)) == MRF24J40_RESULT_OK)
			trace_printf("Current channel: %d\n",
					(current_channel % 0x10) + 11);

	}
	else
	{
		trace_printf("Received EXTI from radio\n");
	}
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi)
{
	trace_printf("Hmm, your SPI is fucked up!\n");
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
