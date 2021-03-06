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

#include "illuminatir.h"

#include <stdint.h>
#include <string.h>


#if defined(__AVR)
#	include <avr/pgmspace.h>
const char * illuminatir_error_toString_P( illuminatir_error_t err )
#else
#	define PSTR
const char * illuminatir_error_toString( illuminatir_error_t err )
#endif
{
	switch( err ) {
		case ILLUMINATIR_ERROR_NONE:                return PSTR("None");
		case ILLUMINATIR_ERROR_UNKNOWN:             return PSTR("Unknown");
		case ILLUMINATIR_ERROR_NULL_POINTER:        return PSTR("Null Pointer");
		case ILLUMINATIR_ERROR_BUFFER_OVERFLOW:     return PSTR("Buffer overflow");
		case ILLUMINATIR_ERROR_PACKET_TOO_SHORT:    return PSTR("Packet too short to be valid");
		case ILLUMINATIR_ERROR_PACKET_TOO_LONG:     return PSTR("Packet too long");
		case ILLUMINATIR_ERROR_UNSUPPORTED_VERSION: return PSTR("Unsupported version");
		case ILLUMINATIR_ERROR_UNSUPPORTED_FORMAT:  return PSTR("Unsupported format");
		case ILLUMINATIR_ERROR_INVALID_SIZE:        return PSTR("Invalid size");
		case ILLUMINATIR_ERROR_INVALID_CRC:         return PSTR("Invalid CRC");
	}
	return NULL;
}


illuminatir_error_t illuminatir_parse( const uint8_t * packets, uint8_t packets_size, illuminatir_parse_setChannel_t setChannelFunc, illuminatir_parse_setConfig_t setConfigFunc )
{
	if( !packets ) {
		return ILLUMINATIR_ERROR_NULL_POINTER;
	}
	while( packets_size ) {
		if( packets_size < ILLUMINATIR_PACKET_MINSIZE ) {
			return ILLUMINATIR_ERROR_PACKET_TOO_SHORT;
		}
		uint8_t version = packets[0] >> 6;
		if( version != 0 ) {
			return ILLUMINATIR_ERROR_UNSUPPORTED_VERSION;
		}
		uint8_t format = (packets[0] & 0b00110000) >> 4;
		uint8_t payload_size = (packets[0] & 0b00001111) + 2;
		uint8_t packet_size = 1 + payload_size + 1;
		if( packet_size > packets_size ) {
			return ILLUMINATIR_ERROR_INVALID_SIZE;
		}
		const uint8_t * payload = packets + 1;
		uint8_t crc_received = packets[packet_size - 1];
		uint8_t crc_calculated = illuminatir_crc8( packets, packet_size - 1, ILLUMINATIR_CRC8_INITIAL_SEED );
		if( crc_received != crc_calculated ) {
			return ILLUMINATIR_ERROR_INVALID_CRC;
		}
		
		switch( format ) {
			case 0: { // OffsetArray - offset + values
				if( !setChannelFunc ) {
					break;
				}
				uint8_t offset = *payload++;
				for( uint8_t i = 0; i < payload_size - 1; i++ ) {
					uint8_t channel = i+offset;
					setChannelFunc( channel, *payload++ );
				}
				break;
			}
			case 1: { // ChannelValuePairs
				if( !setChannelFunc ) {
					break;
				}
				uint8_t pairs = payload_size / 2;
				uint8_t halfpair = payload_size % 2;
				while( pairs ) {
					uint8_t channel = *payload++;
					uint8_t value = *payload++;
					setChannelFunc( channel, value );
					pairs--;
				}
				if( halfpair ) {
					uint8_t channel = *payload++;
					setChannelFunc( channel, 0 );
				}
				break;
			}
			case 2: { // Config - key/value pair
				if( !setConfigFunc ) {
					break;
				}
				const char * key = (const char *)payload;
				uint8_t key_len = strnlen( key, payload_size );
				uint8_t values_size = payload_size - key_len;
				if( values_size > 0 ) {
					values_size--;
				}
				const uint8_t * values = payload + payload_size - values_size;
				setConfigFunc( key, key_len, values, values_size );
				break;
			}
			default: {
				return ILLUMINATIR_ERROR_UNSUPPORTED_FORMAT;
			}
		}
		packets += packet_size;
		packets_size -= packet_size;
	}
	return ILLUMINATIR_ERROR_NONE;
}


illuminatir_error_t illuminatir_build_offsetArray( uint8_t * packet, uint8_t * packet_size, uint8_t offset, const uint8_t * values, uint8_t values_size )
{
	if( !packet_size ) {
		return ILLUMINATIR_ERROR_NULL_POINTER;
	}
	if( values_size < ILLUMINATIR_OFFSETARRAY_MINVALUES ) {
		return ILLUMINATIR_ERROR_INVALID_SIZE;
	}
	if( values_size > ILLUMINATIR_OFFSETARRAY_MAXVALUES ) {
		return ILLUMINATIR_ERROR_INVALID_SIZE;
	}
	uint8_t packet_size_available = *packet_size;
	*packet_size = 1 + 1 + values_size + 1;
	if( packet_size_available < *packet_size ) {
		return ILLUMINATIR_ERROR_BUFFER_OVERFLOW;
	}
	uint8_t * p = packet;
	*p++ = 0
	     | (values_size - 1)
	     ;
	*p++ = offset;
	while( values_size-- ) {
		*p++ = *values++;
	}
	*p++ = illuminatir_crc8( packet, (*packet_size) - 1, ILLUMINATIR_CRC8_INITIAL_SEED );
	return ILLUMINATIR_ERROR_NONE;
}


illuminatir_error_t illuminatir_build_config( uint8_t * packet, uint8_t * packet_size, const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size )
{
	if( !packet_size ) {
		return ILLUMINATIR_ERROR_NULL_POINTER;
	}
	uint8_t payload_size = key_len + 1 + values_size;
	if( payload_size < 2 ) {
		return ILLUMINATIR_ERROR_INVALID_SIZE;
	}
	if( payload_size > 15 + 2 ) {
		return ILLUMINATIR_ERROR_INVALID_SIZE;
	}
	uint8_t packet_size_available = *packet_size;
	*packet_size = 1 + payload_size + 1;
	if( packet_size_available < *packet_size ) {
		return ILLUMINATIR_ERROR_BUFFER_OVERFLOW;
	}
	uint8_t * p = packet;
	*p++ = 0
	     | (0b10 << 4)
	     | (payload_size - 2)
	     ;
	memcpy( p, key, key_len );
	p += key_len;
	*p++ = 0;
	memcpy( p, values, values_size );
	p += values_size;
	*p++ = illuminatir_crc8( packet, (*packet_size)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	return ILLUMINATIR_ERROR_NONE;
}
