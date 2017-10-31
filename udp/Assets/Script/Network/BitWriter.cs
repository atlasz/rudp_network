using System;
using UnityEngine.Assertions;
using System.Net;

public class BitWriter
{
	private UInt32[] m_data;
	private UInt64 m_scratch;
	private int m_numBits;
	private int m_numWords;
	private int m_bitsWritten;
	private int m_wordIndex;
	private int m_scratchBits;

	public BitWriter (UInt32[] data, int bytes)
	{
		Assert.IsNotNull(data);
		Assert.AreEqual(bytes % 4, 0);
		m_data = data;
		m_numWords = bytes / 4;
		m_numBits = m_numWords * 32;
		m_bitsWritten = 0;
		m_wordIndex = 0;
		m_scratch = 0;
		m_scratchBits = 0;
	}

	public void WriteBits( UInt32 value, int bits )
	{
		Assert.IsTrue( bits > 0 );
		Assert.IsTrue( bits <= 32 );
		Assert.IsTrue( m_bitsWritten + bits <= m_numBits );

		value &= (UInt32)(((UInt64)1 << bits ) - 1);

		m_scratch |= (UInt64) value << m_scratchBits;

		m_scratchBits += bits;

		if ( m_scratchBits >= 32 )
		{
			Assert.IsTrue( m_wordIndex < m_numWords );
			m_data[m_wordIndex] = (UInt32)(m_scratch & 0xFFFFFFFF);//endian
			m_scratch >>= 32;
			m_scratchBits -= 32;
			m_wordIndex++;
		}

		m_bitsWritten += bits;
	}

	public void WriteAlign()
	{
		int remainderBits = m_bitsWritten % 8;
		if ( remainderBits != 0 )
		{
			UInt32 zero = 0;
			WriteBits( zero, 8 - remainderBits );
			Assert.IsTrue( ( m_bitsWritten % 8 ) == 0 );
		}
	}

	public void WriteBytes( byte[] data, int bytes )
	{
		Assert.IsTrue( GetAlignBits() == 0 );
		Assert.IsTrue( m_bitsWritten + bytes * 8 <= m_numBits );
		Assert.IsTrue( ( m_bitsWritten % 32 ) == 0 || ( m_bitsWritten % 32 ) == 8 || ( m_bitsWritten % 32 ) == 16 || ( m_bitsWritten % 32 ) == 24 );

		int headBytes = ( 4 - ( m_bitsWritten % 32 ) / 8 ) % 4;
		if ( headBytes > bytes )
			headBytes = bytes;
		for ( int i = 0; i < headBytes; ++i )
			WriteBits( data[i], 8 );
		if ( headBytes == bytes )
			return;

		FlushBits();

		Assert.IsTrue( GetAlignBits() == 0 );

		int numWords = ( bytes - headBytes ) / 4;
		if ( numWords > 0 )
		{
			Assert.IsTrue( ( m_bitsWritten % 32 ) == 0 );
			Array.Copy(data, headBytes, m_data, m_wordIndex, numWords * 4);
			//memcpy( &m_data[m_wordIndex], data + headBytes, numWords * 4 );
			m_bitsWritten += numWords * 32;
			m_wordIndex += numWords;
			m_scratch = 0;
		}

		Assert.IsTrue( GetAlignBits() == 0 );

		int tailStart = headBytes + numWords * 4;
		int tailBytes = bytes - tailStart;
		Assert.IsTrue( tailBytes >= 0 && tailBytes < 4 );
		for ( int i = 0; i < tailBytes; ++i )
			WriteBits( data[tailStart+i], 8 );

		Assert.IsTrue( GetAlignBits() == 0 );

		Assert.IsTrue( headBytes + numWords * 4 + tailBytes == bytes );
	}

	public void FlushBits()
	{
		if ( m_scratchBits != 0 )
		{
			Assert.IsTrue( m_wordIndex < m_numWords );
			m_data[m_wordIndex] = (UInt32)(m_scratch & 0xFFFFFFFF);//todo endian
			m_scratch >>= 32;
			m_scratchBits = 0;
			m_wordIndex++;                
		}
	}

	public int GetAlignBits()
	{
		return ( 8 - ( m_bitsWritten % 8 ) ) % 8;
	}

	public int GetBitsWritten()
	{
		return m_bitsWritten;
	}

	public int GetBitsAvailable()
	{
		return m_numBits - m_bitsWritten;
	}

	public UInt32[] GetData()
	{
		return m_data;
	}

	public int GetBytesWritten()
	{
		return ( m_bitsWritten + 7 ) / 8;
	}

	public int GetTotalBytes()
	{
		return m_numWords * 4;
	}
}

