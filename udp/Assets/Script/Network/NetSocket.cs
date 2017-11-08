using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Net;
using System;

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
	private IPEndPoint m_svrEndPoint;
	private EndPoint m_bufferEndPoint;

	public NetSocket(NetSocketType socketType)
	{
		m_ipv4Socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
		m_svrEndPoint = new IPEndPoint(IPAddress .Parse (NetworkConfig.address) , NetworkConfig.port);
		m_bufferEndPoint = new IPEndPoint(IPAddress.Any, 0);
	}

	public int SendData(byte[] data, int offset, int size)
	{
		int result = 0;
		result = m_ipv4Socket.SendTo(data, offset, size, SocketFlags.None, m_svrEndPoint);
		return result;
	}

	public void ReceieveData(ref byte[] data)
	{
		m_ipv4Socket.ReceiveFrom(data, 0, data.Length, SocketFlags.None, ref m_bufferEndPoint);
	}

	public void Close()
	{
		m_ipv4Socket.Close();
	}
}
