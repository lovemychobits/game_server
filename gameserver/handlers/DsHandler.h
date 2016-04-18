#ifndef GAMESERVER_DSHANDLER_H
#define GAMESERVER_DSHANDLER_H

#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
using namespace cpnet;
//#include "PlayerDataGetter.h"

/*
	处理从DataServer返回的消息
*/
class DsHandler : public IMsgHandler
{
public:
	virtual ~DsHandler()
	{
	}
	void HandleConnect(IConnection* pConnection);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen);

private:
	//PlayerDataGetter m_dataGetter;
};


#endif