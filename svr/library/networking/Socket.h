#pragma once
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Address.h"

enum SocketType
{
    SOCKET_TYPE_IPV4,
    SOCKET_TYPE_IPV6
};

enum SocketError
{
    SOCKET_ERROR_NONE,
    SOCKET_ERROR_CREATE_FAILED,
    SOCKET_ERROR_SET_NON_BLOCKING_FAILED,
    SOCKET_ERROR_SOCKOPT_IPV6_ONLY_FAILED,
    SOCKET_ERROR_BIND_IPV4_FAILED,
    SOCKET_ERROR_BIND_IPV6_FAILED
};

typedef int SocketHandle;

class Socket
{
public:

    Socket( uint16_t port, SocketType type = SOCKET_TYPE_IPV6 );

    ~Socket();

    bool IsError() const;

    int GetError() const;

    bool SendPacket( const Address & address, const void * packetData, size_t packetBytes );

    int ReceivePacket( Address & from, void * packetData, int maxPacketSize );

private:

    int m_error;
    uint16_t m_port;
    SocketHandle m_socket;
};