#pragma once
#include <NetworkUtil.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class BitReader
{
public:

#ifdef DEBUG
    BitReader( const void* data, int bytes ) : m_data( (const uint32_t*)data ), m_numBytes( bytes ), m_numWords( ( bytes + 3 ) / 4)
#else // #ifdef DEBUG
    BitReader( const void* data, int bytes ) : m_data( (const uint32_t*)data ), m_numBytes( bytes )
#endif // #ifdef DEBUG
    {
        // IMPORTANT: Although we support non-multiples of four bytes passed in, the actual buffer
        // underneath the bit reader must round up to at least 4 bytes because we read a dword at a time.
        assert( data );
        m_numBits = m_numBytes * 8;
        m_bitsRead = 0;
        m_scratch = 0;
        m_scratchBits = 0;
        m_wordIndex = 0;
    }

    bool WouldOverflow( int bits ) const
    {
        return m_bitsRead + bits > m_numBits;
    }

    uint32_t ReadBits( int bits )
    {
        assert( bits > 0 );
        assert( bits <= 32 );
        assert( m_bitsRead + bits <= m_numBits );

        m_bitsRead += bits;

        assert( m_scratchBits >= 0 && m_scratchBits <= 64 );

        if ( m_scratchBits < bits )
        {
            #ifdef DEBUG
            assert( m_wordIndex < m_numWords );
            #endif // #ifdef DEBUG         
            m_scratch |= uint64_t( network_to_host( m_data[m_wordIndex] ) ) << m_scratchBits;
            m_scratchBits += 32;
            m_wordIndex++;
        }

        assert( m_scratchBits >= bits );

        const uint32_t output = m_scratch & ( (uint64_t(1)<<bits) - 1 );

        m_scratch >>= bits;
        m_scratchBits -= bits;

        return output;
    }

    bool ReadAlign()
    {
        const int remainderBits = m_bitsRead % 8;
        if ( remainderBits != 0 )
        {
            uint32_t value = ReadBits( 8 - remainderBits );
            assert( m_bitsRead % 8 == 0 );
            if ( value != 0 )
                return false;
        }
        return true;
    }

    void ReadBytes( uint8_t* data, int bytes )
    {
        assert( GetAlignBits() == 0 );
        assert( m_bitsRead + bytes * 8 <= m_numBits );
        assert( ( m_bitsRead % 32 ) == 0 || ( m_bitsRead % 32 ) == 8 || ( m_bitsRead % 32 ) == 16 || ( m_bitsRead % 32 ) == 24 );

        int headBytes = ( 4 - ( m_bitsRead % 32 ) / 8 ) % 4;
        if ( headBytes > bytes )
            headBytes = bytes;
        for ( int i = 0; i < headBytes; ++i )
            data[i] = (uint8_t) ReadBits( 8 );
        if ( headBytes == bytes )
            return;

        assert( GetAlignBits() == 0 );

        int numWords = ( bytes - headBytes ) / 4;
        if ( numWords > 0 )
        {
            assert( ( m_bitsRead % 32 ) == 0 );
            memcpy( data + headBytes, &m_data[m_wordIndex], numWords * 4 );
            m_bitsRead += numWords * 32;
            m_wordIndex += numWords;
            m_scratchBits = 0;
        }

        assert( GetAlignBits() == 0 );

        int tailStart = headBytes + numWords * 4;
        int tailBytes = bytes - tailStart;
        assert( tailBytes >= 0 && tailBytes < 4 );
        for ( int i = 0; i < tailBytes; ++i )
            data[tailStart+i] = (uint8_t) ReadBits( 8 );

        assert( GetAlignBits() == 0 );

        assert( headBytes + numWords * 4 + tailBytes == bytes );
    }

    int GetAlignBits() const
    {
        return ( 8 - m_bitsRead % 8 ) % 8;
    }

    int GetBitsRead() const
    {
        return m_bitsRead;
    }

    int GetBytesRead() const
    {
        return m_wordIndex * 4;
    }

    int GetBitsRemaining() const
    {
        return m_numBits - m_bitsRead;
    }

    int GetBytesRemaining() const
    {
        return GetBitsRemaining() / 8;
    }

    int GetTotalBits() const 
    {
        return m_numBits;
    }

    int GetTotalBytes() const
    {
        return m_numBits / 8;
    }

private:

    const uint32_t* m_data;
    uint64_t m_scratch;
    int m_numBits;
    int m_numBytes;
#ifdef DEBUG
    int m_numWords;
#endif // #ifdef DEBUG
    int m_bitsRead;
    int m_scratchBits;
    int m_wordIndex;
};