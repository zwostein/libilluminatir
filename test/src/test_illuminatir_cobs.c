#include <illuminatir.h>
#include <unity.h>
#include <string.h>
#include "common.h"


static uint8_t  channels[256] = {0};
static unsigned setChannel_called = 0;

static char     lastConfigKey[ILLUMINATIR_MAX_PACKET_SIZE];
static uint8_t  lastConfigKey_size = 0;
static uint8_t  lastConfigValues[ILLUMINATIR_MAX_PACKET_SIZE];
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


void check_decode( const uint8_t * in, size_t in_size, const uint8_t * expected, size_t expected_size, unsigned line )
{
	size_t dst_maxsize = ILLUMINATIR_COBS_DECODE_DST_MAXSIZE(in_size);
	UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT( dst_maxsize, expected_size, line, "ILLUMINATIR_COBS_DECODE_DST_MAXSIZE did return a smaller than expected maximum decoded size for given source size." );

	uint8_t decoded[dst_maxsize];
	size_t decoded_size = illuminatir_cobs_decode( decoded, sizeof(decoded), in, in_size );
	UNITY_TEST_ASSERT_EQUAL_UINT( decoded_size, expected_size, line, "illuminatir_cobs_decode did return an unexpected decoded size." );
	if( expected_size ) {
		UNITY_TEST_ASSERT_EQUAL_HEX8_ARRAY( decoded, expected, expected_size, line, "illuminatir_cobs_decode decoded data did not match expected data." );
	}
}


void check_encode( const uint8_t * in, size_t in_size, const uint8_t * expected, size_t expected_size, unsigned line )
{
	size_t dst_maxsize = ILLUMINATIR_COBS_ENCODE_DST_MAXSIZE(in_size);
	UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT( dst_maxsize, expected_size, line, "ILLUMINATIR_COBS_ENCODE_DST_MAXSIZE did return a smaller than expected maximum encoded size for given source size." );

	uint8_t encoded[dst_maxsize];
	size_t encoded_size = illuminatir_cobs_encode( encoded, sizeof(encoded), in, in_size );
	UNITY_TEST_ASSERT_EQUAL_UINT( encoded_size, expected_size, line, "illuminatir_cobs_encode did return an unexpected encoded size." );
	if( expected_size ) {
		UNITY_TEST_ASSERT_EQUAL_HEX8_ARRAY( encoded, expected, expected_size, line, "illuminatir_cobs_encode encoded data did not match expected data." );
	}
}


#define CHECK_DECODE(in,expected) check_decode(in, sizeof(in), expected, sizeof(expected), __LINE__)
#define CHECK_ENCODE(in,expected) check_encode(in, sizeof(in), expected, sizeof(expected), __LINE__)

#define CHECK_DECODE_AND_ENCODE(decoded,encoded) \
{ \
	CHECK_ENCODE(decoded,encoded); \
	CHECK_DECODE(encoded,decoded); \
}


void test_illuminatir_cobs_decode_encode_edgeCases( void )
{
	CHECK_DECODE_AND_ENCODE(((uint8_t[]){}), ((uint8_t[]){}));
	CHECK_DECODE_AND_ENCODE(((uint8_t[]){0xa5}), ((uint8_t[]){0x02,0xa5}));
}


void test_illuminatir_cobs_decode_encode_examplesFromWikipedia( void )
{
	// examples from https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
	CHECK_DECODE_AND_ENCODE(((uint8_t[]){0x00}), ((uint8_t[]){0x01,0x01}));
	CHECK_DECODE_AND_ENCODE(((uint8_t[]){0x00,0x00}), ((uint8_t[]){0x01,0x01,0x01}));
	CHECK_DECODE_AND_ENCODE(((uint8_t[]){0x11,0x22,0x00,0x33}), ((uint8_t[]){0x03,0x11,0x22,0x02,0x33}));
	CHECK_DECODE_AND_ENCODE(((uint8_t[]){0x11,0x22,0x33,0x44}), ((uint8_t[]){0x05,0x11,0x22,0x33,0x44}));
	CHECK_DECODE_AND_ENCODE(((uint8_t[]){0x11,0x00,0x00,0x00}), ((uint8_t[]){0x02,0x11,0x01,0x01,0x01}));
	{
		uint8_t decoded[254]; // 01 02 03 ... FD FE
		for( unsigned i = 0; i<sizeof(decoded); i++ ) {
			decoded[i] = i+1;
		}
		uint8_t encoded[255]; // FF 01 02 03 ... FD FE
		encoded[0] = 0xff;
		for( unsigned i = 0; i<sizeof(decoded); i++ ) {
			encoded[i+1] = i+1;
		}
		CHECK_DECODE_AND_ENCODE(decoded, encoded);
	}
	{
		uint8_t decoded[255]; // 00 01 02 ... FC FD FE
		for( unsigned i = 0; i<sizeof(decoded); i++ ) {
			decoded[i] = i;
		}
		uint8_t encoded[256]; // 01 FF 01 02 ... FC FD FE
		encoded[0] = 0x01;
		encoded[1] = 0xff;
		for( unsigned i = 1; i<sizeof(decoded); i++ ) {
			encoded[i+1] = i;
		}
		CHECK_DECODE_AND_ENCODE(decoded, encoded);
	}
	{
		uint8_t decoded[255]; // 01 02 03 ... FD FE FF
		for( unsigned i = 0; i<sizeof(decoded); i++ ) {
			decoded[i] = i+1;
		}
		uint8_t encoded[257]; // FF 01 02 03 ... FD FE 02 FF
		encoded[0] = 0xff;
		for( unsigned i = 1; i<sizeof(decoded); i++ ) {
			encoded[i] = i;
		}
		encoded[255] = 0x02;
		encoded[256] = 0xff;
		CHECK_DECODE_AND_ENCODE(decoded, encoded);
	}
	{
		uint8_t decoded[255]; // 02 03 04 ... FE FF 00
		for( unsigned i = 0; i<sizeof(decoded); i++ ) {
			decoded[i] = i+2;
		}
		uint8_t encoded[257]; // FF 02 03 04 ... FE FF 01 01
		encoded[0] = 0xff;
		for( unsigned i = 1; i<sizeof(decoded); i++ ) {
			encoded[i] = i+1;
		}
		encoded[255] = 0x01;
		encoded[256] = 0x01;
		CHECK_DECODE_AND_ENCODE(decoded, encoded);
	}
	{
		uint8_t decoded[255]; // 03 04 05 ... FF 00 01
		for( unsigned i = 0; i<sizeof(decoded); i++ ) {
			decoded[i] = i+3;
		}
		uint8_t encoded[256]; // FE 03 04 05 ... FF 02 01
		encoded[0] = 0xfe;
		for( unsigned i = 1; i<sizeof(decoded); i++ ) {
			encoded[i] = i+2;
		}
		encoded[254] = 0x02;
		encoded[255] = 0x01;
		CHECK_DECODE_AND_ENCODE(decoded, encoded);
	}
}


void test_illuminatir_cobs_build_parse_offsetValues( void )
{
	uint8_t cobsPacket[ILLUMINATIR_COBS_ENCODE_DST_MAXSIZE(ILLUMINATIR_MAX_PACKET_SIZE)];
	uint8_t cobsPacket_size = sizeof(cobsPacket);
	const uint8_t values[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_cobs_build_offsetArray( cobsPacket, &cobsPacket_size, 4, values, sizeof(values) ) );
	TEST_ASSERT_LESS_OR_EQUAL_UINT( sizeof(cobsPacket), cobsPacket_size );

	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_cobs_parse( cobsPacket, cobsPacket_size, setChannel, setConfig ) );
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


void test_illuminatir_cobs_build_parse_config( void )
{
	uint8_t cobsPacket[ILLUMINATIR_COBS_ENCODE_DST_MAXSIZE(ILLUMINATIR_MAX_PACKET_SIZE)];
	uint8_t cobsPacket_size = sizeof(cobsPacket);
	const char key[] = "IlluminatIR";
	const uint8_t values[] = {1,2,3,4,5};
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_cobs_build_config( cobsPacket, &cobsPacket_size, key, sizeof(key)-1, values, sizeof(values) ) );
	TEST_ASSERT_LESS_OR_EQUAL_UINT( sizeof(cobsPacket), cobsPacket_size );

	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_cobs_parse( cobsPacket, cobsPacket_size, setChannel, setConfig ) );
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


int main( void )
{
	UNITY_BEGIN();
	RUN_TEST(test_illuminatir_cobs_decode_encode_examplesFromWikipedia);
	RUN_TEST(test_illuminatir_cobs_decode_encode_edgeCases);
	RUN_TEST(test_illuminatir_cobs_build_parse_offsetValues);
	RUN_TEST(test_illuminatir_cobs_build_parse_config);
	return UNITY_END();
}
