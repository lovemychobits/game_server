#include "header.h"
#include "NetCluster.h"
#include "TimerTrigger.h"
#include "ClientSession.hpp"
#include "ServerSession.hpp"
#include "MsgChainer.h"

namespace cpnet
{
	Logger g_netLog;
	bool g_bStopped = false;

	INetCluster* CreateNetCluster()
	{
		return NetCluster::GetInstance();
	}

	void NetCluster::SetNetThreadNum(uint32_t uThreadNum)
	{
		m_uThreadNum = uThreadNum;
	}

	IClientSession* NetCluster::CreateClientSession()
	{
		ClientSession::pointer pClientSession = ClientSession::Create(m_ioService, m_strand);
		return pClientSession;
	}

	IServerSession* NetCluster::CreateServerSession()
	{
		ServerSession::pointer pServerSession = ServerSession::Create(m_ioService, m_strand);
		return pServerSession;
	}

	ITimerTrigger* NetCluster::CreateTimerTrigger()
	{
		return new TimerTrigger(m_ioService, m_strand);
	}

	IMsgChainer* NetCluster::CreateMsgChainer()
	{
		return new MsgChainer();
	}

	bool NetCluster::Init()
	{
		log4cplus::initialize();
		g_netLog = Logger::getInstance(LOG4CPLUS_TEXT("NETWORK"));
		return true;
	}

	void NetCluster::Run()
	{
		for (uint32_t i=0; i<m_uThreadNum; i++)
		{
			boost::shared_ptr<boost::thread> pThread(new boost::thread(boost::bind(&BoostIoService::run, &m_ioService)));
			m_threadGroup.push_back(pThread);
		}
		m_ioService.run();
	}

	void NetCluster::Stop()
	{
		m_ioService.stop();
		vector<boost::shared_ptr<boost::thread> >::iterator threadIt = m_threadGroup.begin();		
		vector<boost::shared_ptr<boost::thread> >::iterator threadItEnd = m_threadGroup.end();
		for (; threadIt != threadItEnd; threadIt++)
		{
			if ((*threadIt)->get_id() == boost::this_thread::get_id())				// 不能join自己
			{
				continue;
			}
			(*threadIt)->join();													// 可以保证之前需要发送的数据都可以操作完成
		}
		g_bStopped = true;
	}
}
