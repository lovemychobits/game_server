//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

// Generated from: client.proto
namespace client
{
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"PBVector")]
  public partial class PBVector : global::ProtoBuf.IExtensible
  {
    public PBVector() {}
    
    private float _x = default(float);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"x", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    [global::System.ComponentModel.DefaultValue(default(float))]
    public float x
    {
      get { return _x; }
      set { _x = value; }
    }
    private float _y = default(float);
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"y", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    [global::System.ComponentModel.DefaultValue(default(float))]
    public float y
    {
      get { return _y; }
      set { _y = value; }
    }
    private float _z = default(float);
    [global::ProtoBuf.ProtoMember(3, IsRequired = false, Name=@"z", DataFormat = global::ProtoBuf.DataFormat.FixedSize)]
    [global::System.ComponentModel.DefaultValue(default(float))]
    public float z
    {
      get { return _z; }
      set { _z = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"EnterGameReq")]
  public partial class EnterGameReq : global::ProtoBuf.IExtensible
  {
    public EnterGameReq() {}
    
    private string _userName = "";
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"userName", DataFormat = global::ProtoBuf.DataFormat.Default)]
    [global::System.ComponentModel.DefaultValue("")]
    public string userName
    {
      get { return _userName; }
      set { _userName = value; }
    }
    private string _passwd = "";
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"passwd", DataFormat = global::ProtoBuf.DataFormat.Default)]
    [global::System.ComponentModel.DefaultValue("")]
    public string passwd
    {
      get { return _passwd; }
      set { _passwd = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"EnterGameAck")]
  public partial class EnterGameAck : global::ProtoBuf.IExtensible
  {
    public EnterGameAck() {}
    
    private uint _errorCode = default(uint);
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"errorCode", DataFormat = global::ProtoBuf.DataFormat.TwosComplement)]
    [global::System.ComponentModel.DefaultValue(default(uint))]
    public uint errorCode
    {
      get { return _errorCode; }
      set { _errorCode = value; }
    }
    private client.PBVector _pos = null;
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"pos", DataFormat = global::ProtoBuf.DataFormat.Default)]
    [global::System.ComponentModel.DefaultValue(null)]
    public client.PBVector pos
    {
      get { return _pos; }
      set { _pos = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"QueryPathReq")]
  public partial class QueryPathReq : global::ProtoBuf.IExtensible
  {
    public QueryPathReq() {}
    
    private client.PBVector _startPos = null;
    [global::ProtoBuf.ProtoMember(1, IsRequired = false, Name=@"startPos", DataFormat = global::ProtoBuf.DataFormat.Default)]
    [global::System.ComponentModel.DefaultValue(null)]
    public client.PBVector startPos
    {
      get { return _startPos; }
      set { _startPos = value; }
    }
    private client.PBVector _endPos = null;
    [global::ProtoBuf.ProtoMember(2, IsRequired = false, Name=@"endPos", DataFormat = global::ProtoBuf.DataFormat.Default)]
    [global::System.ComponentModel.DefaultValue(null)]
    public client.PBVector endPos
    {
      get { return _endPos; }
      set { _endPos = value; }
    }
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
  [global::System.Serializable, global::ProtoBuf.ProtoContract(Name=@"QueryPathAck")]
  public partial class QueryPathAck : global::ProtoBuf.IExtensible
  {
    public QueryPathAck() {}
    
    private readonly global::System.Collections.Generic.List<client.PBVector> _path = new global::System.Collections.Generic.List<client.PBVector>();
    [global::ProtoBuf.ProtoMember(1, Name=@"path", DataFormat = global::ProtoBuf.DataFormat.Default)]
    public global::System.Collections.Generic.List<client.PBVector> path
    {
      get { return _path; }
    }
  
    private global::ProtoBuf.IExtension extensionObject;
    global::ProtoBuf.IExtension global::ProtoBuf.IExtensible.GetExtensionObject(bool createIfMissing)
      { return global::ProtoBuf.Extensible.GetExtensionObject(ref extensionObject, createIfMissing); }
  }
  
    [global::ProtoBuf.ProtoContract(Name=@"ClientProtocol")]
    public enum ClientProtocol
    {
            
      [global::ProtoBuf.ProtoEnum(Name=@"REQ_ENTER_GAME", Value=2000)]
      REQ_ENTER_GAME = 2000,
            
      [global::ProtoBuf.ProtoEnum(Name=@"REQ_MOVE", Value=2001)]
      REQ_MOVE = 2001,
            
      [global::ProtoBuf.ProtoEnum(Name=@"REQ_QUERY_PATH", Value=2002)]
      REQ_QUERY_PATH = 2002
    }
  
    [global::ProtoBuf.ProtoContract(Name=@"LoginError")]
    public enum LoginError
    {
            
      [global::ProtoBuf.ProtoEnum(Name=@"SUCCESS", Value=0)]
      SUCCESS = 0
    }
  
}