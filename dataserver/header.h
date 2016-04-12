#ifndef DATASEVER_HEADER_H
#define DATASEVER_HEADER_H

#ifdef _MSC_VER
	#define _WIN32_WINNT 0x0501
#endif

#if defined(_MSC_VER)
	#pragma warning (disable:4996) 
#endif 

#include <string>
#include <map>

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
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>

#include "../network/INetCluster.h"
#include "../network/IConnection.h"
using namespace cpnet;

using namespace std;

extern Logger g_dsLog;

#define TRACELOG(logEvent)	LOG4CPLUS_TRACE(g_dsLog, logEvent)
#define DEBUGLOG(logEvent)	LOG4CPLUS_DEBUG(g_dsLog, logEvent)
#define INFOLOG(logEvent)	LOG4CPLUS_INFO(g_dsLog, logEvent)
#define WARNLOG(logEvent)	LOG4CPLUS_WARN(g_dsLog, logEvent)
#define ERRORLOG(logEvent)	LOG4CPLUS_ERROR(g_dsLog, logEvent)
#define FATALLOG(logEvent)	LOG4CPLUS_FATAL(g_dsLog, logEvent)

#endif