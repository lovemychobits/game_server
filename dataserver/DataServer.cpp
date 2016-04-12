#include "header.h"
#include "DataServer.h"
#include "handlers/GsHandler.h"
#include "mysql/PlayerDataMng.h"
#include "handlers/DsMsgChainer.h"

bool DataServer::Init(const char* pConfPath)
{
	if (!InitLog4cpp())
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


bool DataServer::InitLog4cpp()
{
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("./ds.properties"));
	Logger root = Logger::getRoot();
	return true;
}

bool DataServer::InitServerConf(const char* pConfPath)
{
	return gpServerConfig->LoadServerConf(pConfPath);
}

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