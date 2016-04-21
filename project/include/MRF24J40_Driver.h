/*
 * MRF24J40_Driver.h
 *
 *  Created on: 19 апр. 2016 г.
 *      Author: evgeniyMorozov
 */

#ifndef MRF24J40_DRIVER_H_
#define MRF24J40_DRIVER_H_

#include "stm32f4xx_hal.h"

typedef uint8_t MRF24J40_Result;
#define MRF24J40_RESULT_OK		(uint8_t)(0x00)
#define MRF24J40_RESULT_ERR		(uint8_t)(0x01)

typedef uint8_t MRF24J40_ShortAddr;
#define MRF24J40_RXMCR			(MRF24J40_ShortAddr)(0x00)
#define MRF24J40_PANIDL			(MRF24J40_ShortAddr)(0x01)
#define MRF24J40_PANIDH			(MRF24J40_ShortAddr)(0x02)
#define MRF24J40_SADRL			(MRF24J40_ShortAddr)(0x03)
#define MRF24J40_SADRH			(MRF24J40_ShortAddr)(0x04)
#define MRF24J40_EADR(N)		(MRF24J40_ShortAddr)(0x05 + (N))
#define MRF24J40_PACON2			(MRF24J40_ShortAddr)(0x18)
#define MRF24J40_SOFTRST		(MRF24J40_ShortAddr)(0x2A)
#define MRF24J40_TXSTBL			(MRF24J40_ShortAddr)(0x2E)
#define MRF24J40_INTSTAT		(MRF24J40_ShortAddr)(0x31)
#define MRF24J40_INTCON			(MRF24J40_ShortAddr)(0x32)
#define MRF24J40_RFCTL			(MRF24J40_ShortAddr)(0x36)
#define MRF24J40_BBREG2			(MRF24J40_ShortAddr)(0x3A)
#define MRF24J40_BBREG6			(MRF24J40_ShortAddr)(0x3E)
#define MRF24J40_CCAEDTH		(MRF24J40_ShortAddr)(0x3F)

typedef uint16_t MRF24J40_LongAddr;
#define MRF24J40_RFCON(N)		(MRF24J40_LongAddr)(0x200 + (N))
#define MRF24J40_SLPCON1		(MRF24J40_LongAddr)(0x220)

#define MRF24J40_CHANNEL(CH)     (uint8_t)((((CH) - 11) & 0x0F) << 4)

typedef struct MRF24J40_HandleDef
{
	SPI_HandleTypeDef spi_handle;
	uint8_t msg[3];

} MRF24J40_HandleTypeDef;

MRF24J40_Result MRF24J40_InitializeChip(MRF24J40_HandleTypeDef * handle);

MRF24J40_Result MRF24J40_WriteShort(MRF24J40_HandleTypeDef * handle,
		MRF24J40_ShortAddr addr, uint8_t val);
MRF24J40_Result MRF24J40_ReadShort(MRF24J40_HandleTypeDef * handle,
		MRF24J40_ShortAddr addr, uint8_t * val);

MRF24J40_Result MRF24J40_WriteLong(MRF24J40_HandleTypeDef * handle,
		MRF24J40_LongAddr addr, uint8_t val);
MRF24J40_Result MRF24J40_ReadLong(MRF24J40_HandleTypeDef * handle,
		MRF24J40_LongAddr addr, uint8_t * val);

MRF24J40_Result MRF24J40_SetChannel(MRF24J40_HandleTypeDef * handle,
		uint8_t channel);

#endif /* MRF24J40_DRIVER_H_ */
