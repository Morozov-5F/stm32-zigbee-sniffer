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
	if (!mach || !frame || !frame_length)
		return;
	uint8_t pos = 0;
	mach->frame_control.frame_type = frame[pos] & 0b11;
	mach->frame_control.security_enabled = (frame[pos] & 0b100) >> 2;
	mach->frame_control.frame_pending = (frame[pos] & 0b1000) >> 3;
	mach->frame_control.ack_request = (frame[pos] & 0b10000) >> 4;
	mach->frame_control.panID_compression = (frame[pos] & 0b100000) >> 5;
	mach->frame_control.dst_addr_mode = (frame[++pos] & 0b1100) >> 2;
	mach->frame_control.frame_ver = (frame[pos] & 0b110000) >> 4;
	mach->frame_control.src_addr_mode = (frame[pos] & 0b11000000) >> 6;

	mach->sequence_number = frame[++pos];

	if (MAC_DST_ADDR_PRESENTED(mach->frame_control))
	{
		if (memcpy(&mach->dest_panID, frame + (++pos), 2) != &mach->dest_panID)
		{
			return;
		}
		uint8_t bytesToCopy = 2
				* (mach->frame_control.dst_addr_mode == MAC_ADDR_SHORT )
				+ 8 * (mach->frame_control.dst_addr_mode == MAC_ADDR_LONG );
		if (memcpy(&mach->dest_address, frame + (++pos), bytesToCopy)
				!= &mach->dest_address)
		{
			return;
		}
	}
	if (MAC_SRC_ADDR_PRESENTED(mach->frame_control))
	{
		if (mach->frame_control.panID_compression)
			mach->src_panID = mach->dest_panID;
		else
		{
			if (memcpy(&mach->src_panID, frame + (++pos), 2)
					!= &mach->src_panID)
			{
				return;
			}
		}
		uint8_t bytesToCopy = 2
				* (mach->frame_control.src_addr_mode == MAC_ADDR_SHORT )
				+ 8 * (mach->frame_control.src_addr_mode == MAC_ADDR_LONG );
		if (memcpy(&mach->src_address, frame + (++pos), bytesToCopy)
				!= &mach->src_address)
		{
			return;
		}
	}
}
