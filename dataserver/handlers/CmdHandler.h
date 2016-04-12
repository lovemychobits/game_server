#ifndef DATASERVER_CMDHANDLER_H
#define DATASERVER_CMDHANDLER_H

#include "../header.h"
#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
using namespace cpnet;

/*
	处理从命令通道发送过来的请求
*/
class DataServer;
class CmdHandler : public IMsgHandler
{
public:
	CmdHandler()
	{

	}

	~CmdHandler()
	{
	}

	void SetDataServer(DataServer* pServer)
	{
		m_pDataServer = pServer;
	}

public:
	void HandleConnect(IConnection* pConn);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen);

private:
	DataServer* m_pDataServer;
};

#endif