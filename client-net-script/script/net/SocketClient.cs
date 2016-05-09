using System.Security.Cryptography;
using UnityEngine;
using System;
using System.Collections;
using System.Net;
using System.Net.Sockets;

public class StateObject
{
    private byte[] m_buffer = new byte[1024];
    public byte[] Buffer
    {
        get { return m_buffer; }
    }
}

public class SocketClient
{
    private TcpClient m_tcpClient;
    private NetworkStream m_stream;
    private byte[] m_buffer = new byte[8192];
    private bool m_isClosed = true;
    public bool IsClosed
    {
        get { return m_isClosed; }
    }

    private IAsyncResult m_sendAsyncResult;
    private IAsyncResult m_receiveAsyncResult;

    private const int HEAD_LENGTH = 12;
    private int m_bodyLength;

    private System.Action<int, byte[]> m_dataProcesser = null;

    private int m_command;

    private int m_receiveBodyLengthAccumulation = 0;
    private byte[] m_receiveBodyAccumulation;

    public SocketClient(System.Action<int, byte[]> action)
    {
        m_tcpClient = new TcpClient();
        m_dataProcesser = action;
    }

	private string ip;
	private int port;
	public SocketClient(string _ip, int port, System.Action<int, byte[]> action) {
		this.ip = _ip;
		this.port = port;
		m_tcpClient = new TcpClient();
		m_dataProcesser = action;
	}

	public bool Connect(System.Action<bool> action)
    {
        bool retValue;
        IPAddress ipAddress = IPAddress.Parse(ip);
        IPEndPoint point = new IPEndPoint(ipAddress, port);
        try
        {
            m_tcpClient.Connect(point);
        }
        catch
        {
            retValue = false;
        }
        retValue = m_tcpClient.Connected;

        if (retValue)
        {
            m_isClosed = false;
            m_stream = m_tcpClient.GetStream();
        }
		action(retValue);
        return retValue;
    }

    public void Send(byte[] data, int message_size)
    {
        if (m_stream == null || !m_stream.CanWrite)
        {
            Debug.LogError("Error >>> NetworkStream == Null or NetworkStream Can't Write!!!");
            return;
        }

        AsyncCallback callback = new AsyncCallback(OnSendComp);

        /*byte[] encryptData = AES.AESHelper.AESEncrypt(data);
        byte[] decryptData = AES.AESHelper.AESDecrypt(encryptData);*/
        
        Debug.Log("CALL >>> BeginWrite");
        m_stream.BeginWrite(data, 0, message_size, callback, null);
    }

    private void OnSendComp(IAsyncResult result)
    {
        m_stream.EndWrite(result);
        Debug.Log("CALL >>> EndWrite");
    }

    public void StartReceive()
    {
        m_receiveBodyLengthAccumulation = 0;
        m_receiveBodyAccumulation = null;
        ReceiveHead();
    }

    private void ReceiveHead()
    {
        if (!m_stream.CanRead) return;

        Debug.Log("FUN >>> ReceiveHead");
        AsyncCallback callback = new AsyncCallback(OnReceiveHeadComp);
        try
        {
            m_stream.BeginRead(m_buffer, 0, HEAD_LENGTH, callback, null);
        }
        catch (Exception e)
        {
            Debug.Log("!Catch exception, " + e);
        }
    }

    private void OnReceiveHeadComp(IAsyncResult result)
    {
        Debug.Log("FUN >>> OnReceiveHeadComp");
        if (m_isClosed) return;
        int receiveLength = 0;
        try
        {
            if (m_stream != null)
            {
                receiveLength = m_stream.EndRead(result);
            }
        }
        catch (Exception e)
        {
            Debug.Log("!Catch exception, " + e);
        }

        //Debug.Log("receiveLength = " + receiveLength);
        if (receiveLength <= 0)
        {
            Close();
            return;
        }

        MessageHeader.Header header = (MessageHeader.Header)ObjectBytesTrans.BytesToStruct(m_buffer, typeof(MessageHeader.Header));
        int bodyLength = header.messageSize - HEAD_LENGTH;
        Debug.Log("bodyLength = " + bodyLength);

#if UNITY_EDITOR
        if (bodyLength > m_buffer.Length)
            Debug.LogError(">>> receive data length exceeds buffer length!");
#endif

        m_bodyLength = bodyLength;
        m_command = header.messageCommand;
        if (bodyLength > 0)
        {
            ReceiveBody(bodyLength, HEAD_LENGTH);
        }
        else
        {
			if (m_dataProcesser != null) {
				m_dataProcesser(m_command, m_receiveBodyAccumulation);
			}
            ReceiveHead();
        }
    }

    private void ReceiveBody(int length, int offset, bool bFirstReceive = true)
    {
        //> DataAvailable标示在包头，所以第一次读取数据需要判断DataAvailable
        //> 非第一次读取则不能判断DataAvailable，此时DataAvailable == false.
        if (!m_stream.CanRead || (bFirstReceive && !m_stream.DataAvailable))
        {
            Debug.LogError(">>> FUN ReceiveBody, stream is not readable, or data is not available.");
            //return;
        }

        Debug.Log("FUN >>> ReceiveBody");
        AsyncCallback callback = new AsyncCallback(OnReceiveBodyComp);
        try
        {
            m_stream.BeginRead(m_buffer, offset, length, callback, null);
        }
        catch (Exception e)
        {
            Debug.Log("!Catch exception, " + e);
        }
    }

    private void OnReceiveBodyComp(IAsyncResult result)
    {
        Debug.Log("FUN >>> OnReceiveBodyComp");
        if (m_isClosed) return;
        int receiveLength = m_stream.EndRead(result);
        Debug.Log("receiveLength = " + receiveLength);
        if (receiveLength > 0)
        {
            if (m_receiveBodyAccumulation == null)
            {
                m_receiveBodyAccumulation = new byte[m_bodyLength];
            }
            WriteBytes(m_buffer, HEAD_LENGTH + m_receiveBodyLengthAccumulation, receiveLength, m_receiveBodyAccumulation, m_receiveBodyLengthAccumulation);
            m_receiveBodyLengthAccumulation += receiveLength;
        }

		Debug.Log ("m_receiveBodyLengthAccumulation" + m_receiveBodyLengthAccumulation);
		Debug.Log ("m_bodyLength" + m_bodyLength);
        if (m_receiveBodyLengthAccumulation < m_bodyLength)
        {
            ReceiveBody(m_bodyLength - m_receiveBodyLengthAccumulation, HEAD_LENGTH + m_receiveBodyLengthAccumulation, false);
        }
        else
        {
            if (m_dataProcesser != null)
                m_dataProcesser(m_command, m_receiveBodyAccumulation);
            StartReceive();
        }
    }

    public void Close()
    {
        Debug.Log("FUN >>> Close");
        if (m_isClosed) return;
        m_tcpClient.Close();
        m_stream.Dispose();
        m_isClosed = true;
    }

    public byte[] WriteBytes(byte[] source_bytes, int source_start_index, int length, byte[] target_bytes, int target_start_index)
    {
        if (target_bytes == null) return null;
        if (source_bytes == null || source_bytes.Length == 0) return target_bytes;
        for (int i = 0; i < length; i++)
        {
            target_bytes[target_start_index + i] = source_bytes[source_start_index + i];
        }
        return target_bytes;
    }
}
