using UnityEngine.Assertions;
using System;


public static class SerializationExtension 
{
	public static bool SerializeInt(this NObject obj, WriteStream stream, int value, int min, int max)
	{
		Assert.IsTrue(min < max);               
		Assert.IsTrue(value >= min);               
		Assert.IsTrue(value <= max);               
		int int32_value = value;                                                            
		return stream.SerializeInteger( int32_value, min, max );
	}

	public static int DeserializeInt(this NObject obj, ReadStream stream, int min, int max)
	{
		Assert.IsTrue(min < max);                            
		int int32_value = 0;                                                          
		stream.SerializeInteger( ref int32_value, min, max );
		return int32_value;
	}
		
	public static bool SerializeBits(this NObject obj, WriteStream stream, int value, int bits)
	{
		Assert.IsTrue(bits > 0); 
		Assert.IsTrue(bits <= 32);  
		uint uint32_value = (uint)value;
		return stream.SerializeBits(uint32_value, bits);
	}

	public static uint DeserializeBits(this NObject obj, ReadStream stream, int bits)
	{
		Assert.IsTrue(bits > 0); 
		Assert.IsTrue(bits <= 32);  
		uint uint32_value = 0;
		stream.SerializeBits(ref uint32_value, bits);
		return uint32_value;
	}

	public static void SerializeAlign(this NObject obj, BaseStream stream)
	{
		if(stream.streamType == SteamStateEnum.Read)
		{
			ReadStream read = (ReadStream)stream;
			read.SerializeAlign();
		}
		else if(stream.streamType == SteamStateEnum.Write)
		{
			WriteStream write = (WriteStream)stream;
			write.SerializeAlign();
		}
	}

	public static bool SerializeBool(this NObject obj, WriteStream stream, bool value)
	{ 
		uint uint32_value = value ? (uint)1 : 0;
		return stream.SerializeBits(uint32_value, 1);
	}

	public static bool DeserializeBool(this NObject obj, ReadStream stream)
	{ 
		uint uint32_value = 0;
		stream.SerializeBits(ref uint32_value, 1);
		return uint32_value > 0;
	}

	public static bool SerializeFloat(this NObject obj, WriteStream stream, float value)
	{              
		byte[] valArr = BitConverter.GetBytes(value);//todo: reduce extra memeory?
		uint uint32_value = BitConverter.ToUInt32(valArr, 0);
		return stream.SerializeBits( uint32_value, 32);
	}

	public static float DeserializeFloat(this NObject obj, ReadStream stream)
	{              
		uint uint32_value = 0;                                                            
		stream.SerializeBits( ref uint32_value, 32);
		byte[] valArr = BitConverter.GetBytes(uint32_value);
		float ret = BitConverter.ToSingle(valArr, 0);
		return ret;
	}

	public static bool SerializeUInt64(this NObject obj, WriteStream stream, UInt64 value)
	{              
		uint high,low;
		low = ((uint)value) & 0xFFFFFFFF;
		high = (uint)(value >> 32);
		SerializeBits(obj,stream, (int)low, 32);
		SerializeBits(obj, stream, (int)high, 32);
		return true;
	}

	public static UInt64 DeserializeUInt64(this NObject obj, ReadStream stream)
	{              
		uint high,low;
		low = DeserializeBits(obj, stream, 32);
		high = DeserializeBits(obj, stream, 32);
		UInt64 value = ((UInt64)high << 32) | low;
		return value;
	}

	public static bool SerializeDouble(this NObject obj, WriteStream stream, double value)
	{              
		ulong val = (ulong)BitConverter.DoubleToInt64Bits(value);
		SerializeUInt64(obj, stream, val);
		return true;
	}

	public static double DeserializeDouble(this NObject obj, ReadStream stream)
	{              
		long val = (long)DeserializeUInt64(obj, stream);
		return BitConverter.Int64BitsToDouble(val);
	}

	public static bool SerializeBytes(this NObject obj, WriteStream stream, byte[] data, int bytes)
	{              
		return stream.SerializeBytes(data, bytes);
	}

	public static bool DeserializeBytes(this NObject obj, ReadStream stream, ref byte[] data, int bytes)
	{              
		return stream.SerializeBytes(ref data, bytes);
	}

	public static bool SerializeString(this NObject obj, WriteStream stream, string data)
	{             
		int length = data.Length;
		SerializeInt(obj, stream, length, 0, int.MaxValue);
		char[] arr = data.ToCharArray();
		SerializeBytes(obj, stream, System.Text.Encoding.Default.GetBytes(data.ToCharArray()), length);
		return true;
	}

	public static string DeserializeString(this NObject obj, ReadStream stream)
	{             
		int length = DeserializeInt(obj, stream, 0, int.MaxValue);
		byte[] val = new byte[length];
		DeserializeBytes(obj, stream, ref val, length);
		//val[length] = Convert.ToByte('\0');
		string ret =System.Text.Encoding.Default.GetString(val);
		return ret;
	}

}
