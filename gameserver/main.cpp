#include <iostream>
#include "header.h"
#include "GameServer.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include "../utils/Utility.h"
#include <boost/lexical_cast.hpp>  

#define BOOST_DATE_TIME_SOURCE
using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace cpnet;

Logger g_gsLog = Logger::getInstance(LOG4CPLUS_TEXT("ALL_MSGS"));
Logger g_gateLog = Logger::getInstance(LOG4CPLUS_TEXT("GATELOG"));

IClientSession* g_pDsSession = NULL;
IClientSession* g_pLobbySession = NULL;
IClientSession* g_pLoginAgentSession = NULL;
IClientSession* g_pPayAgentSession = NULL;

bool g_bStopServer = false;

int main(int argc, char* argv[])
{
	GameServer gameServer;
	const char* pInitConfPath = "./gs.xml";
	if (argc >= 2)
	{
		pInitConfPath = argv[1];		// 读取配置
	}

	if (!gameServer.Init(pInitConfPath))
	{
		ERRORLOG("gameserver init failed.");
		return 1;
	}

	TRACELOG("SERVER Start");
	gameServer.Start();
	gameServer.Exit();
	TRACELOG("SERVER Stop");

	return 0;
}