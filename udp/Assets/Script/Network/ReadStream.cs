using System;
using UnityEngine.Assertions;

public class ReadStream:BaseStream
{
	private Object m_context;
	private int m_error;
	private int m_bitsRead;
	private BitReader m_reader;

	public ReadStream(byte[] buffer, int bytes )
	{
		Assert.IsNotNull(buffer);
		Assert.IsTrue(buffer.Length % 4 == 0);
		m_streamType = SteamStateEnum.Read;
		m_context = null;
		m_error = 0;
		m_bitsRead = 0;
		m_reader = new BitReader(NetworkUtil.ByteArrayToUIntArray(buffer), bytes);
	}

	public bool SerializeInteger( ref int value, int min, int max )
	{
		Assert.IsTrue( min < max );
		int bits = (int)NetworkUtil.BitsRequired((uint)min, (uint)max );
		if ( m_reader.WouldOverflow( bits ) )
		{
			m_error = (int)ProtocolError.PROTOCOL2_ERROR_STREAM_OVERFLOW;
			return false;
		}
		UInt32 unsigned_value = m_reader.ReadBits( bits );
		value = (int) unsigned_value + min;
		m_bitsRead += bits;
		return true;
	}

	public bool SerializeBits( ref UInt32 value, int bits )
	{
		Assert.IsTrue( bits > 0 );
		Assert.IsTrue( bits <= 32 );
		if ( m_reader.WouldOverflow( bits ) )
		{
			m_error = (int)ProtocolError.PROTOCOL2_ERROR_STREAM_OVERFLOW;
			return false;
		}
		UInt32 read_value = m_reader.ReadBits( bits );
		value = read_value;
		m_bitsRead += bits;
		return true;
	}

	public bool SerializeBytes( byte[] data, int bytes )
	{
		if ( !SerializeAlign() )
			return false;
		if ( m_reader.WouldOverflow( bytes * 8 ) )
		{
			m_error = (int)ProtocolError.PROTOCOL2_ERROR_STREAM_OVERFLOW;
			return false;
		}
		m_reader.ReadBytes( data, bytes );
		m_bitsRead += bytes * 8;
		return true;
	}

	public bool SerializeAlign()
	{
		int alignBits = m_reader.GetAlignBits();
		if ( m_reader.WouldOverflow( alignBits ) )
		{
			m_error = (int)ProtocolError.PROTOCOL2_ERROR_STREAM_OVERFLOW;
			return false;
		}
		if ( !m_reader.ReadAlign() )
			return false;
		m_bitsRead += alignBits;
		return true;
	}

	public int GetAlignBits()
	{
		return m_reader.GetAlignBits();
	}

//	public bool SerializeCheck( const char * string )
//	{
//		#if PROTOCOL2_SERIALIZE_CHECKS            
//		SerializeAlign();
//		uint32_t value = 0;
//		SerializeAlign();
//		SerializeBits( value, 32 );
//		const uint32_t magic = hash_string( string, 0 );
//		if ( magic != value )
//		{
//		printf( "serialize check failed: '%s'. expected %x, got %x\n", string, magic, value );
//		}
//		return value == magic;
//		#else // #if PROTOCOL2_SERIALIZE_CHECKS
//		return true;
//		#endif // #if PROTOCOL2_SERIALIZE_CHECKS
//	}

	public int GetBitsProcessed()
	{
		return m_bitsRead;
	}

	public int GetBitsRemaining()
	{
		return m_reader.GetBitsRemaining();
	}

	public int GetBytesProcessed()
	{
		return ( m_bitsRead + 7 ) / 8;
	}

	public void SetContext( Object context )
	{
		m_context = context;
	}

	public Object GetContext()
	{
		return m_context;
	}

	public int GetError()
	{
		return m_error;
	}

	public int GetBytesRead()
	{
		return m_reader.GetBytesRead();
	}
}

