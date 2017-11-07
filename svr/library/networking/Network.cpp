#include "Network.h"
#include <NetworkUtil.h>
#include <assert.h>


int WritePacket( const PacketInfo & info, 
                 Packet *packet,
                 uint8_t *buffer, 
                 int bufferSize, 
                 Object *header )
{
    assert( packet );
    assert( buffer );
    assert( bufferSize > 0 );
    assert( info.protocolId );

    const int numPacketTypes = PacketFactorySingleton::get_mutable_instance().GetNumPacketTypes();

    WriteStream stream( buffer, bufferSize );

    stream.SetContext( info.context );

    for ( int i = 0; i < info.prefixBytes; ++i )
    {
        uint8_t zero = 0;
        stream.SerializeBits( zero, 8 );
    }

    uint32_t crc32 = 0;

    if ( !info.rawFormat )
        stream.SerializeBits( crc32, 32 );

    if ( header )
    {
        if ( !header->SerializeWrite( stream ) )
            return 0;
    }

    int packetType = packet->GetType();

    assert( numPacketTypes > 0 );

    if ( numPacketTypes > 1 )
    {
        stream.SerializeInteger( packetType, 0, numPacketTypes - 1 );
    }

    if ( !packet->SerializeWrite( stream ) )
        return 0;

    //stream.SerializeCheck( "end of packet" );

    stream.Flush();

    if ( !info.rawFormat )
    {
        uint32_t network_protocolId = host_to_network( info.protocolId );
        crc32 = calculate_crc32( (uint8_t*) &network_protocolId, 4 );
        crc32 = calculate_crc32( buffer + info.prefixBytes, stream.GetBytesProcessed() - info.prefixBytes, crc32 );
        *((uint32_t*)(buffer+info.prefixBytes)) = host_to_network( crc32 );
    }

    if ( stream.GetError() )
        return 0;

    return stream.GetBytesProcessed();
}

Packet* ReadPacket( const PacketInfo & info, 
                    const uint8_t * buffer, 
                    int bufferSize, 
                    Object * header, 
                    int * errorCode )
{
    assert( buffer );
    assert( bufferSize > 0 );
    assert( info.protocolId != 0 );

    if ( errorCode )
        *errorCode = YOJIMBO_PROTOCOL_ERROR_NONE;

    ReadStream stream( buffer, bufferSize );

    stream.SetContext( info.context );

    for ( int i = 0; i < info.prefixBytes; ++i )
    {
        uint32_t dummy = 0;
        stream.SerializeBits( dummy, 8 );
    }

    uint32_t read_crc32 = 0;
    if ( !info.rawFormat )
    {
        stream.SerializeBits( read_crc32, 32 );

        uint32_t network_protocolId = host_to_network( info.protocolId );
        uint32_t crc32 = calculate_crc32( (const uint8_t*) &network_protocolId, 4 );
        uint32_t zero = 0;
        crc32 = calculate_crc32( (const uint8_t*) &zero, 4, crc32 );
        crc32 = calculate_crc32( buffer + info.prefixBytes + 4, bufferSize - 4 - info.prefixBytes, crc32 );

        if ( crc32 != read_crc32 )
        {
            printf( "corrupt packet. expected crc32 %x, got %x\n", crc32, read_crc32 );

            if ( errorCode )
                *errorCode = YOJIMBO_PROTOCOL_ERROR_CRC32_MISMATCH;
            return NULL;
        }
    }

    if ( header )
    {
        if ( !header->SerializeRead( stream ) )
        {
            if ( errorCode )
                *errorCode = YOJIMBO_PROTOCOL_ERROR_SERIALIZE_HEADER_FAILED;
            return NULL;
        }
    }

    int packetType = 0;

    const int numPacketTypes = PacketFactorySingleton::get_mutable_instance().GetNumPacketTypes();

    assert( numPacketTypes > 0 );

    if ( numPacketTypes > 1 )
    {
        if ( !stream.SerializeInteger( packetType, 0, numPacketTypes - 1 ) )
        {
            if ( errorCode )
                *errorCode = YOJIMBO_PROTOCOL_ERROR_INVALID_PACKET_TYPE;
            return NULL;
        }
    }

    Packet *packet = PacketFactorySingleton::get_mutable_instance().CreatePacket( packetType );
    if ( !packet )
    {
        if ( errorCode )
            *errorCode = YOJIMBO_PROTOCOL_ERROR_CREATE_PACKET_FAILED;
        return NULL;
    }

    if ( !packet->SerializeRead( stream ) )
    {
        if ( errorCode )
            *errorCode = YOJIMBO_PROTOCOL_ERROR_SERIALIZE_PACKET_FAILED;
        PacketFactorySingleton::get_mutable_instance().DestroyPacket( packet );
        return NULL;
    }

    if ( stream.GetError() )
    {
        if ( errorCode )
            *errorCode = stream.GetError();
        PacketFactorySingleton::get_mutable_instance().DestroyPacket( packet );
        return NULL;
    }

    return packet;
}