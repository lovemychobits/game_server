#ifndef	NETWORK_SERVERAPP_H
#define NETWORK_SERVERAPP_H

#include "header.h"
#include "IMsgParser.h"
#include "Connection.h"
#include "ITimerTrigger.h"
#include "TimerTrigger.h"
#include "INetCluster.h"
#include "IMsgHandler.h"
#include "IMsgChainer.h"

namespace cpnet
{
	class ServerSession : public IServerSession
	{
	public:
		typedef ServerSession* pointer;
		static pointer Create(BoostIoService& ioService, BoostStrand& strand)
		{
			return new ServerSession(ioService, strand);
		}

		virtual void Listen(const char* pHostIp, uint32_t uPort)
		{
			try 
			{
				BoostEndPoint endPoint(BoostAddress::from_string(pHostIp), uPort);
				m_acceptor.open(endPoint.protocol());
				m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
				m_acceptor.bind(endPoint);
				m_acceptor.listen();
			}
			catch(boost::system::system_error e)
			{
				ERROR_NET("server listen exception, err=[" << e.what() << "]");
			}

			Connection::pointer pNewConnection = Connection::Create(*m_pIoService, *m_pStrand, m_uHeadSize);
			m_acceptor.async_accept(pNewConnection->socket(), m_pStrand->wrap(boost::bind(&ServerSession::HandleAccept, this, pNewConnection, boost::asio::placeholders::error)));
			TRACE_NET("server listen start, ip=[" << pHostIp << "], port=[" << uPort << "]");

			return;
		}

		virtual void SetMsgChainer(IMsgChainer* pMsgChainer)
		{
			m_pMsgChainer = pMsgChainer;
		}

		virtual void SetMsgHandler(IMsgHandler* pMsgHandler)
		{
			m_pMsgHandler = pMsgHandler;
		}

		virtual void SetHeadLen(uint32_t uHeadSize)
		{
			m_uHeadSize = uHeadSize;
		}

		virtual void Stop()
		{
			m_acceptor.close();
		}

	private:
		ServerSession(BoostIoService& ioService, BoostStrand& strand) : m_acceptor(ioService), m_uHeadSize(0)
		{
			m_pIoService = &ioService;
			m_pStrand = &strand;
		}
		virtual ~ServerSession()
		{
		}

		void HandleAccept(Connection::pointer pConnection, const boost::system::error_code& error)
		{
			if (error)
			{
				ERROR_NET("server app handle accept error=[" << error.message().c_str() << "]");
				return;
			}

			// 设置回调函数，以及消息处理接口
			pConnection->SetMsgChainer(m_pMsgChainer);
			pConnection->SetMsgHandler(m_pMsgHandler);
			pConnection->Start();

			Connection::pointer pNewConnection = Connection::Create(*m_pIoService, *m_pStrand, m_uHeadSize);
			m_acceptor.async_accept(pNewConnection->socket(), boost::bind(&ServerSession::HandleAccept, this, pNewConnection, boost::asio::placeholders::error));
		}

	private:
		BoostIoService* m_pIoService;
		BoostStrand* m_pStrand;
		BoostAcceptor m_acceptor;
		IMsgParser* m_pMsgParser;
		IMsgChainer* m_pMsgChainer;
		IMsgHandler* m_pMsgHandler;

		uint32_t m_uHeadSize;
	};
}

#endif