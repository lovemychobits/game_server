#ifndef DATASERVER_GSHANDLER_H
#define DATASERVER_GSHANDLER_H

#include "../../utils/Utility.h"
#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
#include "../../utils/ProtoTemplate.hpp"
using namespace cpnet;

/*
	处理从GameServer发送过来的请求
*/
class DataServer;
class GsHandler : public IMsgHandler
{
public:
	GsHandler(DataServer* pDataServer);
	void HandleConnect(IConnection* pConn);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen);

private:
	//boost::hash<string> m_stringHasher;
	DataServer* m_pDataServer;
};

#endif