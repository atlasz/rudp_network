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
		BitPackerTest();
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
		obj.Init();

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

	public void Init()
	{
		m_data.intA = -9;
		m_data.intB = -999;
		m_data.byteC = 3;
		m_data.shortD = 6;
		m_data.boolE = false;
		m_data.floatF = 3.1415926f;
		m_data.uint64G = UInt64.MaxValue;
		m_data.doubleH = double.MinValue;
		for(int i = 0; i < m_data.bytesI.Length; ++i)
		{
			m_data.bytesI[i] = (byte)UnityEngine.Random.Range(0, 255);
		}
	}

	public override bool SerializeWrite (WriteStream stream)
	{
		this.SerializeInt(stream, m_data.intA, -100, 100);
		this.SerializeInt(stream, m_data.intB, -1000, 1000);
		this.SerializeBits(stream, m_data.byteC, 4);
		this.SerializeBits(stream, m_data.shortD, 8);
		this.SerializeAlign(stream);
		this.SerializeBool(stream, m_data.boolE);
		this.SerializeFloat(stream, m_data.floatF);
		this.SerializeUInt64(stream, m_data.uint64G);
		this.SerializeDouble(stream, m_data.doubleH);
		this.SerializeBytes(stream, m_data.bytesI, m_data.bytesI.Length);
		return true;
	}

	public override bool SerializeRead (ReadStream stream)
	{
		m_data.intA = this.DeserializeInt(stream, -100, 100);
		m_data.intB = this.DeserializeInt(stream, -1000, 1000);
		m_data.byteC = (byte)this.DeserializeBits(stream, 4);
		m_data.shortD = (short)this.DeserializeBits(stream, 8);
		this.SerializeAlign(stream);
		m_data.boolE = this.DeserializeBool(stream);
		m_data.floatF = this.DeserializeFloat(stream);
		m_data.uint64G = this.DeserializeUInt64(stream);
		m_data.doubleH = this.DeserializeDouble(stream);
		this.DeserializeBytes(stream, ref m_data.bytesI, m_data.bytesI.Length);
		return true;
	}

	public override string ToString ()
	{
		return string.Format ("[TestObject: data.intA={0}, data.intB={1},data.byteC={2}," +
			"data.shortD={3}, data.boolE={4}, data.floatF={5}, data.uint64G{6}, data.doubleH{7}" +
			",data.bytesI={8}]", 
			data.intA, data.intB, data.byteC, data.shortD, data.boolE,data.floatF,data.uint64G,
			data.doubleH, data.bytesI);
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
	public int intA;
	public int intB;
	public byte byteC;
	public short shortD;
	public bool boolE;
	public float floatF;
	public UInt64 uint64G;
	public double doubleH;
	public byte[] bytesI = new byte[17];

	public override bool Equals (object obj)
	{
		bool ret = false;
		if(obj is TestData)
		{
			TestData target = (TestData)obj;

			bool bytesEq = false;

			if(this.bytesI.Length == target.bytesI.Length)
			{
				bytesEq = true;
			}
			if(bytesEq)
			{
				for(int i = 0 ; i < this.bytesI.Length; ++i)
				{
					if(this.bytesI[i] != target.bytesI[i])
					{
						bytesEq = false;
						break;
					}
				}
			}

			ret = (this.intA == target.intA
				&& this.intB == target.intB
				&& this.byteC == target.byteC
				&& this.shortD == target.shortD
				&& this.boolE == target.boolE
				&& this.floatF == target.floatF
				&& this.uint64G == target.uint64G
				&& this.doubleH == target.doubleH
				&& bytesEq);
		}	
		return ret;
	}
}

