/*
 * IlluminatIR
 * Copyright (C) 2021  zwostein
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * \file
 * \brief The libIlluminatIR header file.
 **/

/**
 * \page libilluminatir_examples Examples
 * \brief A collection of examples.
 * \tableofcontents
 *
 * \section libilluminatir_example_parse_minimal_sec Minimal Parser Example
 * A minimal example showing the receiver side that is only interested in parsing some "received" packet.
 * \include example_parse_minimal.c
 *
 * \section libilluminatir_example_cobs_build_sec Building COBS Encoded Packets
 * Building a COBS encoded packet.
 * \include example_cobs_build.c
 **/

/**
 * \page libilluminatir_protocol Protocol
 * \brief A description of the packets defined in this protocol.
 * \tableofcontents
 *
 * \section libilluminatir_packets_sec IlluminatIR Packets:
 * \code{.unparsed}
 * .----------------.----------------.----------------.
 * | Header         | Payload        | Checksum       |
 * | 1 Byte         | 2 to 17 Bytes  | 1 Byte         |
 * '----------------'----------------'----------------'
 * \endcode
 *
 * For use over unreliable interfaces (like serial infrared transmitter/receiver) the packets shall be encoded using [Consistent Overhead Byte Stuffing] (https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing).
 * The \ref COBS module provides functions for handling COBS packets.
 *
 *
 * \subsection libilluminatir_header_sec Header (1 byte):
 * \code{.unparsed}
 * .----------------------------------------------------------.
 * | Header                                                   |
 * |--------------.--------------.----------------------------|
 * |   0      1   |   2      3   |   4      5      6      7   |
 * |   Version    | PayloadType  |        PayloadSize         |
 * |--------------'--------------'----------------------------|
 * | 1 Byte                                                   |
 * '----------------------------------------------------------'
 * \endcode
 * - 2 bit - Version:
 * 	- \c 0b00 - Default
 * 	- \c 0bxx - Reserved/Ignored
 * - 2 bit - PayloadType:
 * 	- \c 0b00 - OffsetArray
 * 	- \c 0b01 - ChannelValuePairs
 * 	- \c 0b10 - Config
 * 	- \c 0bxx - Reserved/Ignored
 * - 4 bit - PayloadSize:
 * 	- \c 0..15 - 2 to 17 bytes of payload (PayloadSize-2)
 *
 *
 * \subsection libilluminatir_payload_offsetArray_sec Payload (Type 0) - OffsetArray:
 * \code{.unparsed}
 * .--------------------------------------------------------.
 * | Payload - OffsetArray (if PayloadType==0b00)           |
 * |----------------.                                       |
 * | Offset         |----------------.-----.----------------|
 * |                | ChannelValue 0 |     | ChannelValue N |
 * | 1 Byte         | 1 Byte         | ... | 1 Byte         |
 * |----------------'----------------'-----'----------------|
 * | PayloadSize Bytes                                      |
 * '--------------------------------------------------------'
 * \endcode
 * - 8 bit - Offset
 * - Values - Repeated N times:
 * 	- 8 bit - Channel (Offset+n) value
 *
 * \note The channel number wraps around (256==0, if Offset+N>255).
 *
 * \sa illuminatir_build_offsetArray, illuminatir_cobs_build_offsetArray
 *
 *
 * \subsection libilluminatir_payload_channelValuePairs_sec Payload (Type 1) - ChannelValuePairs:
 * \code{.unparsed}
 * .-------------------------------------------------------------------------.
 * | Payload - ChannelValuePairs (if PayloadType==0b01)                      |
 * |---------------------------------.-----.---------------------------------|
 * | Channel:Value pair 0            |     | Channel:Value pair N            |
 * |----------------.----------------|     |----------------.----------------|
 * | Channel        | Value          | ... | Channel        | Value          |
 * | 1 Byte         | 1 Byte         |     | 1 Byte         | 1 Byte         |
 * |----------------'----------------'-----'----------------'----------------|
 * | PayloadSize Bytes                                                       |
 * '-------------------------------------------------------------------------'
 * \endcode
 * - Channel:Value pair - Repeated N times:
 * 	- 8 bit - Channel
 * 	- 8 bit - Value
 *
 * \note If PayloadSize is odd, Value defaults to 0.
 *
 * \todo Implement \c illuminatir_build_channelValuePairs and \c illuminatir_cobs_build_channelValuePairs.
 *
 *
 * \subsection libilluminatir_payload_config_sec Payload (Type 2) - Config:
 * \code{.unparsed}
 * .-------------------------------------------------------------------------.
 * | Payload - Config (if PayloadType==0b10)                                 |
 * |---------------------------------.---------------------------------------|
 * | Key                             | Optional                              |
 * |----------------.----------------|-----.---------------------------------|
 * | Character      |                |     | Value                           |
 * |                |                |     |----------------.----------------|
 * |                |                |     | Data           |                |
 * | 1 Byte         |      ...       |  0  | 1 Byte         |      ...       |
 * |----------------'----------------'-----'----------------'----------------|
 * | PayloadSize Bytes                                                       |
 * '-------------------------------------------------------------------------'
 * \endcode
 * - Key - Repeated until null-terminator or PayloadSize reached:
 * 	- 8 bit - Character
 * - Optional:
 * 	- 8 bit - 0 delimiter
 * 	- Value - Repeated N times:
 * 		- 8 bit - Data bytes
 *
 * \note The exact format of Value is specific to each Key.
 * It is possible that a Key divides the Value field into sub-values.
 * Some common example Keys can be seen in the table \ref libilluminatir_payload_config_keys_sec "below".
 *
 * \sa illuminatir_build_config, illuminatir_cobs_build_config
 *
 *
 * \subsubsection libilluminatir_payload_config_keys_sec Common Configuration Keys (Case insensitive):
 *
 * |           Key |     Value(s)     | Description                                                      |
 * | ------------: | :--------------: | :--------------------------------------------------------------- |
 * |        "Base" |                  | Base channel (Might be overridden by hardware switches).         |
 * |               |   uint8 - Offset | The devices channels are offset by this number.                  |
 * | "MakeDefault" |                  | Make current values the default on power-up.                     |
 *
 *
 * \subsection libilluminatir_checksum_sec Checksum (1 byte):
 * - 8 bit - CRC-8/KOOP (Header+Payload)
 **/


#ifndef ILLUMINATIR_INCLUDED
#define ILLUMINATIR_INCLUDED

#include <stdint.h>
#include <stddef.h>


/**
 * @defgroup IlluminatIR IlluminatIR
 * \brief IlluminatIR protocol handling functions.
 *
 * Provides functions to build and parse IlluminatIR packets.
 * @{
 */

#define ILLUMINATIR_MIN_PACKET_SIZE 4  ///< Minimum size of raw packets.
#define ILLUMINATIR_MAX_PACKET_SIZE 19 ///< Maximum size of raw packets.

#define ILLUMINATIR_OFFSETARRAY_MIN_VALUES 1  ///< Minimum number of channels in an OffsetArray type packet.
#define ILLUMINATIR_OFFSETARRAY_MAX_VALUES 16 ///< Maximum number of channels in an OffsetArray type packet.


/**
 * \brief Error return types.
 */
typedef enum {
	ILLUMINATIR_ERROR_NONE,                ///< Successful execution of function.
	ILLUMINATIR_ERROR_UNKNOWN,             ///< Unknown/internal error.
	ILLUMINATIR_ERROR_NULL_POINTER,        ///< Unexpected null pointer encountered.
	ILLUMINATIR_ERROR_BUFFER_OVERFLOW,     ///< Provided buffer was too small.
	ILLUMINATIR_ERROR_PACKET_TOO_SHORT,    ///< Packet was too short to be valid.
	ILLUMINATIR_ERROR_PACKET_TOO_LONG,     ///< Packet was too long to be valid.
	ILLUMINATIR_ERROR_UNSUPPORTED_VERSION, ///< Unsupported packet version.
	ILLUMINATIR_ERROR_UNSUPPORTED_FORMAT,  ///< Unsupported payload format.
	ILLUMINATIR_ERROR_INVALID_SIZE,        ///< Packet's size does not match provided data.
	ILLUMINATIR_ERROR_INVALID_CRC,         ///< Packet CRC is invalid.
} illuminatir_error_t;

/**
 * \brief Returns a static description string for \p err.
 *
 * \note On \c avr-gcc this function is called \c illuminatir_error_toString_P instead and returns a string in \c PROGMEM.
 * \param err Error code
 */
#if defined(__AVR)
const char * illuminatir_error_toString_P( illuminatir_error_t err );
#else
const char * illuminatir_error_toString( illuminatir_error_t err );
#endif

/**
 * \brief Signature of a function that sets a single channel.
 *
 * \param channel Channel number.
 * \param value   New channel value.
 */
typedef void (*illuminatir_parse_setChannel_t)( uint8_t channel, uint8_t value );

/**
 * \brief Signature of a function that handles configuration key/values.
 *
 * \attention The \p key string is not NULL-terminated! The length is stored in \p key_len!
 * \param key         Non-NULL terminated key string.
 * \param key_len     The size of \p key in characters.
 * \param values      The key's new value(s). This is a generic byte array of size \p values_size.
 * \param values_size Size of \p values in bytes.
 */
typedef void (*illuminatir_parse_setConfig_t)( const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size );

/**
 * \brief Parses a packet and calls \ref illuminatir_parse_setChannel_t or \ref illuminatir_parse_setConfig_t functions accordingly.
 *
 * \param packet         Pointer to a packet.
 * \param packet_size    Size of \p packet in bytes.
 * \param setChannelFunc Pointer to a function that is called when OffsetArray or ChannelValuePairs type payloads are parsed.
 * \param setConfigFunc  Pointer to a function that is called when Config type payloads are parsed.
 */
illuminatir_error_t illuminatir_parse( const uint8_t * packet, uint8_t packet_size, illuminatir_parse_setChannel_t setChannelFunc, illuminatir_parse_setConfig_t setConfigFunc );

/**
 * \brief Builds a packet of payload type OffsetArray.
 *
 * \param packet      Pointer to a buffer.
 * \param packet_size Size of \p packet buffer in bytes.
 * \param offset      The channel number of the first element in \p values.
 * \param values      Pointer an array of values.
 * \param values_size Size of \p values in bytes.
 */
illuminatir_error_t illuminatir_build_offsetArray( uint8_t * packet, uint8_t * packet_size, uint8_t offset, const uint8_t * values, uint8_t values_size );

/**
 * \brief Builds a packet of payload type Config.
 *
 * \param packet      Pointer to a buffer.
 * \param packet_size Size of \p packet buffer in bytes.
 * \param key         Key string.
 * \param key_len     The length of \p key in characters (NULL character excluded).
 * \param values      The key's value(s).
 * \param values_size Size of \p values in bytes.
 */
illuminatir_error_t illuminatir_build_config     ( uint8_t * packet, uint8_t * packet_size, const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size );

/**
 * @}
 */


/**
 * @defgroup CRC8 CRC8
 * \brief 8 bit CRC.
 *
 * Provides 8 bits of cyclic redundancy check calculation.
 * @{
 */

#define ILLUMINATIR_CRC8_INITIAL_SEED (0) ///< Initial seed. Use this as \p crc parameter in the first call to \ref illuminatir_crc8.

/**
 * \brief 8 bit CRC calculation.
 *
 * Calculates 8 bits of CRC over data.
 * \note For general use cases you would not need this function. It is however exported in case you need an 8 bit CRC and don't want to implement/include another one.
 * \param data      Pointer to data.
 * \param data_size Size of data.
 * \param crc       If data should be processed in chunks, this is the CRC of the previous chunk. Otherwise use \ref ILLUMINATIR_CRC8_INITIAL_SEED.
 */
uint8_t illuminatir_crc8( const uint8_t * data, size_t data_size, uint8_t crc );

/**
 * @}
 */


/**
 * @defgroup COBS COBS
 * \brief COBS encoding/decoding.
 *
 * Provides Consistent Overhead Byte Stuffing (COBS) encoding and decoding functions.
 * @{
 */

#define ILLUMINATIR_COBS_ENCODE_DST_MAXSIZE(SRC_SIZE) ((SRC_SIZE)+(((SRC_SIZE)+253U)/254U))    ///< The maximum encoded packet size to expect for \p SRC_SIZE of data.
#define ILLUMINATIR_COBS_DECODE_DST_MAXSIZE(SRC_SIZE) (((SRC_SIZE)==0) ? 0U : ((SRC_SIZE)-1U)) ///< The maximum decoded packet size to expect for \p SRC_SIZE of data.

/**
 * \brief COBS encode.
 * 
 * Encodes \p src using COBS.
 * \note For general use cases you would not need this function. It is however exported in case you want to encode other data as well.
 * \param dst      Pointer to destination buffer.
 * \param dst_size Size of destination buffer.
 * \param src      Pointer to source buffer.
 * \param src_size Size of source buffer.
 * \return The actual size of the encoded data in \p dst. 0 on error.
 */
size_t illuminatir_cobs_encode( uint8_t * dst, size_t dst_size, const uint8_t * src, size_t src_size );

/**
 * \brief COBS decode.
 * 
 * Decodes \p src using COBS.
 * \note For general use cases you would not need this function. It is however exported in case you want to decode other data as well.
 * \param dst      Pointer to destination buffer.
 * \param dst_size Size of destination buffer.
 * \param src      Pointer to source buffer.
 * \param src_size Size of source buffer.
 * \return The actual size of the decoded data in \p dst. 0 on error.
 */
size_t illuminatir_cobs_decode( uint8_t * dst, size_t dst_size, const uint8_t * src, size_t src_size );

illuminatir_error_t illuminatir_cobs_parse( const uint8_t * cobsPacket, uint8_t cobsPacket_size, illuminatir_parse_setChannel_t setChannelFunc, illuminatir_parse_setConfig_t setConfigFunc ); ///< A version of \ref illuminatir_parse for COBS encoded packets.
illuminatir_error_t illuminatir_cobs_build_offsetArray( uint8_t * cobsPacket, uint8_t * cobsPacket_size, uint8_t offset, const uint8_t * values, uint8_t values_size );                        ///< A version of \ref illuminatir_build_offsetArray building COBS encoded packets.
illuminatir_error_t illuminatir_cobs_build_config     ( uint8_t * cobsPacket, uint8_t * cobsPacket_size, const char * key, uint8_t key_len, const uint8_t * values, uint8_t values_size );     ///< A version of \ref illuminatir_build_config building COBS encoded packets.

/**
 * @}
 */


#endif
