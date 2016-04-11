#ifndef NETWORK_CLIENTOBJ_H
#define NETWORK_CLIENTOBJ_H

#include "header.h"
#include "INetCluster.h"
#include "Connection.h"
#include "IMsgParser.h"
#include "IMsgHandler.h"
#include "NetCluster.h"
#include "IMsgChainer.h"

namespace cpnet
{
	class ClientSession : public IClientSession
	{
	public:
		typedef ClientSession* pointer;
		static pointer Create(BoostIoService& ioService, BoostStrand& strand)
		{
			return pointer(new ClientSession(ioService, strand));
		}
		virtual ~ClientSession()
		{
			if (m_pTimerTrigger)
			{
				delete m_pTimerTrigger;
			}
			ERROR_NET("~ClientSession");
		}
		
		// 连接指定服务器
		virtual void Connect(const char* pServerIp, uint32_t uPort)
		{
			m_strIp = pServerIp;
			m_uPort = uPort;

			m_pConnection = Connection::Create(*m_pIoService, *m_pStrand, m_uHeadSize);
			m_pConnection->SetMsgChainer(m_pMsgChainer);
			m_pConnection->SetMsgHandler(m_pMsgHandler);
			m_pConnection->SetDisconnCallback(boost::bind(&ClientSession::HandleDisconnect, this, _1, _2));
		
			// 开始连接
			DoReConnect();
		}

		virtual void SetMsgChainer(IMsgChainer* pMsgChainer)
		{
			m_pMsgChainer = pMsgChainer;
		}

		virtual void SetMsgHandler(IMsgHandler* pMsgHandler)
		{
			m_pMsgHandler = pMsgHandler;
		}

		virtual void SetHeadLen(uint32_t uHeadLen)
		{
			m_uHeadSize = uHeadLen;
		}

		virtual void SendMsg(const char* pBuf, uint32_t uLen)
		{
			m_pConnection->SendMsg(pBuf, uLen);
		}

		virtual void Stop()
		{
			m_pConnection->Shutdown();
		}

		virtual bool IsConnected()
		{
			return m_pConnection->IsConnected();
		}

	private:
		ClientSession(BoostIoService& ioService, BoostStrand& strand) : m_pStrand(NULL), m_pTimerTrigger(NULL), m_uHeadSize(0)
		{
			m_pIoService = &ioService;
			m_pStrand = &strand;
			InitReconnectTimer();
		}

		void DoReConnect()
		{
			BoostEndPoint endpoint(BoostAddress::from_string(m_strIp), m_uPort);
			BoostErrCode errCode;
			if (m_pConnection->socket().is_open())
			{
				m_pConnection->socket().close(errCode);						// 先关闭当前连接，然后重连
			}
			m_pConnection->socket().async_connect(endpoint, m_pStrand->wrap(boost::bind(&ClientSession::HandleConnect, this, boost::asio::placeholders::error)));
		}

		void HandleConnect(const BoostErrCode& err)
		{
			if (err)
			{
				ERROR_NET("ip=[" << m_strIp.c_str() << "], port=[" << m_uPort << "]" << err.message().c_str());
				OnTimerConnect();
				return;
			}
			m_pConnection->Start();
		}

		void HandleDisconnect(IConnection* pConnection, const BoostErrCode& err)
		{
			m_pMsgHandler->HandleDisconnect(pConnection, err);
			OnTimerConnect();	
		}

		void OnTimerConnect()
		{
			m_pTimerTrigger->AddOnceTimer(boost::bind(&ClientSession::DoReConnect, this), 3);			// 每隔3秒重连一次
		}

		void InitReconnectTimer()
		{
			m_pTimerTrigger = NetCluster::GetInstance()->CreateTimerTrigger();
			if (!m_pTimerTrigger)
			{
				return;
			}
		}

	private:
		Connection::pointer m_pConnection;					// 网络连接对象
		BoostIoService* m_pIoService;						// ASIO核心对象
		BoostStrand* m_pStrand;								// ASIO对象同步
		IMsgParser* m_pMsgParser;
		IMsgChainer* m_pMsgChainer;							// 消息处理链
		IMsgHandler* m_pMsgHandler;							// 消息处理对象
		ITimerTrigger* m_pTimerTrigger;						// 定时器对象

		uint32_t m_uHeadSize;								// 消息头长度

		string m_strIp;										// 连接目标的IP
		uint32_t m_uPort;									// 连接目标的端口
	};
}

#endif