#ifndef GAMESERVER_HEADER_H
#define GAMESERVER_HEADER_H

#ifdef _MSC_VER
    #define _WIN32_WINNT 0x0501
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400 
	#pragma warning(disable:4996) 
#endif 

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/ndc.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/loggingmacros.h>

#include <google/protobuf/text_format.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/random.hpp>
#include <boost/timer/timer.hpp>

#include <string>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>

#include "../network/INetCluster.h"
#include "../network/header.h"
#include "../network/TimerTrigger.h"
#include "../utils/ProtoTemplate.hpp"

using namespace cpnet;

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;


extern Logger g_gsLog;
extern Logger g_gateLog;

extern bool g_bStopServer;

extern IClientSession* g_pDsSession;					// 和DataServer连接的对象
extern IClientSession* g_pLobbySession;					// 和LobbyServer连接的对象
extern IClientSession* g_pLoginAgentSession;			// 和LoginAgent连接的对象
extern IClientSession* g_pPayAgentSession;				// 和PayAgent连接的对象

#define TRACELOG(logEvent)	LOG4CPLUS_TRACE(g_gsLog, logEvent)
#define DEBUGLOG(logEvent)	LOG4CPLUS_DEBUG(g_gsLog, logEvent)
#define INFOLOG(logEvent)	LOG4CPLUS_INFO(g_gsLog, logEvent)
#define WARNLOG(logEvent)	LOG4CPLUS_WARN(g_gsLog, logEvent)
#define ERRORLOG(logEvent)	LOG4CPLUS_ERROR(g_gsLog, logEvent)
#define FATALLOG(logEvent)	LOG4CPLUS_FATAL(g_gsLog, logEvent)

#endif