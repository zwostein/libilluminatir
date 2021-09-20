#include <assert.h>
#include <stdio.h>
#include <illuminatir.h>

int main( void )
{
	uint8_t cobsPacket[ILLUMINATIR_COBS_ENCODE_DST_MAXSIZE(ILLUMINATIR_MAX_PACKET_SIZE)];
	uint8_t cobsPacket_size = sizeof(cobsPacket);
	uint8_t values[] = {11,22,33,44,55,66,77,88};
	
	illuminatir_error_t err = illuminatir_cobs_build_offsetArray( cobsPacket, &cobsPacket_size, 0, values, sizeof(values) );
	assert( err == ILLUMINATIR_ERROR_NONE );
	
	// print the COBS encoded packet
	printf("0x");
	for( unsigned i = 0; i < cobsPacket_size; i++ )
		printf("%02X", cobsPacket[i]);
	printf("\n");
	return 0;
}
