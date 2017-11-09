using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Net;
using System;
using UnityEngine;
using System.Threading;

public enum NetSocketType
{
	SOCKET_TYPE_IPV4,
	SOCKET_TYPE_IPV6,
}

public class NetworkConfig
{
	public static string address = "127.0.0.1";
	public static int port = 8888;
}
public class NetSocket
{
	private Socket m_ipv4Socket;
	private Thread m_threadv4;

	private IPEndPoint m_svrEndPoint;
	private EndPoint m_bufferEndPoint;
	private Action<byte[]> m_receiveProcessor;

	private bool m_isRunning = false;

	public NetSocket(NetSocketType socketType)
	{
		m_ipv4Socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
		m_svrEndPoint = new IPEndPoint(IPAddress .Parse (NetworkConfig.address) , NetworkConfig.port);
		m_bufferEndPoint = new IPEndPoint(IPAddress.Any, 0);

		m_isRunning = true;

		m_threadv4 = new Thread(ReceieveData);
		m_threadv4.Name = "threadv4(" + NetworkConfig.port + ")";
		m_threadv4.IsBackground = true;
		m_threadv4.Start();
	}
		
	public int SendData(byte[] data, int offset, int size)
	{
		int result = 0;
		result = m_ipv4Socket.SendTo(data, offset, size, SocketFlags.None, m_svrEndPoint);
		return result;
	}

	private void ReceieveData()
	{
		//Debug.Log("ReceieveData start");
		while(m_isRunning)
		{
			if(!m_ipv4Socket.Poll(100000, SelectMode.SelectRead))
			{
				continue;
			}

			byte[] data = new byte[1024];

			try
			{
				//Reading data
				m_ipv4Socket.ReceiveFrom(data, 0, data.Length, SocketFlags.None, ref m_bufferEndPoint);

			}
			catch(System.Exception ex)
			{
				Debug.LogError(ex.StackTrace);
			}
			//
			m_receiveProcessor(data);
		}
		//Debug.Log("ReceieveData end");
	}

	public void Close()
	{
		m_isRunning = false;
		if(Thread.CurrentThread != m_threadv4)
		{
			m_threadv4.Join();
		}
		m_threadv4 = null;

		m_ipv4Socket.Close();
	}

	public void AddReceiveProcessor( Action<byte[]> call)
	{
		m_receiveProcessor -= call;
		m_receiveProcessor += call;
	}

	public void RemoveReceiveProcessor( Action<byte[]> call)
	{
		m_receiveProcessor -= call;
	}
}
