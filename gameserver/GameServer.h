#ifndef GAMESERVER_GAMESERVER_H
#define GAMESERVER_GAMESERVER_H

#include <boost/thread.hpp> 
#include "config/ServerConfig.h"
#include "../network/INetCluster.h"
#include "../network/ITimerTrigger.h"

class ClientHandler;
class ClientMsgParser;
class CmdHandler;
class DsHandler;
class LobbyHandler;
class LoginAgentHandler;
class PayAgentHandler;
class WifiPvpHandler;

class GameServer
{
public:
	GameServer() : m_pServerSession(NULL), m_pDsSession(NULL), m_pLobbySession(NULL), m_pNetCluster(NULL), 
		m_pTimerTrigger(NULL), m_pClientHandler(NULL), m_pDsHandler(NULL)
	{
		
	}

	~GameServer();

	bool Init(const char* pConfPath);

	void Start()
	{
		m_pNetCluster->Run();
	}
	void Stop();
	void Exit();
	void KickAllOff();

private:
	bool _InitObjects();
	bool _InitLog4cpp();								// 初始化log4cplus
	bool _InitServerConf(const char* pConfPath);		// 初始化gameserver系统配置

	bool _InitServerApp();								// 启动GameServer监听
	bool _InitDataServer();								// 连接DBServer
	bool _InitLobbyServer();							// 连接LobbyServer
	bool _InitLoginServerAgent();						// 连接LoginServerAgent 
	bool _InitPayServerAgent();							// 连接PayServerAgent

	bool _InitTimerTrigger();							// 启动定时器模块
	bool _InitLoadConf();								// 初始化配置
	bool _InitModules();								// 初始化游戏内模块
	bool _InitEventRecord();							// 初始化事件记录

private:
	IServerSession* m_pServerSession;					// GameServer和客户端直接交互的session
	IClientSession* m_pDsSession;						// 和DataServer之间交互
	IClientSession* m_pLobbySession;					// 和LobbyServer之间交互

	INetCluster*	m_pNetCluster;
	ITimerTrigger*	m_pTimerTrigger;


private:
	// handler 对象
	ClientHandler* m_pClientHandler;					// 客户端处理类
	DsHandler* m_pDsHandler;							// DataServer处理类

private:
	boost::mutex m_mutex;
};

#endif