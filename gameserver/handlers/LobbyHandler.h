#ifndef GAMESERVER_LOBBYHANDLER_H
#define GAMESERVER_LOBBYHANDLER_H

#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
using namespace cpnet;
/*
	处理从DataServer返回的消息
*/
class LobbyHandler : public IMsgHandler
{
public:
	virtual ~LobbyHandler()
	{
	}
	void HandleConnect(IConnection* pConnections);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t uBytesTransferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen);

	void RegGameServer(IConnection* pConnections);					// 向lobbyserver注册gs
	void RegGameRooms();											// 向lobbyserver注册room
};


#endif