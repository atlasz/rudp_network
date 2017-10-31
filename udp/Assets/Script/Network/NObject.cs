
public class NObject
{  
	public virtual bool SerializeRead(ReadStream stream ) {return true;}

	public virtual bool SerializeWrite(WriteStream stream ) {return true;}

	public virtual bool SerializeMeasure(MeasureStream stream ) {return true;}
};
