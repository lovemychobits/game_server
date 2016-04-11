#ifndef NETWORK_HEADER_H
#define NETWORK_HEADER_H

#ifdef _MSC_VER
	#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0501
	#endif
#endif

#include "MessageHeader.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <deque>
using namespace std;

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp> 
#include <boost/thread/locks.hpp> 
#include <boost/thread/condition.hpp>
#include <boost/functional/hash.hpp>
#include <boost/random.hpp>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
using namespace log4cplus;
using namespace log4cplus::helpers;

#define TRACE_NET(logEvent)	LOG4CPLUS_TRACE	(g_netLog, logEvent)
#define DEBUG_NET(logEvent)	LOG4CPLUS_DEBUG	(g_netLog, logEvent)
#define INFO_NET (logEvent)	LOG4CPLUS_INFO	(g_netLog, logEvent)
#define WARN_NET (logEvent)	LOG4CPLUS_WARN	(g_netLog, logEvent)
#define ERROR_NET(logEvent)	LOG4CPLUS_ERROR	(g_netLog, logEvent)
#define FATAL_NET(logEvent)	LOG4CPLUS_FATAL	(g_netLog, logEvent)

namespace cpnet
{
	extern Logger g_netLog;
	extern bool g_bStopped;

	typedef boost::asio::io_service BoostIoService;
	typedef boost::asio::strand BoostStrand;
	typedef boost::asio::ip::tcp BoostTcp;
	typedef boost::asio::ip::tcp::acceptor BoostAcceptor;
	typedef boost::asio::ip::address BoostAddress;
	typedef boost::asio::ip::tcp::endpoint BoostEndPoint;
	typedef boost::asio::ip::tcp::socket BoostSocket;
	typedef boost::system::error_code BoostErrCode;
	typedef boost::asio::deadline_timer BoostTimer;

	class IConnection;

	typedef boost::function<void(cpnet::IConnection*, const char*, int)> recvFuncCallBack;			// the recv call back 
	typedef boost::function<void(const BoostErrCode&, std::size_t)> sendFuncCallBack;				// the send call back 
	typedef boost::function<void(cpnet::IConnection*)> connFuncCallBack;							// connection call back
	typedef boost::function<void(cpnet::IConnection*, const BoostErrCode&)> disConnFuncCallBack;	// disconnection call back
	typedef boost::function<void(cpnet::IConnection*, const BoostErrCode&)> acceptFuncCallBack;		// accept call back
	typedef boost::function<void(const boost::system::error_code&)> TriggerCallback;				// trigger call back
}

#endif