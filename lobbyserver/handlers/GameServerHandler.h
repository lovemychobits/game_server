#ifndef LOBBYSERVER_GAMESERVERHANDLER_H
#define LOBBYSERVER_GAMESERVERHANDLER_H

#include "../../utils/Utility.h"
#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
using namespace cpnet;

/*
	处理从GameServer发送过来的请求
*/
class LobbyServer;
class GameGroupMng;
class GameServerHandler : public IMsgHandler
{
public:
	GameServerHandler(LobbyServer* pDataServer);
	virtual ~GameServerHandler();

	void SetGroupMng(GameGroupMng* pGroupMng);

	void HandleConnect(IConnection* pConn);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t nLen);

private:
	void savePlayerStatisticsData(const char* pBuf, uint32_t nLen);
	void receiveHttpBulletin(IConnection* pGsConn, const char* pBuf, uint32_t uLen);
	void registerGameServer(IConnection* pGsConn, const char* pBuf, uint32_t uLen);
	void delHttpBulletin(IConnection* pGsConn, const char* pBuf, uint32_t uLen);
	void registerGameRoom(IConnection* pGsConn, const char* pBuf, uint32_t uLen);
	void notifyPlayerEnterRoom(const char* pBuf, uint32_t uLen);

private:
	LobbyServer* m_pLobbyServer;
	GameGroupMng* m_pGameGroupMng;
};

#endif