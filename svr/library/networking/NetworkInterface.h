#pragma once

class NetworkInterface
{
public:

    virtual ~NetworkInterface() {}

    virtual Packet * CreatePacket( int type ) = 0;

    virtual void DestroyPacket( Packet * packet ) = 0;

    virtual void SendPacket( const Address & address, Packet * packet, uint64_t sequence = 0 ) = 0;

    virtual Packet * ReceivePacket( Address & from, uint64_t * sequence = NULL ) = 0;

    virtual void WritePackets( double time ) = 0;

    virtual void ReadPackets( double time ) = 0;

    virtual int GetMaxPacketSize() const = 0;

    virtual void SetContext( void * context ) = 0;
};