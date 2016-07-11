#include "LobbyServer.h"
#include "handlers/GameServerHandler.h"
#include "gsgroup/GameGroupMng.h"
#include "handlers/ClientMsgHandler.h"
#include "handlers/LobbyMsgChainer.h"
#include "playermng/playerMsg.h"

#include "config/ServerConfig.h"
#include "config/GradeConfig.h"
#include "config/SkinConfig.h"
#include "config/LvExpConfig.h"
#include "config/LvExpSettlement.h"
#include "config/LvExpLenConfig.h"
#include "config/LvExpRankConfig.h"


#include "ConfigPath.h"

bool LobbyServer::Init(const char* serverConfPath)
{
	if (!InitLog4cpp())
	{
		return false;
	}
	if (!InitServerConf(serverConfPath))
	{
		return false;
	}
	if (!InitServerApp())
	{
		return false;
	}
	if (!InitPlayerDataMng()) 
	{
		return false;
	}

	if (!InitConfig())
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
	pGsHandler->SetGroupMng(gpGameGroupMng);
	m_pServerSession->SetMsgHandler(pGsHandler);
	m_pServerSession->SetMsgChainer(pMsgChainer);
	m_pServerSession->SetHeadLen(4);

	m_pServerSession->Listen(gpServerConfig->GetBindIp(), gpServerConfig->GetListenPort());

	m_pClientSession = m_pNetCluster->CreateServerSession();
	if (!m_pClientSession)
	{
		return false;
	}

	// 初始化客户端
	ClientMsgHandler* pClientHandler = new ClientMsgHandler();
	pClientHandler->SetGroupMng(gpGameGroupMng);
	m_pClientSession->SetMsgHandler(pClientHandler);
	m_pClientSession->SetMsgChainer(pMsgChainer);
	m_pClientSession->SetHeadLen(4);

	m_pClientSession->Listen(gpServerConfig->GetClientBindIp(), gpServerConfig->GetClientListenPort());

	return true;
}



bool LobbyServer::InitPlayerDataMng() 
{
	return playerMsgDeal->Init();
}

bool LobbyServer::InitConfig()
{
	if (!gradeConfig->LoadGradeConf(configPath->gradeConfigPath))
	{
		cout << "!!! gradeConfig->LoadGradeConf(configPath->gradeConfigPath) error" << endl;
		return false;
	}

	if (!skinConfig->LoadSkinConf(configPath->skinConfigPath))
	{
		cout << "!!! skinConfig->LoadSkinConf(configPath->skinConfigPath) error" << endl;
		return false;
	}

	if (!lvExpConfig->LoadLvExpConf(configPath->lvExpConfigPath))
	{
		cout << "!!! lvExpConfig->LoadLvExpConf(configPath->lvExpConfigPath) error" << endl;
		return false;
	}

	if (!lvExpLenConfig->LoadLvExpLenConf(configPath->lvExpLenConfigPath))
	{
		cout << "!!! lvExpConfig->LoadLvExpConf(configPath->lvExpConfigPath) error" << endl;
		return false;
	}

	if (!lvExpRankConfig->LoadLvExpRankConf(configPath->lvExpRankConfigPath))
	{
		cout << "!!! lvExpRankConfig->LoadLvExpRankConf(configPath->lvExpRankConfigPath) error" << endl;
		return false;
	}

	if (!lvExpSettConfig->LoadLvExpSettConf(configPath->lvExpSettConfigPath))
	{
		cout << "!!! lvExpRankConfig->LoadLvExpRankConf(configPath->lvExpRankConfigPath) error" << endl;
		return false;
	}


	return true;
}