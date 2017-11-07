#include "PacketProcessor.h"
#include <Network.h>

const int MaxPrefixBytes = 9;

PacketProcessor::PacketProcessor(uint32_t protocolId, int maxPacketSize, void * context )
{
    m_protocolId = protocolId;

    m_error = PACKET_PROCESSOR_ERROR_NONE;

    m_maxPacketSize = maxPacketSize + ( ( maxPacketSize % 4 ) ? ( 4 - ( maxPacketSize % 4 ) ) : 0 );
    
    assert( m_maxPacketSize % 4 == 0 );
    assert( m_maxPacketSize >= maxPacketSize );

    m_absoluteMaxPacketSize = maxPacketSize + MaxPrefixBytes;

    m_context = context;

    m_packetBuffer = new uint8_t[m_absoluteMaxPacketSize];
    m_scratchBuffer = new uint8_t[m_absoluteMaxPacketSize];
}

PacketProcessor::~PacketProcessor()
{
    delete [] m_packetBuffer;
    delete [] m_scratchBuffer;

    m_packetBuffer = NULL;
    m_scratchBuffer = NULL;
}


const uint8_t * PacketProcessor::WritePacket( Packet * packet, uint64_t sequence, int & packetBytes)
{
    m_error = PACKET_PROCESSOR_ERROR_NONE;

    PacketInfo info;
    info.context = m_context;
    info.protocolId = m_protocolId;
    info.prefixBytes = 1;

    packetBytes = ::WritePacket( info, packet, m_packetBuffer, m_maxPacketSize );

    if ( packetBytes <= 0 )
    {
        m_error = PACKET_PROCESSOR_ERROR_WRITE_PACKET_FAILED;
        return NULL;
    }

    assert( packetBytes <= m_maxPacketSize );

    return m_packetBuffer;
}

Packet * PacketProcessor::ReadPacket( const uint8_t * packetData, uint64_t & sequence, int packetBytes )
{
    m_error = PACKET_PROCESSOR_ERROR_NONE;

    //const uint8_t prefixByte = packetData[0];

    PacketInfo info;
    info.context = m_context;
    info.protocolId = m_protocolId;
    info.allowedPacketTypes = NULL;//TODO
    info.prefixBytes = 1;

    sequence = 0;

    int readError;
    Packet * packet = ::ReadPacket( info, packetData, packetBytes, NULL, &readError );

    if ( !packet )
    {
        m_error = PACKET_PROCESSOR_ERROR_READ_PACKET_FAILED;
        return NULL;
    }
    
    return packet;
}