/*
 * MAC_Header_Parser.h
 *
 *  Created on: 24 апр. 2016 г.
 *      Author: evgeniyMorozov
 */

#ifndef MAC_HEADER_PARSER_H_
#define MAC_HEADER_PARSER_H_

/**
 * @file MAC_Header_Parser.h
 * @brief This module allows to parse MAC header from 802.15.4 frame.
 */

#include <stdint.h>

#define MAC_FRAME_TYPE_BEACON 			(uint8_t)(0b00)
#define MAC_FRAME_TYPE_DATA 			(uint8_t)(0b01)
#define MAC_FRAME_TYPE_ACK				(uint8_t)(0b10)
#define MAC_FRAME_TYPE_CMD 				(uint8_t)(0b11)

#define MAC_ADDR_SHORT					(uint8_t)(0b10)
#define MAC_ADDR_LONG					(uint8_t)(0b11)

#define MAC_SRC_ADDR_PRESENTED(FC)		((FC).src_addr_mode)
#define MAC_DST_ADDR_PRESENTED(FC)		((FC).dst_addr_mode)

static const char MAC_FRAME_TYPES_STR[4][7] =
{ "BEACON", "DATA", "ACK", "CMD" };

static const char MAC_BOOL_STR[4][6] =
{ "False", "True" };

static const char MAC_ADRESSING_STR[4][6] =
{ "None", " ", "Short", "Long" };

typedef struct MAC_FrameControlFieldDef
{
	uint8_t frame_type :2;
	uint8_t security_enabled :1;
	uint8_t frame_pending :1;
	uint8_t ack_request :1;
	uint8_t panID_compression :1;
	uint8_t reserved :2;
	uint8_t dst_addr_mode :2;
	uint8_t frame_ver :2;
	uint8_t src_addr_mode :2;
} MAC_FrameControlField;

typedef union MAC_AddressUnion
{
	uint64_t addr_long;
	uint16_t addr_short;
} MAC_Address;

typedef struct MAC_HeaderDef
{
	MAC_Address dest_address;
	MAC_Address src_address;
	MAC_FrameControlField frame_control;
	uint16_t dest_panID;
	uint16_t src_panID;
	uint8_t sequence_number;
	uint8_t reserved;
} MAC_HeaderTypeDef;

void MAC_Parse_Header(MAC_HeaderTypeDef * mach, uint8_t frame[],
		uint8_t frame_length);

#endif /* MAC_HEADER_PARSER_H_ */
