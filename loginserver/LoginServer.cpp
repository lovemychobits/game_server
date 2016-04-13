#include "header.h"
#include "LoginServer.h"
#include "config/ServerConfig.h"
#include "handlers/MsgChainer.h"
#include "handlers/ClientHandler.h"

// ��������ʼ��
bool LoginServer::Init(const char* pConfPath)
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
	return true;
}

// ��ʼ��log4cplus
bool LoginServer::InitLog4cplus()
{
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("./ls.properties"));
	Logger root = Logger::getRoot();
	return true;
}

// �������������
bool LoginServer::InitServerConf(const char* pConfPath)
{
	return gpServerConfig->LoadServerConf(pConfPath);
}

// ��������ʼ����
bool LoginServer::InitServerApp()
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

	m_pClientHandler = new ClientHandler(this);
	m_pServerSession->SetMsgHandler(m_pClientHandler);

	IMsgChainer* pMsgChainer = m_pNetCluster->CreateMsgChainer();
	pMsgChainer->AddDecodeLast(new ClientMsgDecoder());
	pMsgChainer->AddEncodeLast(new ClientMsgEncoder());
	m_pServerSession->SetMsgChainer(pMsgChainer);

	m_pServerSession->SetHeadLen(12);
	m_pServerSession->Listen(gpServerConfig->GetBindIp(), gpServerConfig->GetListenPort());
	return true;
}