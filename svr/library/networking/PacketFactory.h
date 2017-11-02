#pragma once
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Packet.h"
#include "singleton.h"

enum TestPacketTypes
{
    TEST_PACKET_A,
    TEST_PACKET_B,
    TEST_PACKET_C,
    TEST_PACKET_NUM_TYPES
};

class PacketFactory
{        
    int m_numPacketTypes;

public:
   // CreatePacket();

    Packet* CreatePacket( int type );

    void DestroyPacket( Packet *packet );

    void SetNumPacketTypes(int num);
    
    int GetNumPacketTypes() const;
};
typedef Singleton<PacketFactory> PacketFactorySingleton;

class TestPacketA:public Packet
{
public:
    TestPacketA() : Packet( TEST_PACKET_A ){}
    ~TestPacketA(){}
};
