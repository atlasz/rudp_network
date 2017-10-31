
public enum SteamStateEnum
{
	Write = 0,
	Read = 1,
	Measure = 2,
}

public class BaseStream
{
	protected SteamStateEnum m_streamType = SteamStateEnum.Read;
	public SteamStateEnum streamType {get{return m_streamType;}}
}
