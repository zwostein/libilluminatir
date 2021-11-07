#include <illuminatir.h>
#include <unity.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"


int16_t randoms[256];

void randoms_clear( void )
{
	memset( randoms, -1, sizeof(randoms) );
}

bool randoms_is_new( uint8_t x )
{
	for( unsigned i = 0; i < sizeof(randoms) / sizeof(randoms[0]); i++ ) {
		if( randoms[i] == -1 ) {
			randoms[i] = x;
			return true;
		} else if( randoms[i] == x ) {
			return false;
		}
	}
	TEST_FAIL_MESSAGE("Should never reach this!");
	return true;
}


void setUp( void ) {
	randoms_clear();
}


void tearDown( void ) {
}


void test_illuminatir_lfsr127_uint8( void )
{
	for( unsigned seed = 1; seed < 256; seed++ ) {
		randoms_clear();
		illuminatir_lfsr127_init( seed );
		unsigned long period = 1;
		while( randoms_is_new(illuminatir_lfsr127_uint8()) ) {
			period++;
		}
		TEST_ASSERT_GREATER_OR_EQUAL_UINT8( 127, period );
	}
}


int main( void )
{
	UNITY_BEGIN();
	RUN_TEST(test_illuminatir_lfsr127_uint8);
	return UNITY_END();
}
