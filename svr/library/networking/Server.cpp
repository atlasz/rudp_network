#include "Server.h"

Server::Server(NetworkInterface & networkInterface)
{
	m_networkInterface = &networkInterface;
	m_numConnectedClients = 0;
	for ( int i = 0; i < MaxClients; ++i )
	{
		ResetClientState( i );
	}
}

Server::~Server()
{

}

void Server::ResetClientState( int clientIndex )
{
    assert( clientIndex >= 0 );
    assert( clientIndex < MaxClients );
    m_clientConnected[clientIndex] = false;
    m_clientId[clientIndex] = 0;
    m_clientAddress[clientIndex] = Address();
    m_clientData[clientIndex] = ServerClientData();
}

void Server::SendPackets( double time )
{
    for ( int i = 0; i < MaxClients; ++i )
    {
        if ( !m_clientConnected[i] )
            continue;

        if ( m_clientData[i].lastPacketSendTime + ConnectionHeartBeatRate > time )
            return;

        ConnectionHeartBeatPacket * packet = (ConnectionHeartBeatPacket*) m_networkInterface->CreatePacket( PACKET_CONNECTION_HEARTBEAT );

        SendPacketToConnectedClient( i, packet, time );
    }
}

void Server::ReceivePackets( double time )
{
    while ( true )
    {
        Address address;
        Packet *packet = m_networkInterface->ReceivePacket( address );
        if ( !packet )
            break;
        
        switch ( packet->GetType() )
        {
            case PACKET_CONNECTION_REQUEST:
               // ProcessConnectionRequest( *(ConnectionRequestPacket*)packet, address, time );
                break;

            case PACKET_CONNECTION_RESPONSE:
                //ProcessConnectionResponse( *(ConnectionResponsePacket*)packet, address, time );
                break;

            case PACKET_CONNECTION_HEARTBEAT:
               // ProcessConnectionHeartBeat( *(ConnectionHeartBeatPacket*)packet, address, time );
                break;

            case PACKET_CONNECTION_DISCONNECT:
               // ProcessConnectionDisconnect( *(ConnectionDisconnectPacket*)packet, address, time );
                break;

            default:
                break;
        }

        m_networkInterface->DestroyPacket( packet );
    }
}

void Server::SendPacketToConnectedClient( int clientIndex, Packet * packet, double time )
{
    assert( packet );
    assert( clientIndex >= 0 );
    assert( clientIndex < MaxClients );
    assert( m_clientConnected[clientIndex] );
    m_clientData[clientIndex].lastPacketSendTime = time;
    m_networkInterface->SendPacket( m_clientAddress[clientIndex], packet );
}