#ifndef LOGINSERVER_HEADER_H
#define LOGINSERVER_HEADER_H

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

extern Logger g_lsLog;

#define TRACELOG(logEvent)	LOG4CPLUS_TRACE(g_lsLog, logEvent)
#define DEBUGLOG(logEvent)	LOG4CPLUS_DEBUG(g_lsLog, logEvent)
#define INFOLOG(logEvent)	LOG4CPLUS_INFO(g_lsLog, logEvent)
#define WARNLOG(logEvent)	LOG4CPLUS_WARN(g_lsLog, logEvent)
#define ERRORLOG(logEvent)	LOG4CPLUS_ERROR(g_lsLog, logEvent)
#define FATALLOG(logEvent)	LOG4CPLUS_FATAL(g_lsLog, logEvent)

#endif