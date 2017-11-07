#pragma once
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <Packet.h>

enum PacketProcessErrors
{
    PACKET_PROCESSOR_ERROR_NONE,                        // everything is fine
    PACKET_PROCESSOR_ERROR_KEY_IS_NULL,                 // we needed an encryption/decryption key but it was passed in as NULL
    PACKET_PROCESSOR_ERROR_PACKET_TOO_SMALL,            // an encrypted packet was discarded because it was too short to possibly contain valid data
    PACKET_PROCESSOR_ERROR_WRITE_PACKET_FAILED,         // failed to write packet
    PACKET_PROCESSOR_ERROR_READ_PACKET_FAILED,          // failed to read packet
    PACKET_PROCESSOR_ERROR_ENCRYPT_FAILED,              // encrypt packet failed
    PACKET_PROCESSOR_ERROR_DECRYPT_FAILED,              // decrypt packet failed
};

class PacketProcessor
{
public:

    PacketProcessor(uint32_t protocolId, int maxPacketSize, void * context = NULL );

    ~PacketProcessor();

    const uint8_t * WritePacket( Packet * packet, uint64_t sequence, int & packetBytes);

    Packet * ReadPacket( const uint8_t * packetData,  uint64_t & sequence, int packetBytes);

    int GetMaxPacketSize() const { return m_maxPacketSize; }

    int GetError() const { return m_error; }

private:

    uint32_t m_protocolId;

    int m_error;
    int m_maxPacketSize;
    int m_absoluteMaxPacketSize;
    
    uint8_t * m_packetBuffer;
    uint8_t * m_scratchBuffer;

    void * m_context;
};