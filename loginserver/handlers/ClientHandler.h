#ifndef LOGINSERVER_GSHANDLER_H
#define LOGINSERVER_GSHANDLER_H

#include "../../utils/Utility.h"
#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
#include "../../utils/ProtoTemplate.hpp"
using namespace cpnet;

/*
处理从Client发送过来的请求
*/
class LoginServer;
class ClientHandler : public IMsgHandler
{
public:
	ClientHandler(LoginServer* pLoginServer);
	void HandleConnect(IConnection* pConn);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen);

private:
	LoginServer* m_pLoginServer;
};

#endif