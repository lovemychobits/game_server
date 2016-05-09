using UnityEngine;
using System.Collections;

public class GameClient : System.IDisposable
{
    private SocketClient m_socketClient;
    private MsgMng m_msgManager;

    public GameClient()
    {
        m_socketClient = new SocketClient("127.0.0.1", 8500, ProcessResponse);
		m_msgManager = MsgMng.getInstace();
    }

    public bool Connect()
    {
        return m_socketClient.Connect(OnConnectComp);
    }

    private void OnConnectComp(bool is_success)
    {
        if (is_success)
        {
            m_socketClient.StartReceive();
        }
    }

    public void Request(int command, byte[] message, int message_size)
    {
        m_socketClient.Send(message, message_size);
    }

    public void Request(int command, byte[] message, int message_size, System.Action<int, byte[]> action)
    {
        m_socketClient.Send(message, message_size);
    }

    public void ProcessResponse(int command, byte[] data)
    {
        //Debug.Log("FUN >>> ProcessResponse");
		MsgMng.ProcMsg (command, data);
    }

    public void Disconnect()
    {
        Dispose();
    }

    public void Dispose()
    {
        m_socketClient.Close();
        System.GC.SuppressFinalize(this);
    }

    public bool IsClosed()
    {
        if (m_socketClient == null) return true;
        return m_socketClient.IsClosed;
    }
}
