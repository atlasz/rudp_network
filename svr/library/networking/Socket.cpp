#include "Socket.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

Socket::Socket( uint16_t port, SocketType type )
{
    m_error = SOCKET_ERROR_NONE;

    // create socket

    m_socket = socket( ( type == SOCKET_TYPE_IPV6 ) ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    if ( m_socket <= 0 )
    {
        printf( "create socket failed: %s\n", strerror( errno ) );
        m_error = SOCKET_ERROR_CREATE_FAILED;
        return;
    }

    // force IPv6 only if necessary

    if ( type == SOCKET_TYPE_IPV6 )
    {
        int yes = 1;
        if ( setsockopt( m_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&yes, sizeof(yes) ) != 0 )
        {
            printf( "failed to set ipv6 only sockopt\n" );
            m_error = SOCKET_ERROR_SOCKOPT_IPV6_ONLY_FAILED;
            return;
        }
    }

    // bind to port

    if ( type == SOCKET_TYPE_IPV6 )
    {
        sockaddr_in6 sock_address;
        memset( &sock_address, 0, sizeof( sockaddr_in6 ) );
        sock_address.sin6_family = AF_INET6;
        sock_address.sin6_addr = in6addr_any;
        sock_address.sin6_port = htons( port );

        if ( ::bind( m_socket, (const sockaddr*) &sock_address, sizeof(sock_address) ) < 0 )
        {
            printf( "bind socket failed (ipv6) - %s\n", strerror( errno ) );
            m_socket = SOCKET_ERROR_BIND_IPV6_FAILED;
            return;
        }
    }
    else
    {
        sockaddr_in sock_address;
        sock_address.sin_family = AF_INET;
        sock_address.sin_addr.s_addr = INADDR_ANY;
        sock_address.sin_port = htons( port );

        if ( ::bind( m_socket, (const sockaddr*) &sock_address, sizeof(sock_address) ) < 0 )
        {
            printf( "bind socket failed (ipv4) - %s\n", strerror( errno ) );
            m_error = SOCKET_ERROR_BIND_IPV4_FAILED;
            return;
        }
    }

    // todo: get the actual port we bound to in the case of passing in port 0 we must ask the OS

    m_port = port;

    // set non-blocking io

    #if NETWORK2_PLATFORM == NETWORK2_PLATFORM_MAC || NETWORK2_PLATFORM == NETWORK2_PLATFORM_UNIX

        int nonBlocking = 1;
        if ( fcntl( m_socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
        {
            printf( "failed to make socket non-blocking\n" );
            m_error = SOCKET_ERROR_SET_NON_BLOCKING_FAILED;
            return;
        }
    
    #elif NETWORK2_PLATFORM == NETWORK2_PLATFORM_WINDOWS

        DWORD nonBlocking = 1;
        if ( ioctlsocket( m_socket, FIONBIO, &nonBlocking ) != 0 )
        {
            printf( "failed to make socket non-blocking\n" );
            m_error = SOCKET_ERROR_SET_NON_BLOCKING_FAILED;
            return;
        }

    #else

        #error unsupported platform

    #endif
}

Socket::~Socket()
{
    if ( m_socket != 0 )
    {
        #if NETWORK2_PLATFORM == NETWORK2_PLATFORM_MAC || NETWORK2_PLATFORM == NETWORK2_PLATFORM_UNIX
        close( m_socket );
        #elif NETWORK2_PLATFORM == NETWORK2_PLATFORM_WINDOWS
        closesocket( m_socket );
        #else
        #error unsupported platform
        #endif
        m_socket = 0;
    }
}

bool Socket::IsError() const
{
    return m_error != SOCKET_ERROR_NONE;
}

int Socket::GetError() const
{
    return m_error;
}

bool Socket::SendPacket( const Address & address, const void * packetData, size_t packetBytes )
{
    assert( packetData );
    assert( packetBytes > 0 );
    assert( address.IsValid() );
    assert( m_socket );
    assert( !IsError() );

    bool result = false;

    if ( address.GetType() == ADDRESS_IPV6 )
    {
        sockaddr_in6 socket_address;
        memset( &socket_address, 0, sizeof( socket_address ) );
        socket_address.sin6_family = AF_INET6;
        socket_address.sin6_port = htons( address.GetPort() );
        memcpy( &socket_address.sin6_addr, address.GetAddress6(), sizeof( socket_address.sin6_addr ) );
        size_t sent_bytes = sendto( m_socket, (const char*)packetData, (int) packetBytes, 0, (sockaddr*)&socket_address, sizeof( sockaddr_in6 ) );
        result = sent_bytes == packetBytes;
    }
    else if ( address.GetType() == ADDRESS_IPV4 )
    {
        sockaddr_in socket_address;
        memset( &socket_address, 0, sizeof( socket_address ) );
        socket_address.sin_family = AF_INET;
        socket_address.sin_addr.s_addr = address.GetAddress4();
        socket_address.sin_port = htons( (unsigned short) address.GetPort() );
        size_t sent_bytes = sendto( m_socket, (const char*)packetData, (int) packetBytes, 0, (sockaddr*)&socket_address, sizeof(sockaddr_in) );
        result = sent_bytes == packetBytes;
    }

    if ( !result )
    {
        printf( "sendto failed: %s\n", strerror( errno ) );
    }

    return result;
}

int Socket::ReceivePacket( Address & from, void * packetData, int maxPacketSize )
{
    assert( m_socket );
    assert( packetData );
    assert( maxPacketSize > 0 );
    
    sockaddr_storage sockaddr_from;
    socklen_t fromLength = sizeof( sockaddr_from );

    int result = recvfrom( m_socket, (char*)packetData, maxPacketSize, 0, (sockaddr*)&sockaddr_from, &fromLength );

    if ( result <= 0 )
    {
        if ( errno == EAGAIN )
            return 0;

        printf( "recvfrom failed: %s\n", strerror( errno ) );

        return 0;
    }

    from = Address( sockaddr_from );

    assert( result >= 0 );

    const int bytesRead = result;

    return bytesRead;
}