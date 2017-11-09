
public enum GameCMD
{
	RUDP_CMD_DATA = 100,
	RUDP_CMD_CONNECTION_START_REQ = 101,
	RUDP_CMD_CONNECTION_START_RSP = 102,
	RUDP_CMD_CONNECTION_STOP_REQ = 103,
	RUDP_CMD_CONNECTION_STOP_RSP = 104,
	RUDP_CMD_HEARTBEAT = 105,
	TEST_PACKET_NUM_TYPES
};

public class ConnectionStartReq : NObject
{
	public byte cmd;
	public ushort sid;
	public ulong uid;

	public override bool SerializeWrite (WriteStream stream)
	{
		this.SerializeBits(stream, cmd, 8);
		this.SerializeBits(stream, sid, 16);
		this.SerializeUInt64(stream, uid);
		return true;
	}

	public override bool SerializeRead (ReadStream stream)
	{
		cmd = (byte)this.DeserializeBits(stream, 8);
		sid = (ushort)this.DeserializeBits(stream, 16);
		uid = this.DeserializeUInt64(stream);
		return true;
	}

	public override string ToString ()
	{
		return string.Format ("[ConnectionStartReq cmd{0} sid{1} uid{2}]", cmd, sid, uid);
	}
}

public class ConnectionStartRsp : NObject
{
	public byte cmd;
	public ushort sid;

	public override bool SerializeWrite (WriteStream stream)
	{
		this.SerializeBits(stream, cmd, 8);
		this.SerializeBits(stream, sid, 16);
		return true;
	}

	public override bool SerializeRead (ReadStream stream)
	{
		cmd = (byte)this.DeserializeBits(stream, 8);
		sid = (ushort)this.DeserializeBits(stream, 16);
		return true;
	}

	public override string ToString ()
	{
		return string.Format ("[ConnectionStartReq cmd{0} sid{1}]", cmd, sid);
	}
}
