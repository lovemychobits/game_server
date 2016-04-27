#include "header.h"
#include "GameServer.h"
#include "handlers/ClientHandler.h"
#include "handlers/DsHandler.h"
#include "handlers/GsMsgChainer.h"
#include "scenemng-alpha/SceneMng.h"
#include "plane_shooting/SceneMng2.h"

GameServer::~GameServer()
{

	// 清理内存
	if (m_pClientHandler)
	{
		delete m_pClientHandler;
	}	
	if (m_pDsHandler)
	{
		delete m_pDsHandler;
	}
	if (m_pServerSession)
	{
		m_pServerSession->Stop();
		delete m_pServerSession;
	}
	if (m_pDsSession)
	{
		m_pDsSession->Stop();
		delete m_pDsSession;
	}
	if (m_pLobbySession)
	{
		m_pLobbySession->Stop();
		delete m_pLobbySession;
	}

	// 先清理NetCluster
	cpnet::ReleaseCluster(m_pNetCluster);

	ERRORLOG("~GameServer");
}


bool GameServer::Init(const char* pConfPath)
{
	if (!_InitLog4cpp())
	{
		return false;
	}
	if (!_InitServerConf(pConfPath))
	{
		ERRORLOG("init load " << pConfPath <<" failed.");
		return false;
	}
	if (!_InitObjects())
	{
		ERRORLOG("init objects conf failed.");
		return false;
	}
	if (!_InitServerApp())
	{
		ERRORLOG("init server app failed.");
		return false;
	}
	//if (!_InitDataServer())
	//{
	//	ERRORLOG("init data server failed.");
	//	return false;
	//}
	if (!_InitTimerTrigger())
	{
		ERRORLOG("init timer trigger failed.");
		return false;
	}
	//if (!_InitLoadConf())
	//{
	//	ERRORLOG("init configure failed.");
	//	return false;
	//}
	if (!_InitModules())
	{
		ERRORLOG("init modules failed.");
		return false;
	}
	return true;
}

void GameServer::Stop()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	// 停止gameserver新的连接建立
	m_pServerSession->Stop();

	// 停止网络模块
	m_pNetCluster->Stop();
}

void GameServer::Exit()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
}

// 将所有玩家踢下线
void GameServer::KickAllOff()
{
	g_bStopServer = true;
}

bool GameServer::_InitObjects()
{
	m_pClientHandler = new ClientHandler();
	if (!m_pClientHandler)
	{
		return false;
	}

	m_pDsHandler = new DsHandler();
	if (!m_pDsHandler)
	{
		return false;
	}

	m_pMsgDecoder = new ClientMsgDecoder();
	m_pMsgEncoder = new ClientMsgEncoder();

	return true;
}

bool GameServer::_InitLog4cpp()
{
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("./gs.properties"));
	Logger root = Logger::getRoot();

	return true;
}

bool GameServer::_InitServerConf(const char* pConfPath)
{
	return gpServerConfig->LoadServerConf(pConfPath);
}

bool GameServer::_InitServerApp()
{
	m_pNetCluster = CreateNetCluster();
	if (!m_pNetCluster)
	{
		return false;
	}
	
	m_pNetCluster->SetNetThreadNum(4);
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
	pMsgChainer->AddDecodeLast(m_pMsgDecoder);
	pMsgChainer->AddEncodeLast(m_pMsgEncoder);

	m_pServerSession->SetMsgHandler(m_pClientHandler);
	m_pServerSession->SetMsgChainer(pMsgChainer);
	m_pServerSession->SetHeadLen(gpServerConfig->GetMsgHeadLen());
	m_pServerSession->Listen(gpServerConfig->GetGsBindIp(), gpServerConfig->GetGsBindPort());
	return true;
}


bool GameServer::_InitDataServer()
{
	m_pDsSession = m_pNetCluster->CreateClientSession();
	if (!m_pDsSession)
	{
		return false;
	}

	IMsgChainer* pMsgChainer = m_pNetCluster->CreateMsgChainer();
	pMsgChainer->AddDecodeLast(m_pMsgDecoder);
	pMsgChainer->AddEncodeLast(m_pMsgEncoder);

	m_pDsSession->SetMsgChainer(pMsgChainer);
	m_pDsSession->SetMsgHandler(m_pDsHandler);
	m_pDsSession->SetHeadLen(gpServerConfig->GetMsgHeadLen());

	m_pDsSession->Connect(gpServerConfig->GetDsIp(), gpServerConfig->GetDsPort());
	g_pDsSession = m_pDsSession;
	return true;
}

bool GameServer::_InitTimerTrigger()
{
	m_pTimerTrigger = m_pNetCluster->CreateTimerTrigger();
	if (!m_pTimerTrigger)
	{
		return false;
	}
	//gpTimerMng->SetTimerTigger(m_pTimerTrigger);
	m_pTimerTrigger->AddCircleTimer(boost::bind(&plane_shooting::SceneMng::OnTimer, plane_shooting::SceneMng::GetInstance(), _1), 50);
	return true;
}

bool GameServer::_InitLoadConf()
{
	//ConfigureReloador::GetInstance()->Init();
	//if (!ConfigureReloador::GetInstance()->ReloadConf())
	//{
	//	return false;
	//}
	return true;
}

bool GameServer::_InitModules()
{
	scene_alpha::SceneMng::getInstance()->Init();
	//float startpos[] = { -16.8693466, -2.36812592, 24.6918983 }; // m_spos = 0x08618340 {-16.8693466, -2.36812592, 24.6918983}
	//float endpos[] = { -16.8693466, -2.36812592, 17.6918983 };					 // m_epos = 0x0861834c {17.6115112, -2.37033081, -22.8771439}

	//float startpos[] = { -9.2f, -2.5f, 21.3f }; 
	//float endpos[] = { 10.5f, -1.4f, 28.0f };


	//DWORD dwStart = ::GetTickCount();
	//for (int i = 0; i < 1; ++i)
	//{
	//	scene_alpha::SceneMng::getInstance()->findPath(startpos, endpos);
	//}
	//DWORD dwEnd = ::GetTickCount();
	//cout << "Time cost, 10000times, time=[" << (dwEnd - dwStart) / 1000.0 << "]" << endl;
	
	return true;
}