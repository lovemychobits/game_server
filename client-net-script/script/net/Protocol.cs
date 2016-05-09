using UnityEngine;
using System.Collections;

public class Protocol
{
    private byte[] m_buffer;
    public byte[] Buffer
    {
        get { return m_buffer; }
    }

    private const int HEAD_LENGTH = 12;

    public Protocol()
    {
        m_buffer = new byte[8192];
    }

    public int MessageObj2Bytes<T>(T message_obj, int message_command)
    {
        System.IO.MemoryStream stream = new System.IO.MemoryStream();
        ProtoBuf.Serializer.Serialize(stream, message_obj);
        int messageSize = HEAD_LENGTH + (int)stream.Length;
        MessageHeader.Header header = new MessageHeader.Header(messageSize, message_command);
        Debug.Log("header : " + header.ToString());
        byte[] headerBytes = ObjectBytesTrans.StructToBytes(header);
        int headerBytesLength = headerBytes.Length;
        System.Array.Copy(headerBytes, 0, m_buffer, 0, headerBytesLength);
        byte[] bodyBytes = stream.GetBuffer();
        long streamLength = stream.Length;
        System.Array.Copy(bodyBytes, 0, m_buffer, headerBytesLength, streamLength);
        int bytesLength = (int)(headerBytesLength + streamLength);
        return bytesLength;
    }
}
