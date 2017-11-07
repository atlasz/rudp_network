#pragma once
#include <NetworkUtil.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <BitWriter.h>
#include <BitReader.h>

#define YOJIMBO_PROTOCOL_ERROR_NONE                         0
#define YOJIMBO_PROTOCOL_ERROR_CRC32_MISMATCH               1
#define YOJIMBO_PROTOCOL_ERROR_INVALID_PACKET_TYPE          2
#define YOJIMBO_PROTOCOL_ERROR_PACKET_TYPE_NOT_ALLOWED      3
#define YOJIMBO_PROTOCOL_ERROR_CREATE_PACKET_FAILED         4
#define YOJIMBO_PROTOCOL_ERROR_SERIALIZE_HEADER_FAILED      5
#define YOJIMBO_PROTOCOL_ERROR_SERIALIZE_PACKET_FAILED      6
#define YOJIMBO_PROTOCOL_ERROR_SERIALIZE_CHECK_FAILED       7
#define YOJIMBO_PROTOCOL_ERROR_STREAM_OVERFLOW              8
#define YOJIMBO_PROTOCOL_ERROR_STREAM_ABORTED               9

class Object
{  
public:

    virtual ~Object() {}

    virtual bool SerializeRead( class ReadStream & stream ) = 0;

    virtual bool SerializeWrite( class WriteStream & stream ) = 0;

    virtual bool SerializeMeasure( class MeasureStream & stream ) = 0;
};

 #define SERIALIZE_FUNCTIONS()                                                                                  \
        bool SerializeRead( class ReadStream & stream ) { return Serialize( stream ); };               \
        bool SerializeWrite( class WriteStream & stream ) { return Serialize( stream ); };             \
        bool SerializeMeasure( class MeasureStream & stream ) { return Serialize( stream ); };         \
        
class WriteStream
{
public:

    enum { IsWriting = 1 };
    enum { IsReading = 0 };

    WriteStream( uint8_t* buffer, int bytes ) : m_error( YOJIMBO_PROTOCOL_ERROR_NONE ), m_context( NULL ), m_writer( buffer, bytes ) {}

    bool SerializeInteger( int32_t value, int32_t min, int32_t max )
    {
        assert( min < max );
        assert( value >= min );
        assert( value <= max );
        const int bits = bits_required( min, max );
        uint32_t unsigned_value = value - min;
        m_writer.WriteBits( unsigned_value, bits );
        return true;
    }

    bool SerializeBits( uint32_t value, int bits )
    {
        assert( bits > 0 );
        assert( bits <= 32 );
        m_writer.WriteBits( value, bits );
        return true;
    }

    bool SerializeBytes( const uint8_t* data, int bytes )
    {
        assert( data );
        assert( bytes >= 0 );
        if ( !SerializeAlign() )
            return false;
        m_writer.WriteBytes( data, bytes );
        return true;
    }

    bool SerializeAlign()
    {
        m_writer.WriteAlign();
        return true;
    }

    int GetAlignBits() const
    {
        return m_writer.GetAlignBits();
    }

    bool SerializeCheck( const char * string )
    {
#if YOJIMBO_SERIALIZE_CHECKS
        SerializeAlign();
        const uint32_t magic = hash_string( string, 0 );
        SerializeBits( magic, 32 );
#endif // #if YOJIMBO_SERIALIZE_CHECKS
        return true;
    }

    void Flush()
    {
        m_writer.FlushBits();
    }

    const uint8_t* GetData() const
    {
        return m_writer.GetData();
    }

    int GetBytesProcessed() const
    {
        return m_writer.GetBytesWritten();
    }

    int GetBitsProcessed() const
    {
        return m_writer.GetBitsWritten();
    }

    int GetBitsRemaining() const
    {
        return GetTotalBits() - GetBitsProcessed();
    }

    int GetTotalBits() const
    {
        return m_writer.GetTotalBytes() * 8;
    }

    int GetTotalBytes() const
    {
        return m_writer.GetTotalBytes();
    }

    void SetContext( void *context )
    {
        m_context = context;
    }

    void* GetContext() const
    {
        return m_context;
    }

    int GetError() const
    {
        return m_error;
    }

private:

    int m_error;
    void *m_context;
    BitWriter m_writer;
};

class ReadStream
{
public:

    enum { IsWriting = 0 };
    enum { IsReading = 1 };

    ReadStream( const uint8_t* buffer, int bytes ) : m_context( NULL ), m_error( YOJIMBO_PROTOCOL_ERROR_NONE ), m_bitsRead(0), m_reader( buffer, bytes ) {}

    bool SerializeInteger( int32_t & value, int32_t min, int32_t max )
    {
        assert( min < max );
        const int bits = bits_required( min, max );
        if ( m_reader.WouldOverflow( bits ) )
        {
            m_error = YOJIMBO_PROTOCOL_ERROR_STREAM_OVERFLOW;
            return false;
        }
        uint32_t unsigned_value = m_reader.ReadBits( bits );
        value = (int32_t) unsigned_value + min;
        m_bitsRead += bits;
        return true;
    }

    bool SerializeBits( uint32_t & value, int bits )
    {
        assert( bits > 0 );
        assert( bits <= 32 );
        if ( m_reader.WouldOverflow( bits ) )
        {
            m_error = YOJIMBO_PROTOCOL_ERROR_STREAM_OVERFLOW;
            return false;
        }
        uint32_t read_value = m_reader.ReadBits( bits );
        value = read_value;
        m_bitsRead += bits;
        return true;
    }

    bool SerializeBytes( uint8_t* data, int bytes )
    {
        if ( !SerializeAlign() )
            return false;
        if ( m_reader.WouldOverflow( bytes * 8 ) )
        {
            m_error = YOJIMBO_PROTOCOL_ERROR_STREAM_OVERFLOW;
            return false;
        }
        m_reader.ReadBytes( data, bytes );
        m_bitsRead += bytes * 8;
        return true;
    }

    bool SerializeAlign()
    {
        const int alignBits = m_reader.GetAlignBits();
        if ( m_reader.WouldOverflow( alignBits ) )
        {
            m_error = YOJIMBO_PROTOCOL_ERROR_STREAM_OVERFLOW;
            return false;
        }
        if ( !m_reader.ReadAlign() )
            return false;
        m_bitsRead += alignBits;
        return true;
    }

    int GetAlignBits() const
    {
        return m_reader.GetAlignBits();
    }

    bool SerializeCheck( const char * string )
    {
#if YOJIMBO_SERIALIZE_CHECKS            
        SerializeAlign();
        uint32_t value = 0;
        SerializeAlign();
        SerializeBits( value, 32 );
        const uint32_t magic = hash_string( string, 0 );
        if ( magic != value )
        {
            printf( "serialize check failed: '%s'. expected %x, got %x\n", string, magic, value );
        }
        return value == magic;
#else // #if YOJIMBO_SERIALIZE_CHECKS
        return true;
#endif // #if YOJIMBO_SERIALIZE_CHECKS
    }

    int GetBitsProcessed() const
    {
        return m_bitsRead;
    }

    int GetBitsRemaining() const
    {
        return m_reader.GetBitsRemaining();
    }

    int GetBytesProcessed() const
    {
        return ( m_bitsRead + 7 ) / 8;
    }

    void SetContext( void* context )
    {
        m_context = context;
    }

    void* GetContext() const
    {
        return m_context;
    }

    int GetError() const
    {
        return m_error;
    }

    int GetBytesRead() const
    {
        return m_reader.GetBytesRead();
    }

private:

    void* m_context;
    int m_error;
    int m_bitsRead;
    BitReader m_reader;
};

class MeasureStream
{
public:

    enum { IsWriting = 1 };
    enum { IsReading = 0 };

    MeasureStream( int bytes ) : m_context( NULL ), m_error( YOJIMBO_PROTOCOL_ERROR_NONE ), m_totalBytes( bytes ), m_bitsWritten(0) {}

#ifdef DEBUG
    bool SerializeInteger( int32_t value, int32_t min, int32_t max )
#else // #ifdef DEBUG
    bool SerializeInteger( int32_t /*value*/, int32_t min, int32_t max )
#endif // #ifdef DEBUG
    {
        assert( min < max );
        #ifdef DEBUG
        assert( value >= min );
        assert( value <= max );
        #endif // #ifdef DEBUG
        const int bits = bits_required( min, max );
        m_bitsWritten += bits;
        return true;
    }

    bool SerializeBits( uint32_t /*value*/, int bits )
    {
        assert( bits > 0 );
        assert( bits <= 32 );
        m_bitsWritten += bits;
        return true;
    }

    bool SerializeBytes( const uint8_t* /*data*/, int bytes )
    {
        SerializeAlign();
        m_bitsWritten += bytes * 8;
        return true;
    }

    bool SerializeAlign()
    {
        const int alignBits = GetAlignBits();
        m_bitsWritten += alignBits;
        return true;
    }

    int GetAlignBits() const
    {
        return 7;       // we can't know for sure, so be conservative and assume worst case
    }

    bool SerializeCheck( const char * /*string*/ )
    {
#if YOJIMBO_SERIALIZE_CHECKS
        SerializeAlign();
        m_bitsWritten += 32;
#endif // #if YOJIMBO_SERIALIZE_CHECKS
        return true;
    }

    int GetBitsProcessed() const
    {
        return m_bitsWritten;
    }

    int GetBitsRemaining() const
    {
        return m_totalBytes * 8 - GetBitsProcessed();
    }

    int GetBytesProcessed() const
    {
        return ( m_bitsWritten + 7 ) / 8;
    }

    int GetTotalBytes() const
    {
        return m_totalBytes;
    }

    int GetTotalBits() const
    {
        return m_totalBytes * 8;
    }

    void SetContext( void* context )
    {
        m_context = context;
    }

    void* GetContext() const
    {
        return m_context;
    }

    int GetError() const
    {
        return m_error;
    }

private:

    void* m_context;
    int m_error;
    int m_totalBytes;
    int m_bitsWritten;
};

#define serialize_int( stream, value, min, max )                    \
    do                                                              \
    {                                                               \
        assert( min < max );                                        \
        int32_t int32_value;                                        \
        if ( Stream::IsWriting )                                    \
        {                                                           \
            assert( int64_t(value) >= int64_t(min) );               \
            assert( int64_t(value) <= int64_t(max) );               \
            int32_value = (int32_t) value;                          \
        }                                                           \
        if ( !stream.SerializeInteger( int32_value, min, max ) )    \
            return false;                                           \
        if ( Stream::IsReading )                                    \
        {                                                           \
            value = int32_value;                                    \
            if ( value < min || value > max )                       \
                return false;                                       \
        }                                                           \
    } while (0)

#define serialize_bits( stream, value, bits )						\
    do                                                              \
    {                                                               \
        assert( bits > 0 );                                         \
        assert( bits <= 32 );                                       \
        uint32_t uint32_value;                                      \
        if ( Stream::IsWriting )                                    \
            uint32_value = (uint32_t) value;                        \
        if ( !stream.SerializeBits( uint32_value, bits ) )          \
            return false;                                           \
        if ( Stream::IsReading )                                    \
            value = uint32_value;                                   \
    } while (0)

#define serialize_bool( stream, value )								\
	do																\
	{																\
        uint32_t uint32_bool_value;									\
		if ( Stream::IsWriting )									\
			uint32_bool_value = value ? 1 : 0; 						\
		serialize_bits( stream, uint32_bool_value, 1 );				\
		if ( Stream::IsReading )									\
			value = uint32_bool_value ? true : false;				\
	} while (0)

#define serialize_enum( stream, value, type, num_entries )          \
    do                                                              \
    {                                                               \
        uint32_t int_value;                                         \
        if ( Stream::IsWriting )                                    \
            int_value = value;                                      \
        serialize_int( stream, int_value, 0, num_entries - 1 );     \
        if ( Stream::IsReading )                                    \
            value = (type) value;                                   \
    } while (0) 

template <typename Stream> bool serialize_float_internal( Stream & stream, float & value )
{
    uint32_t int_value;

    if ( Stream::IsWriting )
        memcpy( &int_value, &value, 4 );

    bool result = stream.SerializeBits( int_value, 32 );

    if ( Stream::IsReading )
        memcpy( &value, &int_value, 4 );

    return result;
}

#define serialize_float( stream, value )                                        \
    do                                                                          \
    {                                                                           \
        if ( !serialize_float_internal( stream, value ) )              \
            return false;                                                       \
    } while (0)

#define serialize_uint32( stream, value )                                       \
    serialize_bits( stream, value, 32 );

template <typename Stream> bool serialize_uint64_internal( Stream & stream, uint64_t & value )
{
    uint32_t hi,lo;
    if ( Stream::IsWriting )
    {
        lo = value & 0xFFFFFFFF;
        hi = value >> 32;
    }
    serialize_bits( stream, lo, 32 );
    serialize_bits( stream, hi, 32 );
    if ( Stream::IsReading )
        value = ( uint64_t(hi) << 32 ) | lo;
    return true;
}

#define serialize_uint64( stream, value )                                       \
    do                                                                          \
    {                                                                           \
        if ( !serialize_uint64_internal( stream, value ) )             \
            return false;                                                       \
    } while (0)

template <typename Stream> bool serialize_double_internal( Stream & stream, double & value )
{
    union DoubleInt
    {
        double double_value;
        uint64_t int_value;
    };

    DoubleInt tmp;
    if ( Stream::IsWriting )
        tmp.double_value = value;

    serialize_uint64( stream, tmp.int_value );

    if ( Stream::IsReading )
        value = tmp.double_value;

    return true;
}

#define serialize_double( stream, value )                                       \
    do                                                                          \
    {                                                                           \
        if ( !serialize_double_internal( stream, value ) )             \
            return false;                                                       \
    } while (0)

template <typename Stream> bool serialize_bytes_internal( Stream & stream, uint8_t* data, int bytes )
{
    return stream.SerializeBytes( data, bytes );
}

#define serialize_bytes( stream, data, bytes )                                  \
    do                                                                          \
    {                                                                           \
        if ( !serialize_bytes_internal( stream, data, bytes ) )        \
            return false;                                                       \
    } while (0)

template <typename Stream> bool serialize_string_internal( Stream & stream, char* string, int buffer_size )
{
    int length;
    if ( Stream::IsWriting )
    {
        length = (int) strlen( string );
        assert( length < buffer_size - 1 );
    }
    serialize_int( stream, length, 0, buffer_size - 1 );
    serialize_bytes( stream, (uint8_t*)string, length );
    if ( Stream::IsReading )
        string[length] = '\0';
    return true;
}

#define serialize_string( stream, string, buffer_size )                                 \
    do                                                                                  \
    {                                                                                   \
        if ( !serialize_string_internal( stream, string, buffer_size ) )     \
            return false;                                                               \
    } while (0)

#define serialize_align( stream )                                                       \
    do                                                                                  \
    {                                                                                   \
        if ( !stream.SerializeAlign() )                                                 \
            return false;                                                               \
    } while (0)

#define serialize_check( stream, string )                                               \
    do                                                                                  \
    {                                                                                   \
        if ( !stream.SerializeCheck( string ) )                                         \
            return false;                                                               \
    } while (0)

#define serialize_object( stream, object )                                              \
    do                                                                                  \
    {                                                                                   \
        if ( !object.Serialize( stream ) )                                              \
            return false;                                                               \
    }                                                                                   \
    while(0)

#define read_bits( stream, value, bits )                                                \
do                                                                                      \
{                                                                                       \
    assert( bits > 0 );                                                                 \
    assert( bits <= 32 );                                                               \
    uint32_t uint32_value;                                                              \
    if ( !stream.SerializeBits( uint32_value, bits ) )                                  \
        return false;                                                                   \
    value = uint32_value;                                                               \
} while (0)

#define read_int( stream, value, min, max )                                             \
    do                                                                                  \
    {                                                                                   \
        assert( min < max );                                                            \
        int32_t int32_value;                                                            \
        if ( !stream.SerializeInteger( int32_value, min, max ) )                        \
            return false;                                                               \
        value = int32_value;                                                            \
        if ( value < min || value > max )                                               \
            return false;                                                               \
    } while (0)

#define read_bool( stream, value ) read_bits( stream, value, 1 )

#define read_float     serialize_float
#define read_uint32    serialize_uint32
#define read_uint64    serialize_uint64
#define read_double    serialize_double
#define read_bytes     serialize_bytes
#define read_string    serialize_string
#define read_align     serialize_align
#define read_check     serialize_check
#define read_object    serialize_object

#define write_bits( stream, value, bits )                                               \
    do                                                                                  \
    {                                                                                   \
        assert( bits > 0 );                                                             \
        assert( bits <= 32 );                                                           \
        uint32_t uint32_value = (uint32_t) value;                                       \
        if ( !stream.SerializeBits( uint32_value, bits ) )                              \
            return false;                                                               \
    } while (0)

#define write_int( stream, value, min, max )                                            \
    do                                                                                  \
    {                                                                                   \
        assert( min < max );                                                            \
        assert( value >= min );                                                         \
        assert( value <= max );                                                         \
        int32_t int32_value = (int32_t) value;                                          \
        if ( !stream.SerializeInteger( int32_value, min, max ) )                        \
            return false;                                                               \
    } while (0)

#define write_float    serialize_float
#define write_uint32   serialize_uint32
#define write_uint64   serialize_uint64
#define write_double   serialize_double
#define write_bytes    serialize_bytes
#define write_string   serialize_string
#define write_align    serialize_align
#define write_check    serialize_check
#define write_object   serialize_object