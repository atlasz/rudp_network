#pragma once
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Packet.h"
#include <singleton.h>
#include <Serialization.h>

enum TestPacketTypes
{
    TEST_PACKET_A,
    TEST_PACKET_B,
    TEST_PACKET_C,
    PACKET_CONNECTION_HEARTBEAT,
    PACKET_CONNECTION_REQUEST,
    PACKET_CONNECTION_RESPONSE,
    PACKET_CONNECTION_DISCONNECT,
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

struct TestPacketA : public Packet
{
    int a,b,c;

    TestPacketA() : Packet( TEST_PACKET_A )
    {
        a = 1;
        b = 2;
        c = 3;        
    }

    template <typename Stream> bool Serialize( Stream & stream )
    {
        serialize_int( stream, a, -10, 10 );
        serialize_int( stream, b, -20, 20 );
        serialize_int( stream, c, -30, 30 );
        return true;
    }

    SERIALIZE_FUNCTIONS();
};

struct TestPacketB : public Packet
{
    int x,y;

    TestPacketB() : Packet( TEST_PACKET_B )
    {
        x = 0;
        y = 1;
    }

    template <typename Stream> bool Serialize( Stream & stream )
    {
        serialize_int( stream, x, -5, +5 );
        serialize_int( stream, y, -5, +5 );
        return true;
    }

    SERIALIZE_FUNCTIONS();
};

struct TestPacketC : public Packet
{
    uint8_t data[16];

    TestPacketC() : Packet( TEST_PACKET_C )
    {
        for ( int i = 0; i < (int) sizeof( data ); ++i )
            data[i] = (uint8_t) i;
    }

    template <typename Stream> bool Serialize( Stream & stream )
    {
        for ( int i = 0; i < (int) sizeof( data ); ++i )
            serialize_int( stream, data[i], 0, 255 );
        return true;
    }

    SERIALIZE_FUNCTIONS();
};

struct ConnectionHeartBeatPacket : public Packet
{
    ConnectionHeartBeatPacket() : Packet( PACKET_CONNECTION_HEARTBEAT ) {}

    template <typename Stream> bool Serialize( Stream & /*stream*/ ) { return true; }

    SERIALIZE_FUNCTIONS();
};

struct ProcessConnectionRequest : public Packet
{
    ProcessConnectionRequest() : Packet( PACKET_CONNECTION_REQUEST ) {}

    template <typename Stream> bool Serialize( Stream & /*stream*/ ) { return true; }

    SERIALIZE_FUNCTIONS();
};

struct ProcessConnectionResponse : public Packet
{
    ProcessConnectionResponse() : Packet( PACKET_CONNECTION_RESPONSE ) {}

    template <typename Stream> bool Serialize( Stream & /*stream*/ ) { return true; }

    SERIALIZE_FUNCTIONS();
};

struct ProcessConnectionDisconnect : public Packet
{
    ProcessConnectionDisconnect() : Packet( PACKET_CONNECTION_DISCONNECT ) {}

    template <typename Stream> bool Serialize( Stream & /*stream*/ ) { return true; }

    SERIALIZE_FUNCTIONS();
};
