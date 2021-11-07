#include "illuminatir.h"

#include <stddef.h>
#include <stdint.h>
#include <assert.h>


void illuminatir_rand( uint8_t * packets, size_t size )
{
	if( !packets || size < 3 ) {
		return;
	}
	illuminatir_lfsr127_init( packets[size-1] ); // using the packet's CRC as seed for the randomizer
	for( size_t i = 1; i < size-1; i++ ) {       // randomize everything between header and CRC
		packets[i] ^= illuminatir_lfsr127_uint8();
	}
}


illuminatir_error_t illuminatir_rand_cobs_parse( const uint8_t * randCobsPackets, uint8_t randCobsPackets_size, illuminatir_parse_setChannel_t setChannelFunc, illuminatir_parse_setConfig_t setConfigFunc )
{
	uint8_t packets[ILLUMINATIR_PACKET_MAXSIZE]; // TODO: maybe use a larger buffer for multiple packets
	size_t packets_size = illuminatir_cobs_decode( packets, sizeof(packets), randCobsPackets, randCobsPackets_size );
	if( packets_size == 0 ) {
		return ILLUMINATIR_ERROR_INVALID_SIZE;
	}
	illuminatir_rand( packets, packets_size );
	return illuminatir_parse( packets, packets_size, setChannelFunc, setConfigFunc );
}


illuminatir_error_t illuminatir_rand_cobs_build_offsetArray( uint8_t * randCobsPacket, uint8_t * randCobsPacket_size, uint8_t offset, const uint8_t * values, uint8_t values_size )
{
	if( !randCobsPacket_size ) {
		return ILLUMINATIR_ERROR_NULL_POINTER;
	}
	uint8_t packet[ILLUMINATIR_PACKET_MAXSIZE] = { 0 };
	uint8_t packet_size = sizeof(packet);
	illuminatir_error_t err = illuminatir_build_offsetArray( packet, &packet_size, offset, values, values_size );
	if( err != ILLUMINATIR_ERROR_NONE ) {
		return err;
	}
	illuminatir_rand( packet, packet_size );
	*randCobsPacket_size = illuminatir_cobs_encode( randCobsPacket, *randCobsPacket_size, packet, packet_size );
	if( *randCobsPacket_size == 0 ) {
		return ILLUMINATIR_ERROR_UNKNOWN;
	}
	return ILLUMINATIR_ERROR_NONE;
}


illuminatir_error_t illuminatir_rand_cobs_build_config( uint8_t * randCobsPacket, uint8_t * randCobsPacket_size, const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size )
{
	if( !randCobsPacket_size ) {
		return ILLUMINATIR_ERROR_NULL_POINTER;
	}
	uint8_t packet[ILLUMINATIR_PACKET_MAXSIZE] = { 0 };
	uint8_t packet_size = sizeof(packet);
	illuminatir_error_t err = illuminatir_build_config( packet, &packet_size, key, key_len, values, values_size );
	if( err != ILLUMINATIR_ERROR_NONE ) {
		return err;
	}
	illuminatir_rand( packet, packet_size );
	*randCobsPacket_size = illuminatir_cobs_encode( randCobsPacket, *randCobsPacket_size, packet, packet_size );
	if( *randCobsPacket_size == 0 ) {
		return ILLUMINATIR_ERROR_UNKNOWN;
	}
	return ILLUMINATIR_ERROR_NONE;
}
