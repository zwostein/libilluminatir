#include <assert.h>
#include <stdio.h>
#include <illuminatir.h>

void setChannel( uint8_t channel, uint8_t value )
{
	printf( "Setting channel %u to %u.\n", channel, value );
}

int main( void )
{
	uint8_t packet[] = { 0x00, 8, 42, 0xAA }; // sets channel 8 to 42
	illuminatir_error_t err = illuminatir_parse( packet, sizeof(packet), setChannel, NULL );
	assert( err == ILLUMINATIR_ERROR_NONE );
	return 0;
}
