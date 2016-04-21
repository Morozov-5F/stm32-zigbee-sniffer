/*
 * MRF24J40_Driver.c
 *
 *  Created on: 19 апр. 2016 г.
 *      Author: evgeniyMorozov
 */

#include "MRF24J40_Driver.h"

#include "diag/Trace.h"

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
	uint32_t i;
	for (i = 0; i < 100000; ++i);
//	MRF24J40_WriteLong(handle, MRF24J40_RFCON(0), 0x73);

	/*
	 MRF24J40_WriteShort(handle, MRF24J40_SADRL, 0x12);
	 MRF24J40_WriteShort(handle, MRF24J40_SADRH, 0x34);

	 MRF24J40_WriteShort(handle, MRF24J40_PANIDL, 0xFF);
	 MRF24J40_WriteShort(handle, MRF24J40_PANIDH, 0xFF);
	 uint8_t i;
	 for (i = 0; i < 8; ++i)
	 MRF24J40_WriteShort(handle, MRF24J40_EADR(i), 0x11);*/

	MRF24J40_WriteShort(handle, MRF24J40_RXMCR, 0x21);

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
		trace_printf("SPI IS FUCKED UP!\n");
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
		trace_printf("SPI IS FUCKED UP!\n");
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
		trace_printf("SPI IS FUCKED UP!\n");
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
		trace_printf("SPI IS FUCKED UP!\n");
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
		trace_printf("SPI IS SUPER FUCKED UP!\n");
		return MRF24J40_RESULT_ERR ;
	}
	return MRF24J40_RESULT_OK ;
}
