#include <iostream>
#include <Address.h>
#include <Socket.h>
#include <PacketFactory.h>
#include <BitReader.h>
#include <BitWriter.h>
#include <Serialization.h>

using namespace std;
/*
bitpack
*/
static void CheckHandler( const char * condition, 
                          const char * function,
                          const char * file,
                          int line )
{
    printf( "check failed: ( %s ), function %s, file %s, line %d\n", condition, function, file, line );
#ifndef NDEBUG
    #if defined( __GNUC__ )
        __builtin_trap();
    #elif defined( _MSC_VER )
        __debugbreak();
    #endif
#endif
    exit( 1 );
}

#define check( condition )                                                     \
do                                                                             \
{                                                                              \
    if ( !(condition) )                                                        \
    {                                                                          \
        CheckHandler( #condition, __FUNCTION__, __FILE__, __LINE__ );          \
    }                                                                          \
} while(0)


void test_bitpacker()
{
    printf( "test_bitpacker\n" );

    const int BufferSize = 256;

    uint8_t buffer[256];

    BitWriter writer( buffer, BufferSize );

    check( writer.GetData() == buffer );
    check( writer.GetTotalBytes() == BufferSize );
    check( writer.GetBitsWritten() == 0 );
    check( writer.GetBytesWritten() == 0 );
    check( writer.GetBitsAvailable() == BufferSize * 8 );

    writer.WriteBits( 0, 1 );
    writer.WriteBits( 1, 1 );
    writer.WriteBits( 10, 8 );
    writer.WriteBits( 255, 8 );
    writer.WriteBits( 1000, 10 );
    writer.WriteBits( 50000, 16 );
    writer.WriteBits( 9999999, 32 );
    writer.FlushBits();

    const int bitsWritten = 1 + 1 + 8 + 8 + 10 + 16 + 32;

    check( writer.GetBytesWritten() == 10 );
    check( writer.GetTotalBytes() == BufferSize );
    check( writer.GetBitsWritten() == bitsWritten );
    check( writer.GetBitsAvailable() == BufferSize * 8 - bitsWritten );

    const int bytesWritten = writer.GetBytesWritten();

    check( bytesWritten == 10 );

    memset( buffer + bytesWritten, 0, BufferSize - bytesWritten );

    BitReader reader( buffer, bytesWritten );

    check( reader.GetBitsRead() == 0 );
    check( reader.GetBitsRemaining() == bytesWritten * 8 );

    uint32_t a = reader.ReadBits( 1 );
    uint32_t b = reader.ReadBits( 1 );
    uint32_t c = reader.ReadBits( 8 );
    uint32_t d = reader.ReadBits( 8 );
    uint32_t e = reader.ReadBits( 10 );
    uint32_t f = reader.ReadBits( 16 );
    uint32_t g = reader.ReadBits( 32 );

    check( a == 0 );
    check( b == 1 );
    check( c == 10 );
    check( d == 255 );
    check( e == 1000 );
    check( f == 50000 );
    check( g == 9999999 );

    check( reader.GetBitsRead() == bitsWritten );
    check( reader.GetBitsRemaining() == bytesWritten * 8 - bitsWritten );
}

/*
serialization
*/

const int MaxItems = 11;

struct TestData
{
    int a,b,c;
    uint32_t d : 8;
    uint32_t e : 8;
    uint32_t f : 8;
    bool g;
    int numItems;
    int items[MaxItems];
    float float_value;
    double double_value;
    uint64_t uint64_value;
    uint8_t bytes[17];
    char string[64];
};

struct TestContext
{
    int min;
    int max;
};

struct TestObject : public Object
{
    TestData data;

    TestObject()
    {
        memset( &data, 0, sizeof( data ) );
    }

    void Init()
    {
        data.a = 1;
        data.b = -2;
        data.c = 150;
        data.d = 55;
        data.e = 255;
        data.f = 127;
        data.g = true;

        data.numItems = MaxItems / 2;
        for ( int i = 0; i < data.numItems; ++i )
            data.items[i] = i + 10;     

        data.float_value = 3.1415926f;
        data.double_value = 1 / 3.0;   
        data.uint64_value = 0x1234567898765432L;

        for ( int i = 0; i < (int) sizeof( data.bytes ); ++i )
            data.bytes[i] = rand() % 255;

        strcpy( data.string, "hello world!" );
    }

    template <typename Stream> bool Serialize( Stream & stream )
    {
        const TestContext & context = *(const TestContext*) stream.GetContext();

        serialize_int( stream, data.a, context.min, context.max );
        serialize_int( stream, data.b, context.min, context.max );

        serialize_int( stream, data.c, -100, 10000 );

        serialize_bits( stream, data.d, 6 );
        serialize_bits( stream, data.e, 8 );
        serialize_bits( stream, data.f, 7 );

        serialize_align( stream );

        serialize_bool( stream, data.g );

        serialize_check( stream, "test object serialize check" );

        serialize_int( stream, data.numItems, 0, MaxItems - 1 );
        for ( int i = 0; i < data.numItems; ++i )
            serialize_bits( stream, data.items[i], 8 );

        serialize_float( stream, data.float_value );

        serialize_double( stream, data.double_value );

        serialize_uint64( stream, data.uint64_value );

        serialize_bytes( stream, data.bytes, sizeof( data.bytes ) );

        serialize_string( stream, data.string, sizeof( data.string ) );

        serialize_check( stream, "end of test object" );

        return true;
    }

    SERIALIZE_FUNCTIONS();

    bool operator == ( const TestObject & other ) const
    {
        return memcmp( &data, &other.data, sizeof( TestData ) ) == 0;
    }

    bool operator != ( const TestObject & other ) const
    {
        return ! ( *this == other );
    }
};

void test_stream()
{
    printf( "test_stream\n" );

    const int BufferSize = 1024;

    uint8_t buffer[BufferSize];

    TestContext context;
    context.min = -10;
    context.max = +10;

    WriteStream writeStream( buffer, BufferSize );

    TestObject writeObject;
    writeObject.Init();
    writeStream.SetContext( &context );
    writeObject.SerializeWrite( writeStream );
    writeStream.Flush();

    const int bytesWritten = writeStream.GetBytesProcessed();

    memset( buffer + bytesWritten, 0, BufferSize - bytesWritten );

    TestObject readObject;
    ReadStream readStream( buffer, bytesWritten );
    readStream.SetContext( &context );
    readObject.SerializeRead( readStream );

    check( readObject == writeObject );
}

void test_packets()
{
    printf( "test packets\n" );

    TestPacketA * a = (TestPacketA*) PacketFactorySingleton::get_mutable_instance().CreatePacket( TEST_PACKET_A );
    TestPacketB * b = (TestPacketB*) PacketFactorySingleton::get_mutable_instance().CreatePacket( TEST_PACKET_B );
    TestPacketC * c = (TestPacketC*) PacketFactorySingleton::get_mutable_instance().CreatePacket( TEST_PACKET_C );

    check( a );
    check( b );
    check( c );

    check( a->GetType() == TEST_PACKET_A );
    check( b->GetType() == TEST_PACKET_B );
    check( c->GetType() == TEST_PACKET_C );

    PacketFactorySingleton::get_mutable_instance().DestroyPacket( a );
    PacketFactorySingleton::get_mutable_instance().DestroyPacket( b );
    PacketFactorySingleton::get_mutable_instance().DestroyPacket( c );
}

void test_crc()
{
    uint32_t network_protocolId = 2;
    uint32_t  crc32_read = calculate_crc32( (uint8_t*) &network_protocolId, 4 );
    cout << " crc32_read: " << crc32_read << endl;

    uint32_t crc32_write = calculate_crc32( (const uint8_t*) &network_protocolId, 4 );
    uint32_t zero = 0;
    crc32_write = calculate_crc32( (const uint8_t*) &zero, 4, crc32_write );
    
    cout << " crc32_write: " << crc32_write << endl;

}

int main()
{
	cout << " NOT Debug " << endl;

	#ifdef DEBUG
	cout << " Debug " << endl;
	#endif

	#ifdef SELF
	cout << " Self " << endl;
	#endif

    Address address;

    cout << "is valid: " << address.IsValid() << endl;

    Socket socket(8088, SOCKET_TYPE_IPV4);


    cout << "Socket is Error: " << socket.IsError() << endl;
    PacketFactorySingleton::get_mutable_instance().SetNumPacketTypes(TEST_PACKET_NUM_TYPES);
    
    int packetType = PacketFactorySingleton::get_mutable_instance().GetNumPacketTypes();

    cout << "PacketFactory GetNumPacketTypes: " << packetType << endl;

    test_bitpacker();
    test_stream();
    test_packets();
    test_crc();
	return 0;
}
