#pragma once
#include <Serialization.h>

class Packet : public Object
{
    int type;

public:
    
    Packet( int _type ) : type(_type) {}

    virtual ~Packet() {}

    int GetType() const { return type; }

private:

    Packet( const Packet & other );
    Packet & operator = ( const Packet & other );
};

struct PacketInfo
{
    bool rawFormat;                             // if true packets are written in "raw" format without crc32 (useful for encrypted packets).
    int prefixBytes;                            // prefix this number of bytes when reading and writing packets. stick your own data there.
    uint32_t protocolId;                        // protocol id that distinguishes your protocol from other packets sent over UDP.
    const uint8_t * allowedPacketTypes;         // array of allowed packet types. if a packet type is not allowed the serialize read or write will fail.
    void * context;                             // context for the packet serialization (optional, pass in NULL)

    PacketInfo()
    {
        rawFormat = false;
        prefixBytes = 0;
        protocolId = 0;
        allowedPacketTypes = NULL;
        context = NULL;
    }
};