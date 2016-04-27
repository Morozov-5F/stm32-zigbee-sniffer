/*
 * MAC_Header_Parser.c
 *
 *  Created on: 24 апр. 2016 г.
 *      Author: evgeniyMorozov
 */
#include "MAC_Header_Parser.h"

#include <string.h>

void MAC_Parse_Header(MAC_HeaderTypeDef * mach, uint8_t frame[],
		uint8_t frame_length)
{
	/* Some basic error handling */
	if (!mach || !frame || frame_length < 2)
		return MAC_HEADER_RES_ERR;
	/* Position in frame */
	uint8_t pos = 0;
	/* First we parse frame control field like described in 802.15.4 data sheet */
	mach->frame_control.frame_type = frame[pos] & 0b11;
	mach->frame_control.security_enabled = (frame[pos] & 0b100) >> 2;
	mach->frame_control.frame_pending = (frame[pos] & 0b1000) >> 3;
	mach->frame_control.ack_request = (frame[pos] & 0b10000) >> 4;
	mach->frame_control.panID_compression = (frame[pos] & 0b100000) >> 5;
	mach->frame_control.dst_addr_mode = (frame[++pos] & 0b1100) >> 2;
	mach->frame_control.frame_ver = (frame[pos] & 0b110000) >> 4;
	mach->frame_control.src_addr_mode = (frame[pos] & 0b11000000) >> 6;
	/* Then we parse a sequence number */
	mach->sequence_number = frame[++pos];

	/* A bit more complicated - find out which MAC address or PAN ID is
	 * presented and store them in correct mach field. As described in
	 * data sheet - if MAC address is presented then PANID has to be presented
	 * too. For more information please refer to 802.15.4 data sheet  */
	if (MAC_DST_ADDR_PRESENTED(mach->frame_control))
	{
		/* If DST MAC is presented, we should read PAN ID first */
		if (memcpy(&mach->dest_panID, frame + (++pos), 2) != &mach->dest_panID)
		{
			return MAC_HEADER_RES_ERR;
		}
		/* If MAC is presented, then we should determine how long it is */
		uint8_t bytesToCopy = 2
				* (mach->frame_control.dst_addr_mode == MAC_ADDR_SHORT )
				+ 8 * (mach->frame_control.dst_addr_mode == MAC_ADDR_LONG );
		if (memcpy(&mach->dest_address, frame + (++pos), bytesToCopy)
				!= &mach->dest_address)
		{
			return MAC_HEADER_RES_ERR;
		}
	}
	/* The same thing for source address */
	if (MAC_SRC_ADDR_PRESENTED(mach->frame_control))
	{
		/* If PANID compression is enabled, then source PAN ID assumed to be
		 * the same as destination PAN ID */
		if (mach->frame_control.panID_compression)
			mach->src_panID = mach->dest_panID;
		else
		{
			if (memcpy(&mach->src_panID, frame + (++pos), 2)
					!= &mach->src_panID)
			{
				return MAC_HEADER_RES_ERR;
			}
		}
		uint8_t bytesToCopy = 2
				* (mach->frame_control.src_addr_mode == MAC_ADDR_SHORT )
				+ 8 * (mach->frame_control.src_addr_mode == MAC_ADDR_LONG );
		if (memcpy(&mach->src_address, frame + (++pos), bytesToCopy)
				!= &mach->src_address)
		{
			return MAC_HEADER_RES_ERR;
		}
	}
	return MAC_HEADER_RES_OK;
}
