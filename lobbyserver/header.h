#ifndef LOBBYSERVER_HEADER_H
#define LOBBYSERVER_HEADER_H

#include <string>
#include <map>
#include <iostream>
#include <boost/lexical_cast.hpp>

#ifdef _MSC_VER
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT 0x0501
	#endif
#endif

#include "../network/INetCluster.h"
#include "../network/IConnection.h"
#include "../utils/ProtoTemplate.hpp"

using namespace std;

extern Logger g_lobbyLog;
extern Logger g_clientLog;

#define TRACELOG(logEvent)	LOG4CPLUS_TRACE(g_lobbyLog, logEvent)
#define DEBUGLOG(logEvent)	LOG4CPLUS_DEBUG(g_lobbyLog, logEvent)
#define INFOLOG(logEvent)	LOG4CPLUS_INFO(g_lobbyLog, logEvent)
#define WARNLOG(logEvent)	LOG4CPLUS_WARN(g_lobbyLog, logEvent)
#define ERRORLOG(logEvent)	LOG4CPLUS_ERROR(g_lobbyLog, logEvent)
#define FATALLOG(logEvent)	LOG4CPLUS_FATAL(g_lobbyLog, logEvent)

#define TRACE_CLIENT(logEvent) LOG4CPLUS_TRACE(g_clientLog, logEvent)

#endif