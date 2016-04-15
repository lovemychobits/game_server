#include "LobbyServer.h"
#include "handlers/GameServerHandler.h"
#include "gsgroup/GameGroupMng.h"
#include "handlers/ClientMsgHandler.h"
#include "handlers/LobbyMsgChainer.h"

bool LobbyServer::Init(const char* pConfPath)
{
	if (!InitLog4cpp())
	{
		return false;
	}
	if (!InitServerConf(pConfPath))
	{
		return false;
	}
	if (!InitServerApp())
	{
		return false;
	}
	return true;
}


bool LobbyServer::InitLog4cpp()
{
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("./lobby.properties"));
	Logger root = Logger::getRoot();

	return true;
}

bool LobbyServer::InitServerConf(const char* pConfPath)
{
	return gpServerConfig->LoadServerConf(pConfPath);
}

bool LobbyServer::InitServerApp()
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

	IMsgChainer* pMsgChainer = m_pNetCluster->CreateMsgChainer();
	pMsgChainer->AddDecodeLast(new ClientMsgDecoder());
	pMsgChainer->AddEncodeLast(new ClientMsgEncoder());

	GameServerHandler* pGsHandler = new GameServerHandler(this);
	GameGroupMng* pGameGroupMng = new GameGroupMng;
	pGameGroupMng->LoadGameGroupConf();
	pGsHandler->SetGroupMng(pGameGroupMng);
	m_pServerSession->SetMsgHandler(pGsHandler);
	m_pServerSession->SetMsgChainer(pMsgChainer);
	m_pServerSession->SetHeadLen(12);

	m_pServerSession->Listen(gpServerConfig->GetBindIp(), gpServerConfig->GetListenPort());

	m_pClientSession = m_pNetCluster->CreateServerSession();
	if (!m_pClientSession)
	{
		return false;
	}

	// 初始化客户端
	ClientMsgHandler* pClientHandler = new ClientMsgHandler();
	pClientHandler->SetGroupMng(pGameGroupMng);
	m_pClientSession->SetMsgHandler(pClientHandler);
	m_pClientSession->SetMsgChainer(pMsgChainer);
	m_pClientSession->SetHeadLen(12);

	m_pClientSession->Listen(gpServerConfig->GetClientBindIp(), gpServerConfig->GetClientListenPort());

	return true;
}