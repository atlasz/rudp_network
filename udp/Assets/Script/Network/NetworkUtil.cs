using System;
using UnityEngine.Assertions;

public enum ProtocolError
{
	 PROTOCOL2_ERROR_NONE                        =0,
	 PROTOCOL2_ERROR_CRC32_MISMATCH              =1,
	 PROTOCOL2_ERROR_INVALID_PACKET_TYPE         =2,
	 PROTOCOL2_ERROR_PACKET_TYPE_NOT_ALLOWED     =3,
	 PROTOCOL2_ERROR_CREATE_PACKET_FAILED        =4,
	 PROTOCOL2_ERROR_SERIALIZE_HEADER_FAILED     =5,
	 PROTOCOL2_ERROR_SERIALIZE_PACKET_FAILED     =6,
	 PROTOCOL2_ERROR_SERIALIZE_CHECK_FAILED      =7,
	 PROTOCOL2_ERROR_STREAM_OVERFLOW             =8,
	 PROTOCOL2_ERROR_STREAM_ABORTED              =9,
}

public class NetworkUtil
{
	public static UInt32 BitSwap( UInt32 value )
    {
        return ( value & 0x000000ff ) << 24 | ( value & 0x0000ff00 ) << 8 | ( value & 0x00ff0000 ) >> 8 | ( value & 0xff000000 ) >> 24;
    }

	public static UInt16 BitSwap( UInt16 value )
    {
		return (ushort)((ushort)((value & 0x00ff ) << 8) | (ushort)((value & (ushort)0xff00 ) >> 8));
    }

	public static UInt32[] ByteArrayToUIntArray(byte[] data)
	{
		Assert.IsNotNull(data);
		Assert.IsTrue(data.Length % 4 == 0, "data.Length: " + data.Length);
		int len = data.Length / 4;
		UInt32[] ret = new UInt32[len];
		for(int i = 0; i < len; ++i)
		{
			ret[i] = BitConverter.ToUInt32(data, i * 4);
		}
		return ret;
	}

	public static byte[] UIntArrayToByteArray(UInt32[] data)
	{
		Assert.IsNotNull(data);
		byte[] result = new byte[data.Length * sizeof(uint)];
		Buffer.BlockCopy(data, 0, result, 0, result.Length);
		return result;
	}

	public static uint BitsRequired(uint min, uint max)
	{
		return ( min == max ) ? 0 : Log2( max - min ) + 1;
	}

	private static uint PopCount( uint x )    
	{
		uint a = x - ( ( x >> 1 )       & 0x55555555 );
		uint b =   ( ( ( a >> 2 )       & 0x33333333 ) + ( a & 0x33333333 ) );
		uint c =   ( ( ( b >> 4 ) + b ) & 0x0f0f0f0f );
		uint d =   c + ( c >> 8 );
		uint e =   d + ( d >> 16 );
		uint result = e & 0x0000003f;
        return result;
    }

	private static uint Log2( uint x )
	{
		uint a = x | ( x >> 1 );
		uint b = a | ( a >> 2 );
		uint c = b | ( b >> 4 );
		uint d = c | ( c >> 8 );
		uint e = d | ( d >> 16 );
		uint f = e >> 1;
		return PopCount( f );
	}
}
