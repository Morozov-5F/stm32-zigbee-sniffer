#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "MRF24J40_Driver.h"
#include "MAC_Header_Parser.h"
#include "hd44780.h"

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

uint8_t current_channel = 3;
uint8_t start_recv = 0;

LCD_PCF8574_HandleTypeDef lcd;
int main(int argc, char* argv[])
{
	lcd.pcf8574.PCF_I2C_ADDRESS = 7;
	lcd.pcf8574.PCF_I2C_TIMEOUT = 1000;
	lcd.pcf8574.i2c.Instance = I2C1;
	lcd.pcf8574.i2c.Init.ClockSpeed = 400000;
	lcd.NUMBER_OF_LINES = NUMBER_OF_LINES_2;
	lcd.type = TYPE0;

	if (LCD_Init(&lcd) != LCD_OK)
	{
		while (1)
			;
	}

	LCD_ClearDisplay(&lcd);
	LCD_SetLocation(&lcd, 0, 0);
	LCD_WriteString(&lcd, "Channel:");
	LCD_SetLocation(&lcd, 8, 0);
	LCD_WriteNumber(&lcd, 11 + current_channel % 16, 10);
	/* Configure SPI handle to work with radio module */
	MRF24J40_CreateHandle(&mrf_handle, SPI1);

	/* Configure GPIO interrupt from radio and button */
	GPIO_InitTypeDef gpio_init;
	gpio_init.Mode = GPIO_MODE_IT_FALLING;
	gpio_init.Pin = GPIO_PIN_0 | GPIO_PIN_3;
	gpio_init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio_init);

	__HAL_RCC_GPIOD_CLK_ENABLE()
	;
	gpio_init.Speed = GPIO_SPEED_FAST;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	gpio_init.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOD, &gpio_init);

	/* User button EXTI priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0x03, 0x00);
	/* Radio EXTI priority */
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0x02, 0x00);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	MRF24J40_InitializeChip(&mrf_handle);
	while (1)
	{
		if (start_recv)
		{
			start_recv = 0;
			MRF24J40_ReceiveFrame(&mrf_handle);
		}
	}
}

void OutputChannel()
{
	LCD_ClearDisplay(&lcd);
	LCD_SetLocation(&lcd, 0, 0);
	LCD_WriteString(&lcd, "Channel:");
	LCD_SetLocation(&lcd, 8, 0);
	LCD_WriteNumber(&lcd, 11 + current_channel % 16, 10);
}

void OutputFrame()
{
	MAC_HeaderTypeDef mach;
	MAC_Parse_Header(&mach, mrf_handle.recieved_frame, mrf_handle.frame_length);

	LCD_SetLocation(&lcd, 11, 0);
	LCD_WriteString(&lcd, "RSSI:-");
	LCD_SetLocation(&lcd, 17, 0);
	LCD_WriteNumber(&lcd, MRF24J40_RSSI_CONVERT(mrf_handle.rssi), 10);

	LCD_SetLocation(&lcd, 0, 1);
	LCD_WriteString(&lcd, "FL:");
	LCD_SetLocation(&lcd, 3, 1);
	LCD_WriteString(&lcd, "   ");
	LCD_SetLocation(&lcd, 3, 1);
	LCD_WriteNumber(&lcd, mrf_handle.frame_length, 10);

	LCD_SetLocation(&lcd, 10, 1);
	LCD_WriteString(&lcd, "FT:");
	LCD_SetLocation(&lcd, 13, 1);
	LCD_WriteString(&lcd, "      ");
	LCD_SetLocation(&lcd, 13, 1);
	LCD_WriteString(&lcd, MAC_FRAME_TYPES_STR[mach.frame_control.frame_type]);

	LCD_SetLocation(&lcd, 0, 2);
	LCD_WriteString(&lcd, "DAM:");
	LCD_SetLocation(&lcd, 4, 2);
	LCD_WriteString(&lcd, "      ");
	LCD_SetLocation(&lcd, 4, 2);
	LCD_WriteString(&lcd, MAC_ADRESSING_STR[mach.frame_control.dst_addr_mode]);

	LCD_SetLocation(&lcd, 10, 2);
	LCD_WriteString(&lcd, "SAM:");
	LCD_SetLocation(&lcd, 14, 2);
	LCD_WriteString(&lcd, "      ");
	LCD_SetLocation(&lcd, 14, 2);
	LCD_WriteString(&lcd, MAC_ADRESSING_STR[mach.frame_control.src_addr_mode]);

	LCD_SetLocation(&lcd, 0, 3);
	LCD_WriteString(&lcd, "SN:");
	LCD_WriteString(&lcd, "   ");
	LCD_SetLocation(&lcd, 3, 3);
	LCD_WriteNumber(&lcd, mach.sequence_number, 10);

	LCD_SetLocation(&lcd, 10, 3);
	LCD_WriteString(&lcd, "FCS:");
	LCD_SetLocation(&lcd, 14, 3);
	LCD_WriteString(&lcd, "    ");
	LCD_SetLocation(&lcd, 14, 3);
	LCD_WriteNumber(&lcd,
			mrf_handle.recieved_frame[mrf_handle.frame_length - 2], 16);
	LCD_WriteNumber(&lcd,
			mrf_handle.recieved_frame[mrf_handle.frame_length - 1], 16);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if (hi2c->Instance == I2C1)
	{

		__GPIOB_CLK_ENABLE()
		;

		GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		__I2C1_CLK_ENABLE()
		;
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
	/*##-1- Reset peripherals ##################################################*/
	__I2C1_FORCE_RESET();
	__I2C1_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks ################################*/
	/* Configure I2C Tx as alternate function  */
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);
	/* Configure I2C Rx as alternate function  */
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
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
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	uint8_t * pRx = hspi->pRxBuffPtr - hspi->RxXferSize;
	if ((pRx == &mrf_handle.intstat) && (*pRx == 0x08))
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		start_recv = 1;
	}
	else if (pRx == &mrf_handle.rssi)
	{
		OutputFrame();
		mrf_handle.callback(&mrf_handle);
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	}

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
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
			OutputChannel();
	}
	else
	{
		MRF24J40_ReadShort(&mrf_handle, MRF24J40_INTSTAT, &mrf_handle.intstat);
		HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	}
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi)
{
	trace_printf("SPI is not configured properly!\n");

}

void SysTick_Handler(void)
{
	HAL_IncTick();
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
