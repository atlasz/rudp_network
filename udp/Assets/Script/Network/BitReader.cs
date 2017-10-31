using System;
using UnityEngine.Assertions;
using System.Net;

public class BitReader
{
	private readonly UInt32[] m_data;
	private UInt64 m_scratch;
	private int m_numBits;
	private int m_numBytes;
	private int m_bitsRead;
	private int m_scratchBits;
	private int m_wordIndex;
	private int m_numWords;

	public BitReader(UInt32[] data, int bytes )
    {
		Assert.IsNotNull( data );
		m_data = data;
		m_numBytes = bytes;
        m_numBits = m_numBytes * 8;
        m_bitsRead = 0;
        m_scratch = 0;
        m_scratchBits = 0;
        m_wordIndex = 0;

		m_numWords = (bytes + 3) / 4;
    }

    public bool WouldOverflow( int bits )
    {
        return m_bitsRead + bits > m_numBits;
    }

	public UInt32 ReadBits( int bits )
    {
		Assert.IsTrue( bits > 0 );
		Assert.IsTrue( bits <= 32 );
		Assert.IsTrue( m_bitsRead + bits <= m_numBits );

        m_bitsRead += bits;

		Assert.IsTrue( m_scratchBits >= 0 && m_scratchBits <= 64 );

        if ( m_scratchBits < bits )
        {
			Assert.IsTrue( m_wordIndex < m_numWords );
			m_scratch |= (UInt64)m_data[m_wordIndex] << m_scratchBits;
            m_scratchBits += 32;
            m_wordIndex++;
        }

		Assert.IsTrue( m_scratchBits >= bits );

		UInt32 output = (UInt32)(m_scratch & (((UInt64)1 << bits) - 1));

        m_scratch >>= bits;
        m_scratchBits -= bits;

        return output;
    }

    public bool ReadAlign()
    {
        int remainderBits = m_bitsRead % 8;
        if ( remainderBits != 0 )
        {
			UInt32 value = ReadBits( 8 - remainderBits );
			Assert.IsTrue( m_bitsRead % 8 == 0 );
            if ( value != 0 )
                return false;
        }
        return true;
    }

	/// <summary>
	/// Reads the bytes.
	/// care read bits
	/// </summary>
	/// <param name="data">Data.</param>
	/// <param name="bytes">Bytes.</param>
	public void ReadBytes( byte[] data, int bytes )
    {
		Assert.IsTrue( GetAlignBits() == 0 );
		Assert.IsTrue( m_bitsRead + bytes * 8 <= m_numBits );
		Assert.IsTrue( ( m_bitsRead % 32 ) == 0 || ( m_bitsRead % 32 ) == 8 || ( m_bitsRead % 32 ) == 16 || ( m_bitsRead % 32 ) == 24 );

        int headBytes = ( 4 - ( m_bitsRead % 32 ) / 8 ) % 4;
        if ( headBytes > bytes )
            headBytes = bytes;
        for ( int i = 0; i < headBytes; ++i )
		{
			data[i] = (byte) ReadBits( 8 );
		}
			
        if ( headBytes == bytes )
		{
			return;
		}

		Assert.IsTrue( GetAlignBits() == 0 );

        int numWords = ( bytes - headBytes ) / 4;
        if ( numWords > 0 )
        {
			Assert.IsTrue( ( m_bitsRead % 32 ) == 0 );
			Array.Copy(m_data, m_wordIndex, data, headBytes, numWords * 4);
           // memcpy( data + headBytes, &m_data[m_wordIndex], numWords * 4 );
            m_bitsRead += numWords * 32;
            m_wordIndex += numWords;
            m_scratchBits = 0;
        }

		Assert.IsTrue( GetAlignBits() == 0 );

        int tailStart = headBytes + numWords * 4;
        int tailBytes = bytes - tailStart;
		Assert.IsTrue( tailBytes >= 0 && tailBytes < 4 );
        for ( int i = 0; i < tailBytes; ++i )
		{
			data[tailStart+i] = (byte) ReadBits( 8 );
		}
			
		Assert.IsTrue( GetAlignBits() == 0 );

		Assert.IsTrue( headBytes + numWords * 4 + tailBytes == bytes );
    }

    public int GetAlignBits()
    {
        return ( 8 - m_bitsRead % 8 ) % 8;
    }

	public int GetBitsRead()
    {
        return m_bitsRead;
    }

	public int GetBytesRead()
    {
        return m_wordIndex * 4;
    }

	public int GetBitsRemaining()
    {
        return m_numBits - m_bitsRead;
    }

	public int GetBytesRemaining()
    {
        return GetBitsRemaining() / 8;
    }

	public int GetTotalBits() 
    {
        return m_numBits;
    }

	public int GetTotalBytes()
    {
        return m_numBits / 8;
    }
}

