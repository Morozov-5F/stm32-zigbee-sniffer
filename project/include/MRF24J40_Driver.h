/*
 * MRF24J40_Driver.h
 *
 *  Created on: 19 апр. 2016 г.
 *      Author: evgeniyMorozov
 */

#ifndef MRF24J40_DRIVER_H_
#define MRF24J40_DRIVER_H_

/**
 * @file MRF24J40_Driver.h
 * @brief This module allows to operate with MRF24J40 module, connected to MCU
 * via SPI interface. Module configures device to work in promiscuous mode in
 * order to receive all packets with correct CRC (you can gain more info about
 * CRC in 802.15.4 specification).
 */

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
#define MRF24J40_BBREG1			(MRF24J40_ShortAddr)(0x39)
#define MRF24J40_BBREG2			(MRF24J40_ShortAddr)(0x3A)
#define MRF24J40_BBREG6			(MRF24J40_ShortAddr)(0x3E)
#define MRF24J40_CCAEDTH		(MRF24J40_ShortAddr)(0x3F)

typedef uint16_t MRF24J40_LongAddr;
#define MRF24J40_RFCON(N)		(MRF24J40_LongAddr)(0x200 + (N))
#define MRF24J40_SLPCON1		(MRF24J40_LongAddr)(0x220)
#define MRF24J40_RXFIFO			(MRF24J40_LongAddr)(0x300)
#define MRF24J40_RXFIFO_DATA(N) (MRF24J40_LongAddr)(0x301 + (N))

#define MRF24J40_CHANNEL(CH)     (uint8_t)((((CH) - 11) & 0x0F) << 4)

typedef void (*MRF24J40_Callback)(void *);

#define MRF24J40_RSSI_CONVERT(VAL) (int8_t)(-(VAL)*0.22 + 90)

/**
 * MRF24J40 handle to operate with this radio module. It has HAL SPI handle
 * and various parameters of received frame.
 */
typedef struct MRF24J40_HandleDef
{
	/** HAL SPI handle in order to use SPI easily */
	SPI_HandleTypeDef spi_handle;
	MRF24J40_Callback callback;
	/** Frame, that was received */
	uint8_t recieved_frame[128];
	/** Request that was sent to device */
	uint8_t msg[3];
	/** Received frame length */
	uint8_t frame_length;
	/** Flag, that shows if MCU is busy reading RXFIFO */
	uint8_t is_receiving;
	/** Received interrupt bitmask */
	uint8_t intstat;
	/** Link quality indicator of received frame */
	uint8_t lqi;
	/** RSSI of received frame */
	uint8_t rssi;
} MRF24J40_HandleTypeDef;

/**
 * Creates MRF24J40 handle and sets up SPI hardware
 *
 * @param handle - pointer to the MRF24J40 handle
 * @param spi_td - pointer to the SPI typedef, specified in stm32f407x.h
 * @return error code
 */
MRF24J40_Result MRF24J40_CreateHandle(MRF24J40_HandleTypeDef * handle,
		SPI_TypeDef * spi_td);

/**
 * Sending initialization sequence to MRF24J40 device as described in data sheet.
 *
 * This function configures MRF24J40 to receive in promiscuous mode (for
 * additional information, please refer to the device data sheet).
 *
 * @param handle - pointer to the MRF24J40 handle
 * @return error code
 */
MRF24J40_Result MRF24J40_InitializeChip(MRF24J40_HandleTypeDef * handle);

/**
 * Sends sequence to specified MRF24J40 device, allowing to write value to
 * device's short address memory (addresses from 0x00 to 0x3F) at given
 * address.
 *
 * This function is non-blocking and requires SPI interrupt function to
 * indicate that transmission was completed.
 *
 * @param handle - pointer to the MRF24J40
 * @param addr - address at the device's short address memory
 * @param val - value to be written
 * @return error code
 */
MRF24J40_Result MRF24J40_WriteShort(MRF24J40_HandleTypeDef * handle,
		MRF24J40_ShortAddr addr, uint8_t val);

/**
 * Sends sequence to specified MRF24J40 device, allowing to read value of
 * device's short address memory (addresses from 0x0 to 0x3F) from given
 * address.
 *
 * This function is non-blocking and requires SPI interrupt function to
 * indicate that reception was completed.
 *
 * @param handle - pointer to the MRF24J40
 * @param addr - address at device's short address memory
 * @param val - pointer to variable, where received value will be stored
 * @return error code
 */
MRF24J40_Result MRF24J40_ReadShort(MRF24J40_HandleTypeDef * handle,
		MRF24J40_ShortAddr addr, uint8_t * val);
/**
 * Sends sequence to specified MRF24J40 device, allowing to write value to
 * device's long address memory (addresses from 0x220 to 0x38F) at given
 * address.
 *
 * This function is non-blocking and requires SPI interrupt function to
 * indicate that transmission was completed.
 *
 * @param handle - pointer to the MRF24J40
 * @param addr - address at the device's long address memory
 * @param val - value to be written
 * @return error code
 */
MRF24J40_Result MRF24J40_WriteLong(MRF24J40_HandleTypeDef * handle,
		MRF24J40_LongAddr addr, uint8_t val);

/**
 * Sends sequence to specified MRF24J40 device, allowing to read value of
 * device's long address memory (addresses from 0x220 to 0x38F) from given
 * address.
 *
 * This function is non-blocking and requires SPI interrupt function to
 * indicate that reception was completed.
 *
 * @param handle - pointer to the MRF24J40
 * @param addr - address at device's long address memory
 * @param val - pointer to variable, where received value will be stored
 * @return error code
 */
MRF24J40_Result MRF24J40_ReadLong(MRF24J40_HandleTypeDef * handle,
		MRF24J40_LongAddr addr, uint8_t * val);

/**
 * Sends sequence to specified MRF24J40 device, allowing to change device's
 * channel.
 *
 * @param handle - pointer to the MRF24J40
 * @param channel - value from 11 to 26, channel to be switched to
 * @return error code
 */
MRF24J40_Result MRF24J40_SetChannel(MRF24J40_HandleTypeDef * handle,
		uint8_t channel);
/**
 * Reads frame data from MRF24J40 RXFIFO register and stores it at given
 * handle's received_frame field.
 *
 * This function is non-blocking and requires properly configured
 * SPI interrupt function to perform successful reception.
 *
 * @param handle - pointer to the MRF24J40 handle
 * @return error code
 */
MRF24J40_Result MRF24J40_ReceiveFrame(MRF24J40_HandleTypeDef * handle);


#endif /* MRF24J40_DRIVER_H_ */
