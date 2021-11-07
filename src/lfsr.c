#include "illuminatir.h"

#include <stdint.h>


// https://en.wikipedia.org/wiki/Linear-feedback_shift_register


/* LFSR Bits:           7
 * Feedback polynomial: x^7 + x^6 + 1
 * Feedback taps:       1100000
 * Feedback bits:       0, 1
 * Period:              127
 */

static uint8_t lfsr127 = 1;

void illuminatir_lfsr127_init( uint8_t seed )
{
	// seed must be nonzero
	if( !seed ) {
		seed = 1;
	}
	lfsr127 = seed;
}

uint8_t illuminatir_lfsr127_uint8( void )
{
	uint8_t out = 0;
	for( uint8_t i = 0; i < 8; i++ ) {
		uint8_t feedback = (lfsr127 >> 0) ^ (lfsr127 >> 1);
		lfsr127 = (lfsr127 >> 1) | (feedback << 6);
		out = (out << 1) | (lfsr127 & 1);
	}
	return out;
}
