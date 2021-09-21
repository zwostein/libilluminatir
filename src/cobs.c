#include "illuminatir.h"

#include <stddef.h>
#include <stdint.h>
#include <assert.h>


// Based on https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing

size_t illuminatir_cobs_encode( uint8_t * dst, size_t dst_size, const uint8_t * src, size_t src_size )
{
	if( !src || !dst ||
	    src_size == 0 ||
	    dst_size < ILLUMINATIR_COBS_ENCODE_DST_MAXSIZE(src_size) ) {
		return 0;
	}

	uint8_t * encode = dst;      // Encoded byte pointer
	uint8_t * codep  = encode++; // Output code pointer
	uint8_t code     = 1;        // Code value
	for( const uint8_t * byte = src; src_size--; ++byte ) {
		if( *byte ) { // Byte not zero, write it
			*encode++ = *byte, ++code;
		}
		if( !*byte || code == 0xff ) { // Input is zero or block completed, restart
			*codep = code, code = 1, codep = encode;
			if( !*byte || src_size ) {
				++encode;
			}
		}
	}
	*codep = code; // Write final code value
	return (size_t)(encode - dst);
}

size_t illuminatir_cobs_decode( uint8_t * dst, size_t dst_size, const uint8_t * src, size_t src_size )
{
	if( !src || !dst ||
	    src_size == 0 ||
	    dst_size < ILLUMINATIR_COBS_DECODE_DST_MAXSIZE(src_size) ) {
		return 0;
	}

	const uint8_t * byte   = src; // Encoded input byte pointer
	uint8_t *       decode = dst; // Decoded output byte pointer
	for( uint8_t code = 0xff, block = 0; byte < src + src_size; --block ) {
		if( block ) { // Decode block byte
			*decode++ = *byte++;
		} else {
			if( code != 0xff ) { // Encoded zero, write it
				*decode++ = 0;
			}
			block = code = *byte++; // Next block length
			if( !code ) { // Delimiter code found
				break;
			}
		}
	}
	return (size_t)(decode - (uint8_t *)dst);
}


illuminatir_error_t illuminatir_cobs_parse( const uint8_t * cobsPacket, uint8_t cobsPacket_size, illuminatir_parse_setChannel_t setChannelFunc, illuminatir_parse_setConfig_t setConfigFunc )
{
	uint8_t packet[ILLUMINATIR_PACKET_MAXSIZE];
	size_t res = illuminatir_cobs_decode( packet, sizeof(packet), cobsPacket, cobsPacket_size );
	if( res == 0 ) {
		return ILLUMINATIR_ERROR_UNKNOWN;
	}
	return illuminatir_parse( packet, res, setChannelFunc, setConfigFunc );
}


illuminatir_error_t illuminatir_cobs_build_offsetArray( uint8_t * cobsPacket, uint8_t * cobsPacket_size, uint8_t offset, const uint8_t * values, uint8_t values_size )
{
	if( !cobsPacket_size ) {
		return ILLUMINATIR_ERROR_NULL_POINTER;
	}
	uint8_t packet[ILLUMINATIR_PACKET_MAXSIZE] = { 0 };
	uint8_t packet_size = sizeof(packet);
	illuminatir_error_t err = illuminatir_build_offsetArray( packet, &packet_size, offset, values, values_size );
	if( err != ILLUMINATIR_ERROR_NONE ) {
		return err;
	}
	*cobsPacket_size = illuminatir_cobs_encode( cobsPacket, *cobsPacket_size, packet, packet_size );
	if( *cobsPacket_size == 0 ) {
		return ILLUMINATIR_ERROR_UNKNOWN;
	}
	return ILLUMINATIR_ERROR_NONE;
}


illuminatir_error_t illuminatir_cobs_build_config( uint8_t * cobsPacket, uint8_t * cobsPacket_size, const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size )
{
	if( !cobsPacket_size ) {
		return ILLUMINATIR_ERROR_NULL_POINTER;
	}
	uint8_t packet[ILLUMINATIR_PACKET_MAXSIZE] = { 0 };
	uint8_t packet_size = sizeof(packet);
	illuminatir_error_t err = illuminatir_build_config( packet, &packet_size, key, key_len, values, values_size );
	if( err != ILLUMINATIR_ERROR_NONE ) {
		return err;
	}
	*cobsPacket_size = illuminatir_cobs_encode( cobsPacket, *cobsPacket_size, packet, packet_size );
	if( *cobsPacket_size == 0 ) {
		return ILLUMINATIR_ERROR_UNKNOWN;
	}
	return ILLUMINATIR_ERROR_NONE;
}
