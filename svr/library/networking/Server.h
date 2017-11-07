#pragma once
#include <stdlib.h>
#include <NetworkInterface.h>
#include <Address.h>
#include <PacketFactory.h>

const int MaxClients = 64;
const float ConnectionHeartBeatRate = 1.0f;

struct ServerClientData
{
    Address address;
    uint64_t clientId;
    double connectTime;
    double lastPacketSendTime;
    double lastPacketReceiveTime;

    ServerClientData()
    {
        clientId = 0;
        connectTime = 0.0;
        lastPacketSendTime = 0.0;
        lastPacketReceiveTime = 0.0;
    }
};

class Server
{
public:
	Server( NetworkInterface & networkInterface );
    ~Server();

    void SendPackets( double time );
    void ReceivePackets( double time );
    void SendPacketToConnectedClient( int clientIndex, Packet * packet, double time );

protected:

	void ResetClientState( int clientIndex );

private:
	NetworkInterface * m_networkInterface;                              // network interface for sending and receiving packets.

    int m_numConnectedClients;                                          // number of connected clients
    
    bool m_clientConnected[MaxClients];                                 // true if client n is connected
    
    uint64_t m_clientId[MaxClients];                                    // array of client id values per-client
    
    Address m_serverAddress;                                            // the external IP address of this server (what clients will be sending packets to)

    Address m_clientAddress[MaxClients];                                // array of client address values per-client
    
    ServerClientData m_clientData[MaxClients]; 
};