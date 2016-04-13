#include "header.h"
#include "DataServer.h"
#include "handlers/GsHandler.h"
#include "mysql/PlayerDataMng.h"
#include "handlers/DsMsgChainer.h"

// 服务器初始化
bool DataServer::Init(const char* pConfPath)
{
	if (!InitLog4cplus())
	{
		ERRORLOG("init log fail!");
		return false;
	}
	if (!InitServerConf(pConfPath))
	{
		ERRORLOG("init server conf fail!");
		return false;
	}
	if (!InitServerApp())
	{
		ERRORLOG("init server app fail!");
		return false;
	}
	if (!InitDataThread())
	{
		ERRORLOG("init data thread fail!");
		return false;
	}
	return true;
}

// 初始化log4cplus
bool DataServer::InitLog4cplus()
{
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("./ds.properties"));
	Logger root = Logger::getRoot();
	return true;
}

// 载入服务器配置
bool DataServer::InitServerConf(const char* pConfPath)
{
	return gpServerConfig->LoadServerConf(pConfPath);
}

// 服务器开始监听
bool DataServer::InitServerApp()
{
	m_pNetCluster = CreateNetCluster();
	if (!m_pNetCluster)
	{
		return false;
	}
	if (!m_pNetCluster->Init())
	{
		return false;
	}
	m_pServerSession = m_pNetCluster->CreateServerSession();
	if (!m_pServerSession)
	{
		return false;
	}

	m_pGsHandler = new GsHandler(this);
	m_pServerSession->SetMsgHandler(m_pGsHandler);	

	IMsgChainer* pMsgChainer = m_pNetCluster->CreateMsgChainer();
	pMsgChainer->AddDecodeLast(new ClientMsgDecoder());
	pMsgChainer->AddEncodeLast(new ClientMsgEncoder());
	m_pServerSession->SetMsgChainer(pMsgChainer);

	m_pServerSession->SetHeadLen(12);
	m_pServerSession->Listen(gpServerConfig->GetBindIp(), gpServerConfig->GetListenPort());
	return true;
}

// 设置数据处理线程
bool DataServer::InitDataThread()
{
	int nThreadNum = gpServerConfig->GetThreadNum();
	m_pDataThread = new DataThread*[nThreadNum];
	m_pThread = new boost::thread*[nThreadNum];
	for (int i=0; i<nThreadNum; i++)
	{
		m_pDataThread[i] = new DataThread;
		if (!m_pDataThread[i] || !m_pDataThread[i]->Init())
		{
			return false;
		}
		m_pThread[i] = new boost::thread(boost::bind(&DataThread::Run, m_pDataThread[i]));
		if (!m_pThread[i])
		{
			return false;
		}
	}
	return true;
}