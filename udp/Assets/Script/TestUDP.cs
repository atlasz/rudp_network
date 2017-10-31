using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net.Sockets;
using System;
using System.Net;
using System.Text;
using UnityEngine.Assertions;

public class TestUDP : MonoBehaviour {

	// Use this for initialization
	void Start () {
		Test();
	}
	
	// Update is called once per frame
	void Update () {
		
	}

	private void EchoTest()
	{
		byte[] data = new byte[1024];
		string input ,stringData;

		Debug.Log(string.Format("This is a Client, host name is {0}", Dns.GetHostName()));

		//设置服务IP，设置TCP端口号
		IPEndPoint ipep = new IPEndPoint(IPAddress .Parse ("127.0.0.1") , 8888);

		//定义网络类型，数据连接类型和网络协议UDP
		Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);

		string welcome = "Hello! ";
		data = Encoding.ASCII.GetBytes(welcome);
		server.SendTo(data, data.Length, SocketFlags.None, ipep);
		IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);
		EndPoint Remote = (EndPoint)sender;

		data = new byte[1024];
		//对于不存在的IP地址，加入此行代码后，可以在指定时间内解除阻塞模式限制
		//server.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReceiveTimeout, 100);
		int recv = server.ReceiveFrom(data, ref Remote);
		Console.WriteLine("Message received from {0}: ", Remote.ToString());
		Console.WriteLine(Encoding .ASCII .GetString (data,0,recv));

		//todo
		input = "hello world";
		server .SendTo (Encoding .ASCII .GetBytes (input ),Remote );
		data = new byte [1024];
		recv = server.ReceiveFrom(data, ref Remote);
		stringData = Encoding.ASCII.GetString(data, 0, recv);

		Debug.LogError(stringData);

		Debug.Log ("Stopping Client.");
		server .Close ();            
	}

	private void Test()
	{
		//EndianTest();
		//SignTest();
		//BitPackerTest();
		TestStream();
		//EchoTest();
	}

	private void EndianTest()
	{
		Assert.IsTrue(BitConverter.IsLittleEndian);
	}

	private void SignTest()
	{
		Debug.Log("SignTest");
		unchecked
		{
			Debug.Log((uint)-8);
		}
	}

	private void BitPackerTest()
	{
		Debug.Log( "test_bitpacker" );

		int BufferSize = 256;

		UInt32[] buffer = new UInt32[256];

		BitWriter writer = new BitWriter( buffer, BufferSize );

		check( writer.GetData() == buffer );
		check( writer.GetTotalBytes() == BufferSize );
		check( writer.GetBitsWritten() == 0 );
		check( writer.GetBytesWritten() == 0 );
		check( writer.GetBitsAvailable() == BufferSize * 8 );

		writer.WriteBits( 0, 1 );
		writer.WriteBits( 1, 1 );
		//writer.WriteBits( 10, 4 );
		unchecked
		{
			writer.WriteBits((uint)-8, 32);
		}
		writer.WriteBits( 255, 8 );
		writer.WriteBits( 1000, 10 );
		writer.WriteBits( 50000, 16 );
		writer.WriteBits( 9999999, 32 );
	
		writer.FlushBits();

		int bitsWritten = 1 + 1 + 32 + 8 + 10 + 16 + 32;

		check( writer.GetBytesWritten() == 13 );
		check( writer.GetTotalBytes() == BufferSize );
		check( writer.GetBitsWritten() == bitsWritten );
		check( writer.GetBitsAvailable() == BufferSize * 8 - bitsWritten );

		int bytesWritten = writer.GetBytesWritten();
//
		check( bytesWritten == 13 );
//
		UInt32[] bufferReader = new UInt32[256];
		Array.Copy(buffer, 0, bufferReader, 0,  bytesWritten);
//
		BitReader reader = new BitReader( bufferReader, bytesWritten );
//
		check( reader.GetBitsRead() == 0 );
		check( reader.GetBitsRemaining() == bytesWritten * 8 );

		UInt32 a = reader.ReadBits( 1 );
		UInt32 b = reader.ReadBits( 1 );
		//UInt32 c = reader.ReadBits( 32 );
		int c = (int)reader.ReadBits( 32 );
		UInt32 d = reader.ReadBits( 8 );
		UInt32 e = reader.ReadBits( 10 );
		UInt32 f = reader.ReadBits( 16 );
		UInt32 g = reader.ReadBits( 32 );
		//int h = (int)reader.ReadBits(4);

		Debug.Log("a: " + a);
		Debug.Log("b: " + b);
		Debug.Log("c: " + c);
		Debug.Log("d: " + d);
		Debug.Log("e: " + e);
		Debug.Log("f: " + f);
		Debug.Log("g: " + g);
		//Debug.Log("h: " + h);

		check( a == 0 );
		check( b == 1 );
		check( c == -8 );
		check( d == 255 );
		check( e == 1000 );
		check( f == 50000 );
		check( g == 9999999 );

		check( reader.GetBitsRead() == bitsWritten );
		check( reader.GetBitsRemaining() == bytesWritten * 8 - bitsWritten );
	}

	private void check(bool val)
	{
		Assert.IsTrue(val);
	}

	private void TestStream()
	{
		int BufferSize = 1024;

		byte[] buffer = new byte[BufferSize];

		WriteStream writeSteam = new WriteStream(buffer, BufferSize);
		TestObject obj = new TestObject();
		obj.data.a = -9;
		obj.data.b = -999;
		obj.data.c = 3;
		obj.data.d = 6;
		Debug.Log("----write object: " + obj.ToString());
		obj.SerializeWrite(writeSteam);
		writeSteam.Flush();

		int bytesWritten = writeSteam.GetBytesProcessed();
		Debug.Log("bytesWritten: " + bytesWritten);

		TestObject readObject = new TestObject();

		byte[] bufferRead = new byte[BufferSize];
		byte[] serializedBuffer = writeSteam.GetData();
		Array.Copy(serializedBuffer, 0, bufferRead, 0, bytesWritten);

		ReadStream readSteam = new ReadStream(bufferRead, bytesWritten);
		readObject.SerializeRead(readSteam);
		Debug.Log("----read object: " + readObject.ToString());

		check(readObject.Equals(obj));
	}
}

public class TestObject : NObject
{
	private TestData m_data = new TestData();
	public TestData data{get{return m_data;}}

	public override bool SerializeWrite (WriteStream stream)
	{
		this.SerializeInt(stream, m_data.a, -100, 100);
		this.SerializeInt(stream, m_data.b, -1000, 1000);
		this.SerializeBits(stream, m_data.c, 4);
		this.SerializeBits(stream, m_data.d, 8);
		return true;
	}

	public override bool SerializeRead (ReadStream stream)
	{
		m_data.a = this.DeserializeInt(stream, -100, 100);
		m_data.b = this.DeserializeInt(stream, -1000, 1000);
		m_data.c = (byte)this.DeserializeBits(stream, 4);
		m_data.d = (short)this.DeserializeBits(stream, 8);
		return true;
	}

	public override string ToString ()
	{
		return string.Format ("[TestObject: data.a ={0}, data.b ={1},data.c={2},data.d={3}]", data.a, data.b, data.c, data.d);
	}

	public override bool Equals (object obj)
	{
		bool ret = false;
		if(obj is TestObject)
		{
			ret = this.data.Equals(((TestObject)obj).data);
		}
		return ret;
	}
}

public class TestData
{
	public int a;
	public int b;
	public byte c;
	public short d;

	public override bool Equals (object obj)
	{
		bool ret = false;
		if(obj is TestData)
		{
			TestData target = (TestData)obj;
			ret = (this.a == target.a
				&& this.b == target.b
				&& this.c == target.c
				&& this.d == target.d);
		}	
		return ret;
	}
}

