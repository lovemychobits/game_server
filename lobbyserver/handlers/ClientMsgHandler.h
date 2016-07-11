#ifndef LOBBYSERVER_CLIENTMSGHANDLER_H
#define LOBBYSERVER_CLIENTMSGHANDLER_H

#include "../../utils/Utility.h"
#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
using namespace cpnet;
/*
	处理从客户端发送过来的请求
*/

class GameGroupMng;
class ClientMsgHandler : public IMsgHandler
{
public:
	ClientMsgHandler();

	void SetGroupMng(GameGroupMng* pGroupMng);

	void HandleConnect(IConnection* pConn);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t nLen);

private:
	void reqGetPlayerInfo(IConnection* pConn, const char* pBuf, uint32_t nLen);
	void verifySDKTokenAndLogin(IConnection* pConn, const char* pBuf, uint32_t nLen);
	void setUseSkin(IConnection* pConn, const char* pBuf, uint32_t nLen);
	void unlockSkin(IConnection* pConn, const char* pBuf, uint32_t nLen);
	void modifyUserInfo(IConnection* pConn, const char* pBuf, uint32_t nLen);
	void netEcho(IConnection* pConn, const char* pBuf, uint32_t nLen);
	void getBulletins(IConnection* pConn);
	void getEnterRoomId(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void palyerAttention(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void getAttentivedList(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void getFansList(IConnection* pConn, const char* pBuf, uint32_t uLen);


	// 团队模式相关
	void createTeam(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void leaveTeam(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void getInviteList(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void inviteJoinTeam(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void acceptJoinTeam(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void refuseJoinTeam(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void kickTeamMember(IConnection* pConn, const char* pBuf, uint32_t uLen);
	void startTeamGame(IConnection* pConn, const char* pBuf, uint32_t uLen);

private:
	GameGroupMng* m_pGameGroupMng;
};

#endif
