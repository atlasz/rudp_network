#include "PacketFactory.h"

// PacketFactory::PacketFactory()
// {
//     m_numPacketTypes = TEST_PACKET_NUM_TYPES;
// }

Packet* PacketFactory::CreatePacket( int type )
{
    assert( type >= 0 );
    switch ( type )
    {
        case TEST_PACKET_A: return new TestPacketA();
        case TEST_PACKET_B: return new TestPacketB();
        case TEST_PACKET_C: return new TestPacketC();
        case RUDP_CMD_CONNECTION_START_REQ: return new ConnectionStartReq();
        case RUDP_CMD_CONNECTION_START_RSP: return new ConnectionStartRsp();
        case RUDP_CMD_CONNECTION_STOP_REQ: return new ConnectionStopReq();
        case RUDP_CMD_CONNECTION_STOP_RSP: return new ConnectionStopRsp();
        case RUDP_CMD_HEARTBEAT: return new ConnectionHeartBeat();
    }
    return NULL;
}

void PacketFactory::DestroyPacket( Packet* packet )
{
    if ( !packet )
        return;

    delete packet;
}

void PacketFactory::SetNumPacketTypes(int num)
{
    m_numPacketTypes = num;
}

int PacketFactory::GetNumPacketTypes() const
{
    return m_numPacketTypes;
}