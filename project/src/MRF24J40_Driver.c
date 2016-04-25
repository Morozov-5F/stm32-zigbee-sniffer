/*
 * MRF24J40_Driver.c
 *
 *  Created on: 19 апр. 2016 г.
 *      Author: evgeniyMorozov
 */

#include "MRF24J40_Driver.h"

#include "diag/Trace.h"
#include "MAC_Header_Parser.h"

void MRF24J40_RecvCallback(void * handle);

MRF24J40_Result MRF24J40_CreateHandle(MRF24J40_HandleTypeDef * handle,
		SPI_TypeDef * spi_td)
{
	if (spi_td != SPI1 || !handle)
		return MRF24J40_RESULT_ERR ;

	handle->spi_handle.Instance = spi_td;
	handle->spi_handle.Init.Mode = SPI_MODE_MASTER;
	handle->spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
	handle->spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;
	handle->spi_handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	handle->spi_handle.Init.CLKPolarity = SPI_PHASE_1EDGE;
	handle->spi_handle.Init.NSS = SPI_NSS_SOFT;
	handle->spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	handle->spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	handle->spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;
	handle->spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	handle->spi_handle.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&handle->spi_handle);

	return MRF24J40_RESULT_OK ;
}

MRF24J40_Result MRF24J40_InitializeChip(MRF24J40_HandleTypeDef * handle)
{
	MRF24J40_WriteShort(handle, MRF24J40_SOFTRST, 0x07);
	MRF24J40_WriteShort(handle, MRF24J40_PACON2, 0x98);
	MRF24J40_WriteShort(handle, MRF24J40_TXSTBL, 0x95);

	MRF24J40_WriteLong(handle, MRF24J40_RFCON(0), 0x03);
	MRF24J40_WriteLong(handle, MRF24J40_RFCON(1), 0x01);
	MRF24J40_WriteLong(handle, MRF24J40_RFCON(2), 0x80);
	MRF24J40_WriteLong(handle, MRF24J40_RFCON(6), 0x90);
	MRF24J40_WriteLong(handle, MRF24J40_RFCON(7), 0x80);
	MRF24J40_WriteLong(handle, MRF24J40_RFCON(8), 0x10);
	MRF24J40_WriteLong(handle, MRF24J40_SLPCON1, 0x21);

	MRF24J40_WriteShort(handle, MRF24J40_BBREG2, 0x80);
	MRF24J40_WriteShort(handle, MRF24J40_CCAEDTH, 0x60);
	MRF24J40_WriteShort(handle, MRF24J40_BBREG6, 0x40);

	MRF24J40_WriteShort(handle, MRF24J40_INTCON, ~(0x08));
	MRF24J40_WriteShort(handle, MRF24J40_RFCTL, 0x04);
	MRF24J40_WriteShort(handle, MRF24J40_RFCTL, 0x00);

	HAL_Delay(10);

	MRF24J40_WriteShort(handle, MRF24J40_RXMCR, 0x21);
	MRF24J40_SetChannel(handle, 14);

	return MRF24J40_RESULT_OK ;
}

MRF24J40_Result MRF24J40_WriteShort(MRF24J40_HandleTypeDef * handle,
		MRF24J40_ShortAddr addr, uint8_t val)
{
	handle->msg[0] = ((addr & 0x3F) << 1) | 0x1;
	handle->msg[1] = val;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit_IT(&handle->spi_handle, handle->msg, 0x02) != HAL_OK)
	{
		trace_printf("SPI is not configured properly!\n");
		return MRF24J40_RESULT_ERR ;
	}
	return MRF24J40_RESULT_OK ;
}

MRF24J40_Result MRF24J40_ReadShort(MRF24J40_HandleTypeDef * handle,
		MRF24J40_ShortAddr addr, uint8_t * val)
{
	handle->msg[0] = (addr << 1) & 0x7E;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit_IT(&handle->spi_handle, handle->msg, 0x01) != HAL_OK
			|| HAL_SPI_Receive_IT(&handle->spi_handle, val, 0x01) != HAL_OK)
	{
		trace_printf("SPI is not configured properly!\n");
		return MRF24J40_RESULT_ERR ;
	}
	return MRF24J40_RESULT_OK ;
}

MRF24J40_Result MRF24J40_WriteLong(MRF24J40_HandleTypeDef * handle,
		MRF24J40_LongAddr addr, uint8_t val)
{
	addr &= 0x3FF;
	handle->msg[0] = ((addr >> 3) & 0xFF) | 0x80;
	handle->msg[1] = ((addr & 0x07) << 5 | 0x10) & 0xF0;
	handle->msg[2] = val;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit_IT(&handle->spi_handle, handle->msg, 0x03) != HAL_OK)
	{
		trace_printf("SPI is not configured properly!\n");
		return MRF24J40_RESULT_ERR ;
	}
	return MRF24J40_RESULT_OK ;
}

MRF24J40_Result MRF24J40_ReadLong(MRF24J40_HandleTypeDef * handle,
		MRF24J40_LongAddr addr, uint8_t * val)
{
	addr &= 0x3FF;
	handle->msg[0] = ((addr >> 3) & 0xFF) | 0x80;
	handle->msg[1] = ((addr & 0x07) << 5) & 0xE0;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	if (HAL_SPI_Transmit_IT(&handle->spi_handle, handle->msg, 0x02) != HAL_OK
			|| HAL_SPI_Receive_IT(&handle->spi_handle, val, 0x01) != HAL_OK)
	{
		trace_printf("SPI is not configured properly!\n");
		return MRF24J40_RESULT_ERR ;
	}
	return MRF24J40_RESULT_OK ;
}

MRF24J40_Result MRF24J40_SetChannel(MRF24J40_HandleTypeDef * handle,
		uint8_t channel)
{
	channel = MRF24J40_CHANNEL(channel) | 0x03;
	if ((MRF24J40_WriteLong(handle, MRF24J40_RFCON(0), channel)
			|| MRF24J40_WriteShort(handle, MRF24J40_RFCTL, 0x04)
			|| MRF24J40_WriteShort(handle, MRF24J40_RFCTL, 0x00))
			!= MRF24J40_RESULT_OK)
	{
		trace_printf("SPI is not configured properly!\n");
		return MRF24J40_RESULT_ERR ;
	}
	return MRF24J40_RESULT_OK ;
}

MRF24J40_Result MRF24J40_ReceiveFrame(MRF24J40_HandleTypeDef * handle)
{
	uint8_t i;
	handle->is_receiving = 1;
	handle->frame_length = 0xFF;
	handle->callback = &MRF24J40_RecvCallback;
	MRF24J40_WriteShort(handle, MRF24J40_BBREG1, 0x04);
	MRF24J40_ReadLong(handle, MRF24J40_RXFIFO, &handle->frame_length);

	/* TODO: Remove lock */
	while (handle->frame_length == 0xff)
		;

	for (i = 0; i < handle->frame_length; ++i)
	{
		if (MRF24J40_ReadLong(handle, MRF24J40_RXFIFO_DATA(i),
				&(handle->recieved_frame[i])) != MRF24J40_RESULT_OK)
			return MRF24J40_RESULT_ERR ;
	}
	MRF24J40_ReadLong(handle, MRF24J40_RXFIFO_DATA(handle->frame_length),
			&handle->lqi);
	MRF24J40_ReadLong(handle, MRF24J40_RXFIFO_DATA(handle->frame_length + 1),
			&handle->rssi);

	MRF24J40_WriteShort(handle, MRF24J40_BBREG1, 0x00);
	return MRF24J40_RESULT_OK ;
}

void MRF24J40_RecvCallback(void * handle)
{
	MRF24J40_HandleTypeDef * mrfh = (MRF24J40_HandleTypeDef *) handle;
	mrfh->is_receiving = 0;
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
