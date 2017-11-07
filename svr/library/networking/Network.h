#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <Packet.h>
#include <PacketFactory.h>

class Object;


#define YOJIMBO_PROTOCOL_ERROR_NONE                         0
#define YOJIMBO_PROTOCOL_ERROR_CRC32_MISMATCH               1
#define YOJIMBO_PROTOCOL_ERROR_INVALID_PACKET_TYPE          2
#define YOJIMBO_PROTOCOL_ERROR_PACKET_TYPE_NOT_ALLOWED      3
#define YOJIMBO_PROTOCOL_ERROR_CREATE_PACKET_FAILED         4
#define YOJIMBO_PROTOCOL_ERROR_SERIALIZE_HEADER_FAILED      5
#define YOJIMBO_PROTOCOL_ERROR_SERIALIZE_PACKET_FAILED      6
#define YOJIMBO_PROTOCOL_ERROR_SERIALIZE_CHECK_FAILED       7
#define YOJIMBO_PROTOCOL_ERROR_STREAM_OVERFLOW              8
#define YOJIMBO_PROTOCOL_ERROR_STREAM_ABORTED               9

int WritePacket( const PacketInfo & info, 
                 Packet *packet, 
                 uint8_t *buffer, 
                 int bufferSize, 
                 Object *header = NULL );

Packet * ReadPacket( const PacketInfo & info, 
                     const uint8_t *buffer, 
                     int bufferSize, 
                     Object *header = NULL, 
                     int *errorCode = NULL );