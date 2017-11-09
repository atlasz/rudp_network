#pragma once
#include <stdlib.h>
#include <Address.h>
#include <PacketFactory.h>
#include <Socket.h>
#include <Session.h>

const int MaxClients = 64;
const float ConnectionHeartBeatRate = 1.0f;

class Server
{
	static const size_t kMaxPkgLength = 500;
public:
	Server();
    ~Server();

   	void Tick();
private:
    //void SendPackets( double time );
    int ReceivePackets();

    void ProcessStart(const Address& from, uint8_t* buffer, int len);

private:
	Socket* m_svrSocket;
};