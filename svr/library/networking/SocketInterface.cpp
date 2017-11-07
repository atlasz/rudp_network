#include "SocketInterface.h"
#include <stdint.h>
#include <inttypes.h>

SocketInterface::SocketInterface( uint32_t protocolId,
                                  uint16_t socketPort, 
                                  SocketType socketType, 
                                  int maxPacketSize, 
                                  int sendQueueSize, 
                                  int receiveQueueSize )
{
    assert( protocolId != 0 );
    assert( sendQueueSize > 0 );
    assert( receiveQueueSize > 0 );

    m_context = NULL;
    
    m_socket = new Socket(socketPort, socketType );
    
    m_protocolId = protocolId;

    m_sendQueueSize = sendQueueSize;

    m_receiveQueueSize = receiveQueueSize;
    
    m_packetProcessor = new PacketProcessor(m_protocolId, maxPacketSize, m_context);
    
    // queue_reserve( m_sendQueue, sendQueueSize );
    // queue_reserve( m_receiveQueue, receiveQueueSize );

    const int numPacketTypes = PacketFactorySingleton::get_mutable_instance().GetNumPacketTypes();

    assert( numPacketTypes > 0 );

    memset( m_counters, 0, sizeof( m_counters ) );
}

SocketInterface::~SocketInterface()
{
    assert( m_socket );

    ClearSendQueue();
    ClearReceiveQueue();

    delete m_socket;

    m_socket = NULL;

    m_packetProcessor = NULL;
}

void SocketInterface::ClearSendQueue()
{
    // for ( int i = 0; i < (int) queue_size( m_sendQueue ); ++i )
    // {
    //     PacketEntry & entry = m_sendQueue[i];
    //     assert( entry.packet );
    //     assert( entry.address.IsValid() );
    //     m_packetFactory->DestroyPacket( entry.packet );
    // }

    // queue_clear( m_sendQueue );

	while (!m_sendQueue.empty())
	{
		PacketEntry & entry = m_sendQueue.front();
		assert( entry.packet );
        assert( entry.address.IsValid() );
        PacketFactorySingleton::get_mutable_instance().DestroyPacket( entry.packet );
		m_sendQueue.pop();
	}
}

void SocketInterface::ClearReceiveQueue()
{
    // for ( int i = 0; i < (int) queue_size( m_receiveQueue ); ++i )
    // {
    //     PacketEntry & entry = m_receiveQueue[i];
    //     assert( entry.packet );
    //     assert( entry.address.IsValid() );
    //     m_packetFactory->DestroyPacket( entry.packet );
    // }

    // queue_clear( m_receiveQueue );
	while (!m_receiveQueue.empty())
	{
		PacketEntry & entry = m_receiveQueue.front();
		assert( entry.packet );
        assert( entry.address.IsValid() );
        PacketFactorySingleton::get_mutable_instance().DestroyPacket( entry.packet );
		m_receiveQueue.pop();
	}
}

bool SocketInterface::IsError() const
{
    assert( m_socket );
    return m_socket->IsError();
}

int SocketInterface::GetError() const
{
    assert( m_socket );
    return m_socket->GetError();
}

Packet * SocketInterface::CreatePacket( int type )
{
    return PacketFactorySingleton::get_mutable_instance().CreatePacket( type );
}

void SocketInterface::DestroyPacket( Packet * packet )
{
    PacketFactorySingleton::get_mutable_instance().DestroyPacket( packet );
}

void SocketInterface::SendPacket( const Address & address, Packet * packet, uint64_t sequence )
{
    assert( packet );
    assert( address.IsValid() );

    if ( IsError() )
    {
        PacketFactorySingleton::get_mutable_instance().DestroyPacket( packet );
        return;
    }

    PacketEntry entry;
    entry.sequence = sequence;
    entry.address = address;
    entry.packet = packet;

    if ( m_sendQueue.size() >= (size_t)m_sendQueueSize )
    {
        m_counters[SOCKET_INTERFACE_COUNTER_SEND_QUEUE_OVERFLOW]++;
        PacketFactorySingleton::get_mutable_instance().DestroyPacket( packet );
        return;
    }

    m_sendQueue.push(entry);

    m_counters[SOCKET_INTERFACE_COUNTER_PACKETS_SENT]++;
}

Packet * SocketInterface::ReceivePacket( Address & from, uint64_t * /*sequence*/ )
{
    if ( IsError() )
        return NULL;

    if ( m_receiveQueue.empty())
        return NULL;

    const PacketEntry & entry = m_receiveQueue.front();

    m_receiveQueue.pop();

    assert( entry.packet );
    assert( entry.address.IsValid() );

    from = entry.address;

    m_counters[SOCKET_INTERFACE_COUNTER_PACKETS_RECEIVED]++;

    return entry.packet;
}

void SocketInterface::WritePackets( double /*time*/ )
{
    assert( m_socket );
    assert( m_packetProcessor );

    while ( !m_sendQueue.empty() )
    {
        const PacketEntry & entry = m_sendQueue.front();

        assert( entry.packet );
        assert( entry.address.IsValid() );

        m_sendQueue.pop();

        int packetBytes;

        const uint8_t * packetData = m_packetProcessor->WritePacket( entry.packet, entry.sequence, packetBytes);

        if ( !packetData )
        {
            switch ( m_packetProcessor->GetError() )
            {
                case PACKET_PROCESSOR_ERROR_KEY_IS_NULL:                m_counters[SOCKET_INTERFACE_COUNTER_ENCRYPTION_MAPPING_FAILURES]++;         break;
                case PACKET_PROCESSOR_ERROR_ENCRYPT_FAILED:             m_counters[SOCKET_INTERFACE_COUNTER_ENCRYPT_PACKET_FAILURES]++;             break;
                case PACKET_PROCESSOR_ERROR_WRITE_PACKET_FAILED:        m_counters[SOCKET_INTERFACE_COUNTER_WRITE_PACKET_FAILURES]++;               break;

                default:
                    break;
            }

            continue;
        }

        m_socket->SendPacket( entry.address, packetData, packetBytes );

        PacketFactorySingleton::get_mutable_instance().DestroyPacket( entry.packet );

        m_counters[SOCKET_INTERFACE_COUNTER_PACKETS_WRITTEN]++;

        m_counters[SOCKET_INTERFACE_COUNTER_UNENCRYPTED_PACKETS_WRITTEN]++;
    }
}

void SocketInterface::ReadPackets( double /*time*/ )
{
    assert( m_socket );
    assert( m_packetProcessor );

    const int maxPacketSize = GetMaxPacketSize();

    uint8_t * packetBuffer = (uint8_t*) alloca( maxPacketSize );

    while ( true )
    {
        Address address;
        int packetBytes = m_socket->ReceivePacket( address, packetBuffer, maxPacketSize );
        if ( !packetBytes )
            break;

        assert( packetBytes > 0 );

        if ( m_receiveQueue.size() == (size_t) m_receiveQueueSize )
        {
            m_counters[SOCKET_INTERFACE_COUNTER_RECEIVE_QUEUE_OVERFLOW]++;
            break;
        }

        uint64_t sequence;

        Packet * packet = m_packetProcessor->ReadPacket( packetBuffer, sequence, packetBytes );

        if ( !packet )
        {
            switch ( m_packetProcessor->GetError() )
            {
                case PACKET_PROCESSOR_ERROR_KEY_IS_NULL:                m_counters[SOCKET_INTERFACE_COUNTER_ENCRYPTION_MAPPING_FAILURES]++;        break;
                case PACKET_PROCESSOR_ERROR_DECRYPT_FAILED:             m_counters[SOCKET_INTERFACE_COUNTER_ENCRYPT_PACKET_FAILURES]++;            break;
                case PACKET_PROCESSOR_ERROR_PACKET_TOO_SMALL:           m_counters[SOCKET_INTERFACE_COUNTER_DECRYPT_PACKET_FAILURES]++;            break;
                case PACKET_PROCESSOR_ERROR_READ_PACKET_FAILED:         m_counters[SOCKET_INTERFACE_COUNTER_READ_PACKET_FAILURES]++;               break;

                default:
                    break;
            }

            continue;
        }

        PacketEntry entry;
        entry.sequence = 0;
        entry.packet = packet;
        entry.address = address;

        m_receiveQueue.push( entry );

        m_counters[SOCKET_INTERFACE_COUNTER_PACKETS_READ]++;

        m_counters[SOCKET_INTERFACE_COUNTER_UNENCRYPTED_PACKETS_READ]++;
    }
}

int SocketInterface::GetMaxPacketSize() const 
{
    return m_packetProcessor->GetMaxPacketSize();
}

void SocketInterface::SetContext( void * context )
{
    m_context = context;
}

uint64_t SocketInterface::GetCounter( int index ) const
{
    assert( index >= 0 );
    assert( index < SOCKET_INTERFACE_COUNTER_NUM_COUNTERS );
    return m_counters[index];
}
