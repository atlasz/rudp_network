using System;
using UnityEngine.Assertions;

public class WriteStream:BaseStream
{
	private int m_error;
	private Object m_context;
	private BitWriter m_writer;

	public WriteStream( byte[] buffer, int bytes )
	{
		m_streamType = SteamStateEnum.Write;
		m_error = 0;
		m_context = null;
		m_writer = new BitWriter(NetworkUtil.ByteArrayToUIntArray(buffer), bytes);
	}

	public bool SerializeInteger( int value, int min, int max )
	{
		Assert.IsTrue( min < max );
		Assert.IsTrue( value >= min );
		Assert.IsTrue( value <= max );
		int bits = (int)NetworkUtil.BitsRequired( (uint)min, (uint)max );
		uint unsigned_value = (UInt32)(value - min);
		m_writer.WriteBits( unsigned_value, bits );
		return true;
	}

	public bool SerializeBits( UInt32 value, int bits )
	{
		Assert.IsTrue( bits > 0 );
		Assert.IsTrue( bits <= 32 );
		m_writer.WriteBits( value, bits );
		return true;
	}

	public bool SerializeBytes(byte[] data, int bytes )
	{
		Assert.IsNotNull( data );
		Assert.IsTrue( bytes >= 0 );
		if ( !SerializeAlign() )
			return false;
		m_writer.WriteBytes( data, bytes );
		return true;
	}

	public bool SerializeAlign()
	{
		m_writer.WriteAlign();
		return true;
	}

	public int GetAlignBits()
	{
		return m_writer.GetAlignBits();
	}

//	public bool SerializeCheck( const char * string )
//	{
//		#if PROTOCOL2_SERIALIZE_CHECKS
//		SerializeAlign();
//		const uint32_t magic = hash_string( string, 0 );
//		SerializeBits( magic, 32 );
//		#endif // #if PROTOCOL2_SERIALIZE_CHECKS
//		return true;
//	}

	public void Flush()
	{
		m_writer.FlushBits();
	}

	public byte[] GetData()
	{
		return NetworkUtil.UIntArrayToByteArray(m_writer.GetData ());
	}

	public int GetBytesProcessed()
	{
		return m_writer.GetBytesWritten();
	}

	public int GetBitsProcessed()
	{
		return m_writer.GetBitsWritten();
	}

	public int GetBitsRemaining()
	{
		return GetTotalBits() - GetBitsProcessed();
	}

	public int GetTotalBits()
	{
		return m_writer.GetTotalBytes() * 8;
	}

	public int GetTotalBytes()
	{
		return m_writer.GetTotalBytes();
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
}

