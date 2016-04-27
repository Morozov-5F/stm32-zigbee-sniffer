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

/** @brief Function exit status */
typedef uint8_t MAC_Header_Result;

/** @brief Function exited without any errors */
#define MAC_HEADER_RES_OK             (MAC_Header_Result)(0x00)
/** @brief Function exited with error */
#define MAC_HEADER_RES_ERR            (MAC_Header_Result)(0x01)

/** @brief MAC Beacon frame type */
#define MAC_FRAME_TYPE_BEACON         (uint8_t)(0b00)
/** @brief MAC Data frame type */
#define MAC_FRAME_TYPE_DATA           (uint8_t)(0b01)
/** @brief MAC ACK frame type */
#define MAC_FRAME_TYPE_ACK            (uint8_t)(0b10)
/** @brief MAC Command frame type */
#define MAC_FRAME_TYPE_CMD            (uint8_t)(0b11)

/** @brief MAC 16-bit adress mode */
#define MAC_ADDR_SHORT                (uint8_t)(0b10)
/** @brief MAC 64-bit adress mode */
#define MAC_ADDR_LONG                 (uint8_t)(0b11)

/** @brief Determmines if source address is presented in MAC header */
#define MAC_SRC_ADDR_PRESENTED(FC)    ((FC).src_addr_mode)
/** @brief Determmines if destination address is presented in MAC header */
#define MAC_DST_ADDR_PRESENTED(FC)    ((FC).dst_addr_mode)

static const char MAC_FRAME_TYPES_STR[4][7] =
{ "BEACON", "DATA", "ACK", "CMD" };

static const char MAC_BOOL_STR[4][6] =
{ "False", "True" };

static const char MAC_ADRESSING_STR[4][6] =
{ "None", " ", "Short", "Long" };

/**
 * @brief Parsed MAC Header Frame control field of 802.15.4
 * packet. For more information about field please refer to 802.15.4.
 * data sheet.
 */
typedef struct MAC_FrameControlFieldDef
{
  /** @brief MAC frame type */
  uint8_t frame_type :2;
  /** @brief MAC security enabled flag */
  uint8_t security_enabled :1;
  /** @brief MAC frame pending flag */
  uint8_t frame_pending :1;
  /** @brief MAC ACK request flag */
  uint8_t ack_request :1;
  /** @brief MAC PAN ID comperssion flag */
  uint8_t panID_compression :1;
  /** @brief Reserved bits in MAC FCF */
  uint8_t reserved :2
  /** @brief Destination adressing mode (DAM) */
  uint8_t dst_addr_mode :2;
  /** @brief MAC Frame version */
  uint8_t frame_ver :2;
  /** @brief Source adressing mode */
  uint8_t src_addr_mode :2;
} MAC_FrameControlField;

/**
 * @brief Holds short (16-bit) or long (64-bit) MAC address
 * presented in MAC Header.
 */
typedef union MAC_AddressUnion
{
  /** @brief Long (64-bit) MAC address */
  uint64_t addr_long;
  /** @brief Short (16-bit) MAC address */
  uint16_t addr_short;
} MAC_Address;

/**
 * @brief Parsed data of MAC Header in 802.15.4 packet. Security
 * fields are not included. For more information about this field please refer
 * to 802.15.4 data sheet.
 */
typedef struct MAC_HeaderDef
{
  /** @brief Destination MAC address */
  MAC_Address dest_address;
  /** @brief Source MAC address */
  MAC_Address src_address;
  /** @brief MAC Header frame control field */
  MAC_FrameControlField frame_control;
  /** @brief Destination PAN ID */
  uint16_t dest_panID;
  /** @brief Source PAN ID */
  uint16_t src_panID;
  /** @brief MAC Sequence number */
  uint8_t sequence_number;
   /** @brief Reserved byte for alignment */
  uint8_t reserved;
} MAC_HeaderTypeDef;

/**
 * @brief Parses MAC Header of given frame or packet and stores parsed data in
 * given MAC_HeaderTypeDef.
 *
 * @param mach - a pointer to MAC Header to store parsed header data
 * @param frame - packet or frame to parse
 * @param frame_length - frame or packet length
 *
 * @return error code
 */
MAC_Header_Result MAC_Parse_Header(MAC_HeaderTypeDef * mach, uint8_t frame[],
    uint8_t frame_length);

#endif /* MAC_HEADER_PARSER_H_ */
