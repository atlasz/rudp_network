#pragma once

#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue> 

#include <PacketProcessor.h>
#include <Socket.h>
#include <PacketFactory.h>
#include <NetworkInterface.h>

class Packet;
class Address;

enum SocketInterfaceCounter
{
    SOCKET_INTERFACE_COUNTER_PACKETS_SENT,
    SOCKET_INTERFACE_COUNTER_PACKETS_RECEIVED,
    SOCKET_INTERFACE_COUNTER_PACKETS_READ,
    SOCKET_INTERFACE_COUNTER_PACKETS_WRITTEN,
    SOCKET_INTERFACE_COUNTER_SEND_QUEUE_OVERFLOW,
    SOCKET_INTERFACE_COUNTER_RECEIVE_QUEUE_OVERFLOW,
    SOCKET_INTERFACE_COUNTER_READ_PACKET_FAILURES,
    SOCKET_INTERFACE_COUNTER_WRITE_PACKET_FAILURES,
    SOCKET_INTERFACE_COUNTER_ENCRYPT_PACKET_FAILURES,
    SOCKET_INTERFACE_COUNTER_DECRYPT_PACKET_FAILURES,
    SOCKET_INTERFACE_COUNTER_ENCRYPTED_PACKETS_READ,
    SOCKET_INTERFACE_COUNTER_ENCRYPTED_PACKETS_WRITTEN,
    SOCKET_INTERFACE_COUNTER_UNENCRYPTED_PACKETS_READ,
    SOCKET_INTERFACE_COUNTER_UNENCRYPTED_PACKETS_WRITTEN,
    SOCKET_INTERFACE_COUNTER_ENCRYPTION_MAPPING_FAILURES,
    SOCKET_INTERFACE_COUNTER_NUM_COUNTERS
};

class SocketInterface : public NetworkInterface
{
    void * m_context;

    uint32_t m_protocolId;
    int m_sendQueueSize;
    int m_receiveQueueSize;

    Socket * m_socket;
    PacketProcessor * m_packetProcessor;

    struct PacketEntry
    {
        uint64_t sequence;
        Address address;
        Packet * packet;
    };

    std::queue<PacketEntry> m_sendQueue;
    std::queue<PacketEntry> m_receiveQueue;

    uint64_t m_counters[SOCKET_INTERFACE_COUNTER_NUM_COUNTERS];

protected:

    void ClearSendQueue();

    void ClearReceiveQueue();

public:

    SocketInterface( uint32_t protocolId,
                     uint16_t socketPort, 
                     SocketType socketType = SOCKET_TYPE_IPV6, 
                     int maxPacketSize = 4 * 1024,
                     int sendQueueSize = 1024,
                     int receiveQueueSize = 1024 );

    ~SocketInterface();

    bool IsError() const;

    int GetError() const;

    Packet * CreatePacket( int type );

    void DestroyPacket( Packet * packet );

    void SendPacket( const Address & address, Packet * packet, uint64_t sequence = 0 );

    Packet * ReceivePacket( Address & from, uint64_t * sequence = NULL );

    void WritePackets( double time );

    void ReadPackets( double time );

    int GetMaxPacketSize() const;

    void SetContext( void * context );

    uint64_t GetCounter( int index ) const;
};