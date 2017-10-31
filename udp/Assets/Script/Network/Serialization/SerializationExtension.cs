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
		Assert.IsTrue(bits < 32);  
		uint uint32_value = (uint)value;
		return stream.SerializeBits(uint32_value, bits);
	}

	public static uint DeserializeBits(this NObject obj, ReadStream stream, int bits)
	{
		Assert.IsTrue(bits > 0); 
		Assert.IsTrue(bits < 32);  
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
}
