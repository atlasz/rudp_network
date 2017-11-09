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
    RUDP_CMD_DATA = 100,
    RUDP_CMD_CONNECTION_START_REQ = 101,
    RUDP_CMD_CONNECTION_START_RSP = 102,
    RUDP_CMD_CONNECTION_STOP_REQ = 103,
    RUDP_CMD_CONNECTION_STOP_RSP = 104,
    RUDP_CMD_HEARTBEAT = 105,
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

struct ConnectionHeartBeat : public Packet
{
    uint8_t cmd;
    uint16_t sid;

    ConnectionHeartBeat() : Packet( RUDP_CMD_HEARTBEAT ) {}

    template <typename Stream> bool Serialize( Stream & stream ) 
    { 
        serialize_bits(stream, cmd, 8);
        serialize_bits(stream, sid, 16);
        return true; 
    }

    SERIALIZE_FUNCTIONS();
};

struct ConnectionStartReq : public Packet
{
    uint8_t cmd;
    uint16_t sid;
    uint64_t uid;

    ConnectionStartReq() : Packet( RUDP_CMD_CONNECTION_START_REQ ) {}

    template <typename Stream> bool Serialize( Stream & stream ) 
    { 
        serialize_bits(stream, cmd, 8);
        serialize_bits(stream, sid, 16);
        serialize_uint64(stream, uid);
        return true; 
    }

    SERIALIZE_FUNCTIONS();
};

struct ConnectionStartRsp : public Packet
{
    uint8_t cmd;
    uint16_t sid;

    ConnectionStartRsp() : Packet( RUDP_CMD_CONNECTION_START_RSP ) {}

    template <typename Stream> bool Serialize( Stream & stream ) 
    { 
        serialize_bits(stream, cmd, 8);
        serialize_bits(stream, sid, 16);
        return true; 
    }

    SERIALIZE_FUNCTIONS();
};

struct ConnectionStopReq : public Packet
{
    uint8_t cmd;
    uint16_t sid;
    uint8_t reason;

    ConnectionStopReq() : Packet( RUDP_CMD_CONNECTION_STOP_REQ ) {}

    template <typename Stream> bool Serialize( Stream & stream ) 
    { 
        serialize_bits(stream, cmd, 8);
        serialize_bits(stream, sid, 16);
        serialize_bits(stream, reason, 8);
        return true; 
    }

    SERIALIZE_FUNCTIONS();
};

struct ConnectionStopRsp : public Packet
{
    uint8_t cmd;
    uint16_t sid;
    uint8_t reason;

    ConnectionStopRsp() : Packet( RUDP_CMD_CONNECTION_STOP_RSP ) {}

    template <typename Stream> bool Serialize( Stream & stream ) 
    { 
        serialize_bits(stream, cmd, 8);
        serialize_bits(stream, sid, 16);
        serialize_bits(stream, reason, 8);
        return true; 
    }

    SERIALIZE_FUNCTIONS();
};
