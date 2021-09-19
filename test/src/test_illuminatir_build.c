#include <illuminatir.h>
#include <unity.h>
#include <string.h>
#include "common.h"


void setUp(void) {
	// set stuff up here
}


void tearDown(void) {
	// clean stuff up here
}


void test_illuminatir_build_offsetArray( void )
{
	uint8_t packet[ILLUMINATIR_MAX_PACKET_SIZE];
	uint8_t values[] = {42};
	uint8_t packet_size = sizeof(packet);
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_build_offsetArray( packet, &packet_size, 0, values, sizeof(values) ) );

	uint8_t expected[] = {0x00,0x00,42,0x00};
	expected[sizeof(expected)-1] = illuminatir_crc8( expected, sizeof(expected)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_EQUAL_UINT8( sizeof(expected), packet_size );
	TEST_ASSERT_EQUAL_HEX8_ARRAY( expected, packet, packet_size );
}


void test_illuminatir_build_config( void )
{
	uint8_t packet[ILLUMINATIR_MAX_PACKET_SIZE];
	const char key[] = "Test";
	uint8_t values[] = {1,2,3};
	uint8_t packet_size = sizeof(packet);
	TEST_ASSERT_ILLUMINATIR_ERROR( ILLUMINATIR_ERROR_NONE, illuminatir_build_config( packet, &packet_size, key, sizeof(key)-1, values, sizeof(values) ) );

	uint8_t expected[] = {0x26,'T','e','s','t',0,1,2,3,0x00};
	expected[sizeof(expected)-1] = illuminatir_crc8( expected, sizeof(expected)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	TEST_ASSERT_EQUAL_UINT8( sizeof(expected), packet_size );
	TEST_ASSERT_EQUAL_HEX8_ARRAY( expected, packet, packet_size );
}


int main( void )
{
	UNITY_BEGIN();
	RUN_TEST(test_illuminatir_build_offsetArray);
	RUN_TEST(test_illuminatir_build_config);
	return UNITY_END();
}
