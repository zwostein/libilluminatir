/*
 * IlluminatIR
 * Copyright (C) 2021  zwostein
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * IlluminatIR Packets:
 * .----------------.----------------.----------------.
 * | Header         | Payload        | Checksum       |
 * | 1 Byte         | 2 to 17 Bytes  | 1 Byte         |
 * '----------------'----------------'----------------'
 * 	If the data stream cannot be packetized, Consistent Overhead Byte Stuffing (COBS) shall be used to synchronize data stream.
 *
 *
 * Header (1 byte):
 * .----------------------------------------------------------.
 * | Header                                                   |
 * |--------------.--------------.----------------------------|
 * |   0      1   |   2      3   |   4      5      6      7   |
 * |   Version    | PayloadType  |        PayloadSize         |
 * |--------------'--------------'----------------------------|
 * | 1 Byte                                                   |
 * '----------------------------------------------------------'
 * 	2 bit - Header version
 * 		0b00 - Default
 * 		0bxx - Ignored
 * 	2 bit - PayloadType
 * 		0b00 - Offset + Values
 * 		0b01 - Channel:Value pairs
 * 		0b10 - Configuration Key:Value pair
 * 		0bxx - Ignored
 * 	4 bit - PayloadSize
 * 		0..15 - 2 to 17 bytes of payload (PayloadSize-2)
 *
 *
 * Payload - Offset + Values (PayloadSize bytes, if PayloadType==0b00):
 * .--------------------------------------------------------.
 * | Payload - Offset + Values (if PayloadType==0b00)       |
 * |----------------.                                       |
 * | Offset         |----------------.-----.----------------|
 * |                | ChannelValue 0 |     | ChannelValue N |
 * | 1 Byte         | 1 Byte         | ... | 1 Byte         |
 * |----------------'----------------'-----'----------------|
 * | PayloadSize Bytes                                      |
 * '--------------------------------------------------------'
 * 	8 bit - Offset
 * 	Repeated N times:
 * 		8 bit - Channel (Offset+N) value (Channel numbers wrap around: 256==0)
 *
 *
 * Payload - Channel:Value pairs (PayloadSize bytes, if PayloadType==0b01):
 * .-------------------------------------------------------------------------.
 * | Payload - Channel:Value pairs (if PayloadType==0b01)                    |
 * |---------------------------------.-----.---------------------------------|
 * | Channel:Value pair 0            |     | Channel:Value pair N            |
 * |----------------.----------------|     |----------------.----------------|
 * | Channel        | Value          | ... | Channel        | Value          |
 * | 1 Byte         | 1 Byte         |     | 1 Byte         | 1 Byte         |
 * |----------------'----------------'-----'----------------'----------------|
 * | PayloadSize Bytes                                                       |
 * '-------------------------------------------------------------------------'
 * 	Repeated N times:
 * 		8 bit - Channel
 * 		8 bit - Value (if length was odd, defaults to 0)
 *
 *
 * Payload - Configuration Key:Value (PayloadSize bytes, if PayloadType==0b10):
 * .-------------------------------------------------------------------------.
 * | Payload - Configuration Key:Value (if PayloadType==0b10)                |
 * |---------------------------------.-----.---------------------------------|
 * | Configuration Key               |     | Value                           |
 * |----------------.----------------|     |----------------.----------------|
 * | Character      |                |  0  | Data           |                |
 * | 1 Byte         |      ...       |     | 1 Byte         |      ...       |
 * |----------------'----------------'-----'----------------'----------------|
 * | PayloadSize Bytes                                                       |
 * '-------------------------------------------------------------------------'
 * 	Repeated until null-terminator:
 * 		8 bit - Configuration Key Character
 * 	8 bit - 0 delimiter
 * 	Repeated N times:
 * 		8 bit - Value bytes
 *
 * 	Common Configuration Keys (Case insensitive)
 * 		"Base"       : Base channel (Might be overridden by hardware switches)
 * 			8 bit - The devices channels are offset by this number.
 * 		"MakeDefault": Make current values the default on power-up
 *
 *
 * Checksum (1 byte):
 * 	8 bit - CRC-8/KOOP (Header+Payload)
 */

#ifndef ILLUMINATIR_INCLUDED
#define ILLUMINATIR_INCLUDED

#include <stdint.h>
#include <stddef.h>


#define ILLUMINATIR_MIN_PACKET_SIZE 4
#define ILLUMINATIR_MAX_PACKET_SIZE 19

#define ILLUMINATIR_OFFSETARRAY_MIN_VALUES 1
#define ILLUMINATIR_OFFSETARRAY_MAX_VALUES 16


typedef enum {
	ILLUMINATIR_ERROR_NONE,
	ILLUMINATIR_ERROR_UNKNOWN,
	ILLUMINATIR_ERROR_NULL_POINTER,
	ILLUMINATIR_ERROR_BUFFER_OVERFLOW,
	ILLUMINATIR_ERROR_PACKET_TOO_SHORT,
	ILLUMINATIR_ERROR_PACKET_TOO_LONG,
	ILLUMINATIR_ERROR_UNSUPPORTED_VERSION,
	ILLUMINATIR_ERROR_UNSUPPORTED_FORMAT,
	ILLUMINATIR_ERROR_INVALID_SIZE,
	ILLUMINATIR_ERROR_INVALID_CRC,
} illuminatir_error_t;

#if defined(__AVR)
const char * illuminatir_error_toString_P( illuminatir_error_t err );
#else
const char * illuminatir_error_toString( illuminatir_error_t err );
#endif

typedef void (*illuminatir_parse_setChannel_t)( uint8_t channel, uint8_t value );
typedef void (*illuminatir_parse_setConfig_t)( const char * key, uint8_t key_size, const uint8_t * values, uint8_t values_size );
illuminatir_error_t illuminatir_parse( const uint8_t * packet, uint8_t packet_size, illuminatir_parse_setChannel_t setChannelFunc, illuminatir_parse_setConfig_t setConfigFunc );

illuminatir_error_t illuminatir_build_offsetArray( uint8_t * packet, uint8_t * packet_size, uint8_t offset, const uint8_t * values, uint8_t values_size );
illuminatir_error_t illuminatir_build_config     ( uint8_t * packet, uint8_t * packet_size, const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size );


////////////////////////////////
// CRC

#define ILLUMINATIR_CRC8_INITIAL_SEED (0)

uint8_t illuminatir_crc8( const uint8_t * data, size_t data_size, uint8_t crc );

////////////////////////////////


////////////////////////////////
// COBS encoding/decoding

#define ILLUMINATIR_COBS_ENCODE_DST_MAXSIZE(SRC_SIZE) ((SRC_SIZE)+(((SRC_SIZE)+253U)/254U))
#define ILLUMINATIR_COBS_DECODE_DST_MAXSIZE(SRC_SIZE) (((SRC_SIZE)==0) ? 0U : ((SRC_SIZE)-1U))

size_t illuminatir_cobs_encode( uint8_t * dst, size_t dst_size, const uint8_t * src, size_t src_size );
size_t illuminatir_cobs_decode( uint8_t * dst, size_t dst_size, const uint8_t * src, size_t src_size );
illuminatir_error_t illuminatir_cobs_parse( const uint8_t * cobsPacket, uint8_t cobsPacket_size, illuminatir_parse_setChannel_t setChannelFunc, illuminatir_parse_setConfig_t setConfigFunc );
illuminatir_error_t illuminatir_cobs_build_offsetArray( uint8_t * cobsPacket, uint8_t * cobsPacket_size, uint8_t offset, const uint8_t * values, uint8_t values_size );
illuminatir_error_t illuminatir_cobs_build_config     ( uint8_t * cobsPacket, uint8_t * cobsPacket_size, const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size );

////////////////////////////////


#endif
