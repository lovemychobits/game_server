#ifndef NETWORK_CONNECTION_H
#define NETWORK_CONNECTION_H

#include "header.h"
#include "IConnection.h"

/*
	wrapper the ASIO connection
*/
namespace cpnet
{
	class MessageThread;
	class Connection : public IConnection, public boost::enable_shared_from_this<Connection>
	{
	public:
		friend class MessageThread;
		typedef boost::shared_ptr<Connection> pointer;

	public:
		static pointer Create(BoostIoService& ioService, BoostStrand& strand, size_t nHeadLength)
		{
			return pointer(boost::shared_ptr<Connection>(new Connection(ioService, strand, nHeadLength)));
		}

		~Connection()
		{
			try 
			{
				m_sock.close();					// 关闭套接字对象
				DoDisconnect(BoostErrCode());	// 通知逻辑层网络连接关闭了，确保逻辑层没有保存被释放了的IConnection指针
			}
			catch(boost::system::system_error& err)
			{
				ERROR_NET("recv message header exception, error=[" << err.what() << "]");
			}
		}

		void Start();

		virtual bool IsConnected();
		virtual const char* GetRemoteIp();
		virtual uint32_t GetRemotePort();
		virtual void Shutdown();

		void SetMsgChainer(IMsgChainer* pMsgChainer);
		void SetMsgHandler(IMsgHandler* pMsgHandler);
		void SetDisconnCallback(const disConnFuncCallBack& disConnCallback);

		BoostSocket& socket();
		void GetRemoteInfo();
		void ShutDown();
		void SendMsg(const void* pData, uint32_t uLen);
	private:
		Connection(BoostIoService& ioService, BoostStrand& strand, size_t nHeadLength) : 
			m_pStrand(NULL), m_sock(ioService), m_bConnect(false), m_nHeadLength(nHeadLength), m_uRemotePort(0), m_bGetRemoteInfo(false), m_bForClientSession(false)
		{
			m_pStrand = &strand;
		}

		void SetConnected(bool bFlag);					// 设置连接状态

		// the async_write call back
		void HandleSend(const boost::system::error_code& error, std::size_t bytes_transferred);
		void StartRead();

		void HandleReadBody(const BoostErrCode& errCode, size_t nBytesTranfered);
		void HandleReadHeader(const BoostErrCode& errCode, size_t nBytesTranfered);
		uint32_t GetMsgSize(const char* pData, uint32_t uLen);									// 获取消息包大小

		// 检查错误码，是否有错误
		bool CheckErrCode(const BoostErrCode& errCode);
		void DoDisconnect(const BoostErrCode& errCode);

	private:
		BoostStrand* m_pStrand;
		BoostSocket m_sock;								// the connection socket
		bool m_bConnect;								// 是否已经连接
		boost::asio::streambuf m_streamBuf;
		
		IMsgChainer* m_pMsgChainer;
		IMsgHandler* m_pMsgHandler;
		deque<string> m_msgQueue;

		size_t m_nHeadLength;							// 消息头的大小

		string m_strRemoteIp;
		uint32_t m_uRemotePort;
		bool m_bGetRemoteInfo;

		bool m_bForClientSession;						// 作为客户端的连接
		disConnFuncCallBack disconnCallback;			// 断开连接的回调，只有m_bForClientSession为true的时候才使用

		boost::mutex m_mutex;
	};
}
#endif 