#include <illuminatir.h>
#include <unity.h>
#include <string.h>
#include "common.h"


static uint8_t         channels[256] = {0};
static unsigned        setChannel_called = 0;

static const char *    lastConfigKey = NULL;
static uint8_t         lastConfigKey_len = 0;
static const uint8_t * lastConfigValues = NULL;
static uint8_t         lastConfigValues_size = 0;
static unsigned        setConfig_called = 0;


void setUp(void) {
	memset( channels, 0, sizeof(channels) );
	setChannel_called = 0;

	lastConfigKey         = NULL;
	lastConfigKey_len    = 0;
	lastConfigValues      = NULL;
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


void setConfig( const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size )
{
	lastConfigKey = key;
	lastConfigKey_len = key_len;
	lastConfigValues = values;
	lastConfigValues_size = values_size;
	setConfig_called++;
}


void test_illuminatir_parse_offsetArray_crcError( void )
{
	uint8_t packet[] = {0x00,0x00,0,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED ) - 1;
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_INVALID_CRC, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 0, setChannel_called );
	TEST_ASSERT_EQUAL_UINT( 0, setConfig_called );
}


void test_illuminatir_parse_offsetArray_tooShort( void )
{
	uint8_t packet[] = {0x00,0x00,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_PACKET_TOO_SHORT, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 0, setChannel_called );
	TEST_ASSERT_EQUAL_UINT( 0, setConfig_called );
}


void test_illuminatir_parse_offsetArray_noOffset_minimumSize( void )
{
	uint8_t packet[] = {0x00,0x00,42,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8( 42, channels[0] );
}


void test_illuminatir_parse_offsetArray_noOffset_maximumSize( void )
{
	uint8_t packet[] = {0x0f,0x00,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 16, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8(  1, channels[ 0] );
	TEST_ASSERT_EQUAL_UINT8(  2, channels[ 1] );
	TEST_ASSERT_EQUAL_UINT8(  3, channels[ 2] );
	TEST_ASSERT_EQUAL_UINT8(  4, channels[ 3] );
	TEST_ASSERT_EQUAL_UINT8(  5, channels[ 4] );
	TEST_ASSERT_EQUAL_UINT8(  6, channels[ 5] );
	TEST_ASSERT_EQUAL_UINT8(  7, channels[ 6] );
	TEST_ASSERT_EQUAL_UINT8(  8, channels[ 7] );
	TEST_ASSERT_EQUAL_UINT8(  9, channels[ 8] );
	TEST_ASSERT_EQUAL_UINT8( 10, channels[ 9] );
	TEST_ASSERT_EQUAL_UINT8( 11, channels[10] );
	TEST_ASSERT_EQUAL_UINT8( 12, channels[11] );
	TEST_ASSERT_EQUAL_UINT8( 13, channels[12] );
	TEST_ASSERT_EQUAL_UINT8( 14, channels[13] );
	TEST_ASSERT_EQUAL_UINT8( 15, channels[14] );
	TEST_ASSERT_EQUAL_UINT8( 16, channels[15] );
}


void test_illuminatir_parse_offsetArray_offset_minimumSize( void )
{
	uint8_t packet[] = {0x00,0x03,42,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8( 42, channels[3] );
}


void test_illuminatir_parse_offsetArray_offset_maximumSize( void )
{
	uint8_t packet[] = {0x0f,0x04,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
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


void test_illuminatir_parse_channelValuePairs_minimumSize( void )
{
	uint8_t packet[] = {0x10,0x07,21,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8( 21, channels[7] );
}


void test_illuminatir_parse_channelValuePairs_maximumSize( void )
{
	channels[16] = 1; // should get reset to 0
	uint8_t packet[] = {0x1f,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 9, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8(  1, channels[ 0] );
	TEST_ASSERT_EQUAL_UINT8(  3, channels[ 2] );
	TEST_ASSERT_EQUAL_UINT8(  5, channels[ 4] );
	TEST_ASSERT_EQUAL_UINT8(  7, channels[ 6] );
	TEST_ASSERT_EQUAL_UINT8(  9, channels[ 8] );
	TEST_ASSERT_EQUAL_UINT8( 11, channels[10] );
	TEST_ASSERT_EQUAL_UINT8( 13, channels[12] );
	TEST_ASSERT_EQUAL_UINT8( 15, channels[14] );
	TEST_ASSERT_EQUAL_UINT8(  0, channels[16] );
}


void test_illuminatir_parse_config_noValues_minimumSize( void )
{
	uint8_t packet[] = {0x20,'X',0,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setConfig_called );
	TEST_ASSERT_NOT_NULL( lastConfigKey );
	TEST_ASSERT_NOT_NULL( lastConfigValues );
	TEST_ASSERT_EQUAL_UINT( 1, lastConfigKey_len );
	TEST_ASSERT_EQUAL_STRING_LEN( "X", lastConfigKey, lastConfigKey_len );
	TEST_ASSERT_EQUAL_UINT( 0, lastConfigValues_size );
}


void test_illuminatir_parse_config_noKey_minimumSize( void )
{
	uint8_t packet[] = {0x20,0,1,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setConfig_called );
	TEST_ASSERT_NOT_NULL( lastConfigKey );
	TEST_ASSERT_NOT_NULL( lastConfigValues );
	TEST_ASSERT_EQUAL_UINT( 0, lastConfigKey_len );
	TEST_ASSERT_EQUAL_UINT( 1, lastConfigValues_size );
	TEST_ASSERT_EQUAL_UINT8( 1, lastConfigValues[0] );
}


void test_illuminatir_parse_config_maximumSize( void )
{
	uint8_t packet[] = {0x2f,'I','l','l','u','m','i','n','a','t','I','R',0,1,2,3,4,5,0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setConfig_called );
	TEST_ASSERT_NOT_NULL( lastConfigKey );
	TEST_ASSERT_NOT_NULL( lastConfigValues );
	TEST_ASSERT_EQUAL_UINT( 11, lastConfigKey_len );
	TEST_ASSERT_EQUAL_STRING_LEN( "IlluminatIR", lastConfigKey, lastConfigKey_len );
	TEST_ASSERT_EQUAL_UINT( 5, lastConfigValues_size );
	TEST_ASSERT_EQUAL_UINT8( 1, lastConfigValues[0] );
	TEST_ASSERT_EQUAL_UINT8( 2, lastConfigValues[1] );
	TEST_ASSERT_EQUAL_UINT8( 3, lastConfigValues[2] );
	TEST_ASSERT_EQUAL_UINT8( 4, lastConfigValues[3] );
	TEST_ASSERT_EQUAL_UINT8( 5, lastConfigValues[4] );
}


void test_illuminatir_parse_config_noKeyDelimiter_maximumSize( void )
{
	uint8_t packet[] = {0x2f,'I','l','l','u','m','i','n','a','t','I','R',' ','T','e','s','t','!',0x00};
	packet[sizeof(packet)-1] = illuminatir_crc8( packet, sizeof(packet)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packet, sizeof(packet), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 1, setConfig_called );
	TEST_ASSERT_NOT_NULL( lastConfigKey );
	TEST_ASSERT_NOT_NULL( lastConfigValues );
	TEST_ASSERT_EQUAL_UINT( 17, lastConfigKey_len );
	TEST_ASSERT_EQUAL_STRING_LEN( "IlluminatIR Test!", lastConfigKey, lastConfigKey_len );
	TEST_ASSERT_EQUAL_UINT( 0, lastConfigValues_size );
}


void test_illuminatir_parse_offsetArray_offset_multiple( void )
{
	uint8_t packets[] = {0x00,0x03,42,0x00,0x00,0x04,21,0x00};
	uint8_t * packet1 = packets;
	size_t packet1_size = 4;
	uint8_t * packet2 = packets + packet1_size;
	size_t packet2_size = 4;
	packet1[packet1_size-1] = illuminatir_crc8( packet1, packet1_size-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	packet2[packet2_size-1] = illuminatir_crc8( packet2, packet2_size-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_parse( packets, sizeof(packets), setChannel, setConfig ) );
	TEST_ASSERT_EQUAL_UINT( 2, setChannel_called );
	TEST_ASSERT_EQUAL_UINT8( 42, channels[3] );
	TEST_ASSERT_EQUAL_UINT8( 21, channels[4] );
}


int main( void )
{
	UNITY_BEGIN();
	RUN_TEST(test_illuminatir_parse_offsetArray_crcError);
	RUN_TEST(test_illuminatir_parse_offsetArray_tooShort);
	RUN_TEST(test_illuminatir_parse_offsetArray_noOffset_minimumSize);
	RUN_TEST(test_illuminatir_parse_offsetArray_noOffset_maximumSize);
	RUN_TEST(test_illuminatir_parse_offsetArray_offset_minimumSize);
	RUN_TEST(test_illuminatir_parse_offsetArray_offset_maximumSize);
	RUN_TEST(test_illuminatir_parse_channelValuePairs_minimumSize);
	RUN_TEST(test_illuminatir_parse_channelValuePairs_maximumSize);
	RUN_TEST(test_illuminatir_parse_config_noValues_minimumSize);
	RUN_TEST(test_illuminatir_parse_config_noKey_minimumSize);
	RUN_TEST(test_illuminatir_parse_config_maximumSize);
	RUN_TEST(test_illuminatir_parse_config_noKeyDelimiter_maximumSize);
	RUN_TEST(test_illuminatir_parse_offsetArray_offset_multiple);
	return UNITY_END();
}
