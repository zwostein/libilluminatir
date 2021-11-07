#include <illuminatir.h>
#include <unity.h>
#include <string.h>
#include "common.h"


static uint8_t  channels[256] = {0};
static unsigned setChannel_called = 0;

static char     lastConfigKey[ILLUMINATIR_CONFIG_KEY_MAXLEN];
static uint8_t  lastConfigKey_size = 0;
static uint8_t  lastConfigValues[ILLUMINATIR_CONFIG_VALUES_MAXSIZE];
static uint8_t  lastConfigValues_size = 0;
static unsigned setConfig_called = 0;


void setUp(void) {
	memset( channels, 0, sizeof(channels) );
	setChannel_called = 0;

	memset( lastConfigKey, 0, sizeof(lastConfigKey) );
	lastConfigKey_size    = 0;
	memset( lastConfigValues, 0, sizeof(lastConfigValues) );
	lastConfigValues_size = 0;
	setConfig_called      = 0;
}


void tearDown(void) {
	// clean stuff up here
}


void setChannel( uint8_t channel, uint8_t value )
{
	channels[channel] = value;
	setChannel_called++;
}


void setConfig( const char * key, uint8_t key_size, const uint8_t * values, uint8_t values_size )
{
	memcpy( lastConfigKey, key, key_size );
	lastConfigKey_size = key_size;
	memcpy( lastConfigValues, values, values_size );
	lastConfigValues_size = values_size;
	setConfig_called++;
}


void test_illuminatir_rand_cobs_build_parse_offsetValues( void )
{
	uint8_t cobsPacket[ILLUMINATIR_COBS_PACKET_MAXSIZE];
	uint8_t cobsPacket_size = sizeof(cobsPacket);
	const uint8_t values[] = {1,2,3,4,5,6,7,8};
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_build_offsetArray( cobsPacket, &cobsPacket_size, 4, values, sizeof(values) ) );
	TEST_ASSERT_LESS_OR_EQUAL_UINT( sizeof(cobsPacket), cobsPacket_size );

	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_parse( cobsPacket, cobsPacket_size, setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 8, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8( 1, channels[ 4] );
	TEST_ASSERT_EQUAL_UINT8( 2, channels[ 5] );
	TEST_ASSERT_EQUAL_UINT8( 3, channels[ 6] );
	TEST_ASSERT_EQUAL_UINT8( 4, channels[ 7] );
	TEST_ASSERT_EQUAL_UINT8( 5, channels[ 8] );
	TEST_ASSERT_EQUAL_UINT8( 6, channels[ 9] );
	TEST_ASSERT_EQUAL_UINT8( 7, channels[10] );
	TEST_ASSERT_EQUAL_UINT8( 8, channels[11] );
}


void test_illuminatir_rand_cobs_build_parse_config( void )
{
	uint8_t cobsPacket[ILLUMINATIR_COBS_PACKET_MAXSIZE];
	uint8_t cobsPacket_size = sizeof(cobsPacket);
	const char key[] = "IlluminatIR";
	const uint8_t values[] = {42};
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_build_config( cobsPacket, &cobsPacket_size, key, sizeof(key)-1, values, sizeof(values) ) );
	TEST_ASSERT_LESS_OR_EQUAL_UINT( sizeof(cobsPacket), cobsPacket_size );

	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_parse( cobsPacket, cobsPacket_size, setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setConfig_called );
	TEST_ASSERT_NOT_NULL( lastConfigKey );
	TEST_ASSERT_NOT_NULL( lastConfigValues );
	TEST_ASSERT_EQUAL_UINT( sizeof(key)-1, lastConfigKey_size );
	TEST_ASSERT_EQUAL_STRING( key, lastConfigKey );
	TEST_ASSERT_EQUAL_UINT( 1, lastConfigValues_size );
	TEST_ASSERT_EQUAL_UINT8( 42, lastConfigValues[0] );
}


void test_illuminatir_rand_cobs_build_parse_offsetValues_maximumSize( void )
{
	uint8_t cobsPacket[ILLUMINATIR_COBS_PACKET_MAXSIZE];
	uint8_t cobsPacket_size = sizeof(cobsPacket);
	const uint8_t values[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_build_offsetArray( cobsPacket, &cobsPacket_size, 4, values, sizeof(values) ) );
	TEST_ASSERT_LESS_OR_EQUAL_UINT( sizeof(cobsPacket), cobsPacket_size );

	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_parse( cobsPacket, cobsPacket_size, setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 16, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8(  1, channels[ 4] );
	TEST_ASSERT_EQUAL_UINT8(  2, channels[ 5] );
	TEST_ASSERT_EQUAL_UINT8(  3, channels[ 6] );
	TEST_ASSERT_EQUAL_UINT8(  4, channels[ 7] );
	TEST_ASSERT_EQUAL_UINT8(  5, channels[ 8] );
	TEST_ASSERT_EQUAL_UINT8(  6, channels[ 9] );
	TEST_ASSERT_EQUAL_UINT8(  7, channels[10] );
	TEST_ASSERT_EQUAL_UINT8(  8, channels[11] );
	TEST_ASSERT_EQUAL_UINT8(  9, channels[12] );
	TEST_ASSERT_EQUAL_UINT8( 10, channels[13] );
	TEST_ASSERT_EQUAL_UINT8( 11, channels[14] );
	TEST_ASSERT_EQUAL_UINT8( 12, channels[15] );
	TEST_ASSERT_EQUAL_UINT8( 13, channels[16] );
	TEST_ASSERT_EQUAL_UINT8( 14, channels[17] );
	TEST_ASSERT_EQUAL_UINT8( 15, channels[18] );
	TEST_ASSERT_EQUAL_UINT8( 16, channels[19] );
}


void test_illuminatir_rand_cobs_build_parse_config_maximumSize( void )
{
	uint8_t cobsPacket[ILLUMINATIR_COBS_PACKET_MAXSIZE];
	uint8_t cobsPacket_size = sizeof(cobsPacket);
	const char key[] = "IlluminatIR";
	const uint8_t values[] = {1,2,3,4,5};
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_build_config( cobsPacket, &cobsPacket_size, key, sizeof(key)-1, values, sizeof(values) ) );
	TEST_ASSERT_LESS_OR_EQUAL_UINT( sizeof(cobsPacket), cobsPacket_size );

	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_parse( cobsPacket, cobsPacket_size, setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setConfig_called );
	TEST_ASSERT_NOT_NULL( lastConfigKey );
	TEST_ASSERT_NOT_NULL( lastConfigValues );
	TEST_ASSERT_EQUAL_UINT( sizeof(key)-1, lastConfigKey_size );
	TEST_ASSERT_EQUAL_STRING( key, lastConfigKey );
	TEST_ASSERT_EQUAL_UINT( 5, lastConfigValues_size );
	TEST_ASSERT_EQUAL_UINT8( 1, lastConfigValues[0] );
	TEST_ASSERT_EQUAL_UINT8( 2, lastConfigValues[1] );
	TEST_ASSERT_EQUAL_UINT8( 3, lastConfigValues[2] );
	TEST_ASSERT_EQUAL_UINT8( 4, lastConfigValues[3] );
	TEST_ASSERT_EQUAL_UINT8( 5, lastConfigValues[4] );
}


void test_illuminatir_rand_cobs_build_parse_offsetValues_multiple( void )
{
	uint8_t packets[ILLUMINATIR_PACKET_MAXSIZE*2];
	uint8_t packets_size = sizeof(packets);

	uint8_t * packet1 = packets;
	uint8_t packet1_size = sizeof(packets)/2;
	const uint8_t values1[] = {1,2,3,4,5,6};
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_build_offsetArray( packet1, &packet1_size, 4, values1, sizeof(values1) ) );
	TEST_ASSERT_LESS_OR_EQUAL_UINT( sizeof(packets)/2, packet1_size );

	uint8_t * packet2 = packets + packet1_size;
	uint8_t packet2_size = sizeof(packets)/2;
	const uint8_t values2[] = {11,12,13,14};
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_build_offsetArray( packet2, &packet2_size, 20, values2, sizeof(values2) ) );
	TEST_ASSERT_LESS_OR_EQUAL_UINT( sizeof(packets)/2, packet2_size );
	
	packets_size = packet1_size + packet2_size;

	illuminatir_rand( packets, packets_size );

	uint8_t cobsPackets[ILLUMINATIR_COBS_PACKET_MAXSIZE];
	uint8_t cobsPackets_size = sizeof(cobsPackets);
	cobsPackets_size = illuminatir_cobs_encode( cobsPackets, cobsPackets_size, packets, packets_size );

	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_rand_cobs_parse( cobsPackets, cobsPackets_size, setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 10, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8(  1, channels[ 4] );
	TEST_ASSERT_EQUAL_UINT8(  2, channels[ 5] );
	TEST_ASSERT_EQUAL_UINT8(  3, channels[ 6] );
	TEST_ASSERT_EQUAL_UINT8(  4, channels[ 7] );
	TEST_ASSERT_EQUAL_UINT8(  5, channels[ 8] );
	TEST_ASSERT_EQUAL_UINT8(  6, channels[ 9] );
	TEST_ASSERT_EQUAL_UINT8( 11, channels[20] );
	TEST_ASSERT_EQUAL_UINT8( 12, channels[21] );
	TEST_ASSERT_EQUAL_UINT8( 13, channels[22] );
	TEST_ASSERT_EQUAL_UINT8( 14, channels[23] );
}


int main( void )
{
	UNITY_BEGIN();
	RUN_TEST(test_illuminatir_rand_cobs_build_parse_offsetValues);
	RUN_TEST(test_illuminatir_rand_cobs_build_parse_config);
	RUN_TEST(test_illuminatir_rand_cobs_build_parse_offsetValues_maximumSize);
	RUN_TEST(test_illuminatir_rand_cobs_build_parse_config_maximumSize);
	RUN_TEST(test_illuminatir_rand_cobs_build_parse_offsetValues_multiple);
	return UNITY_END();
}
