#include "Connection.h"
#include "MsgBuffer.h"
#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffer.hpp>

namespace cpnet
{
	void Connection::Start()
	{
		GetRemoteInfo();
		SetConnected(true);
		StartRead();

		m_pStrand->dispatch(boost::bind(&IMsgHandler::HandleConnect, m_pMsgHandler, this));

	}
	bool Connection::IsConnected()
	{
		return m_bConnect;
	}
	void Connection::GetRemoteInfo()
	{
		if (!m_bGetRemoteInfo)
		{
			try 
			{
				m_strRemoteIp = m_sock.remote_endpoint().address().to_string();
				m_uRemotePort = m_sock.remote_endpoint().port();
				m_bGetRemoteInfo = true;
			}
			catch(boost::system::system_error& err)
			{
				ERROR_NET("get remote info exception, error=[" << err.what() << "]");
			}
		}
	}

	const char* Connection::GetRemoteIp()
	{
		return m_strRemoteIp.c_str();
	}

	uint32_t Connection::GetRemotePort()
	{
		return m_uRemotePort;
	}

	void Connection::Shutdown()
	{
		try 
		{
			m_sock.shutdown(BoostSocket::shutdown_both);
		}
		catch(boost::system::system_error& err)
		{
			ERROR_NET("shutdown exception, error=[" << err.what() << "]");
		}
	}

	void Connection::SetConnected(bool bFlag)
	{
		m_bConnect = bFlag;
	}

	void Connection::SetMsgChainer(IMsgChainer* pMsgChainer)
	{
		m_pMsgChainer = pMsgChainer;
	}

	void Connection::SetMsgHandler(IMsgHandler* pMsgHandler)
	{
		m_pMsgHandler = pMsgHandler;
	}

	void Connection::SetDisconnCallback(const disConnFuncCallBack& _disConnCallback)
	{
		disconnCallback = _disConnCallback;
		m_bForClientSession = true;
	}

	BoostSocket& Connection::socket()
	{
		return m_sock;
	}

	void Connection::SendMsg(const void* pData, uint32_t uLen)
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		boost::shared_ptr<MsgBuffer> msgBuffer = m_pMsgChainer->Encode(this, (const char*)pData, uLen);
		bool bWriteInProcess = !m_msgQueue.empty();
		m_msgQueue.push_back(string(msgBuffer->GetBuf(), msgBuffer->GetLen()));

		if (!bWriteInProcess)								// 如果当前发送队列没有消息正在发送，就直接调用异步发送接口，否则只是将发送消息加入队列
		{
			try
			{
				boost::asio::async_write(m_sock, boost::asio::buffer(m_msgQueue.front().data(), m_msgQueue.front().length()),
					boost::bind(
						&Connection::HandleSend,
						shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred)
					);
			}
			catch(boost::system::system_error& err)
			{
				ERROR_NET("send message exception, error=[" << err.what() << "]");
			}
		}
	}

	// the async_write call back
	void Connection::HandleSend(const boost::system::error_code& error, std::size_t bytes_transferred) 
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);		// 为了保证发送功能是线程安全的
		m_msgQueue.pop_front();
		m_pMsgHandler->HandleWrite(error, bytes_transferred);

		if (!m_msgQueue.empty())
		{
			try 
			{

				boost::asio::async_write(m_sock, boost::asio::buffer(m_msgQueue.front().data(), m_msgQueue.front().length()),
					boost::bind(
						&Connection::HandleSend,
						shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred)
					);
			}
			catch(boost::system::system_error& err)
			{
				ERROR_NET("send message exception, error=[" << err.what() << "]");
			}
		}
	}

	uint32_t Connection::GetMsgSize(const char* pData, uint32_t uLen)
	{
		if (uLen < 4)
		{
			return 0;
		}
		uint32_t* pMsgSize = (uint32_t*)pData;
		return *pMsgSize;
	}

	void Connection::HandleReadHeader(const BoostErrCode& errCode, size_t nBytesTranfered)
	{
		if (g_bStopped || !CheckErrCode(errCode))
		{
			return;
		}
		if (nBytesTranfered < m_nHeadLength)
		{
			ERROR_NET("BytesTranfered=[" << nBytesTranfered << "] is smaller than headLength=[" << m_nHeadLength << "]");
			return;
		}

		const char* pStreamBuf = boost::asio::buffer_cast<const char*>(m_streamBuf.data());
		uint32_t uBodySize = GetMsgSize(pStreamBuf, nBytesTranfered);
		if (uBodySize < m_nHeadLength)
		{
			ERROR_NET("body size is too large or too small, body size =[" << uBodySize << "]");
			DoDisconnect(errCode);
			return;
		}
		try
		{
			if (uBodySize == m_nHeadLength)													// 没有包体部分
			{
				boost::shared_ptr<MsgBuffer> msgBuffer = m_pMsgChainer->Decode(this, pStreamBuf, uBodySize);
				if (msgBuffer->GetLen() == 0)
				{
					ERROR_NET("decode msgbuffer error");
					DoDisconnect(errCode);
				}
				m_pMsgHandler->HandleRecv(this, msgBuffer->GetBuf(), msgBuffer->GetLen());
				m_streamBuf.consume(uBodySize);

				StartRead();
				return;
			}
			boost::asio::async_read(
				m_sock, 
				m_streamBuf,
				boost::asio::transfer_exactly(uBodySize - m_nHeadLength),					// 接收剩下的包体部分
				m_pStrand->wrap(
				boost::bind(&Connection::HandleReadBody, 
				shared_from_this(), 
				boost::asio::placeholders::error, 
				boost::asio::placeholders::bytes_transferred)));
		}
		catch(boost::system::system_error& err)
		{
			ERROR_NET("reve message body exception, error=" << err.what());
		}
	}

	void Connection::HandleReadBody(const BoostErrCode& errCode, size_t nBytesTranfered)
	{
		if (g_bStopped || !CheckErrCode(errCode))
		{
			return;
		}

		const char* pStreamBuf = boost::asio::buffer_cast<const char*>(m_streamBuf.data());
		uint32_t uMsgSize = GetMsgSize(const_cast<char*>(pStreamBuf), nBytesTranfered + m_nHeadLength);
		if (uMsgSize < m_nHeadLength) 
		{
			ERROR_NET("message size is too small, message size =[" << uMsgSize << "]");
			DoDisconnect(errCode);
			return;
		}

		if ((uMsgSize - m_nHeadLength) != nBytesTranfered)
		{
			ERROR_NET("the message size=[" << uMsgSize << "] is not equal bytes transfered");
			DoDisconnect(errCode);
			return;
		}

		// msg chainer decode the msg
		boost::shared_ptr<MsgBuffer> msgBuffer = m_pMsgChainer->Decode(this, pStreamBuf, uMsgSize);
		if (msgBuffer->GetLen() == 0)
		{
			ERROR_NET("decode msgbuffer error");
			DoDisconnect(errCode);
		}
		m_pMsgHandler->HandleRecv(this, msgBuffer->GetBuf(), msgBuffer->GetLen());
		m_streamBuf.consume(uMsgSize);

		// reset recv handle
		StartRead();
	}

	void Connection::StartRead()
	{
		// set read call back function
		try
		{
			boost::asio::async_read(
				m_sock, 
				m_streamBuf,
				boost::asio::transfer_exactly(m_nHeadLength),
				m_pStrand->wrap(
				boost::bind(&Connection::HandleReadHeader, 
				shared_from_this(), 
				boost::asio::placeholders::error, 
				boost::asio::placeholders::bytes_transferred)));

		}
		catch(boost::system::system_error& err)
		{
			ERROR_NET("reve message header exception, error=[" << err.what() << "]");
		}
	}

	// 检查错误码，是否有错误
	bool Connection::CheckErrCode(const BoostErrCode& errCode)
	{
		if (!errCode)
		{
			return true;
		}
		if (boost::asio::error::eof == errCode || boost::asio::error::connection_reset == errCode || boost::asio::error::connection_aborted == errCode)
		{
			SetConnected(false);
			m_sock.close();						// 先关闭连接
			if (m_bForClientSession)
			{
				disconnCallback(this, errCode);
			}
			else
			{
				m_pMsgHandler->HandleDisconnect(this, errCode);
			}
			
			return false;
		}
		else if (boost::asio::error::connection_refused == errCode)			// 连接被拒绝
		{
			ERROR_NET("connect refused.");
			return false;
		}
		else if (boost::asio::error::bad_descriptor == errCode)
		{
			ERROR_NET("bad descriptor");
			return false;
		}
		else
		{
			ERROR_NET("error code=[" << errCode << "]");
			return false;
		}

		return true;
	}

	void Connection::DoDisconnect(const BoostErrCode& errCode)
	{
		m_sock.close();
		m_pMsgHandler->HandleDisconnect(this, errCode);
	}
}
