#ifndef GAMESERVER_CLIENTHANDLER_H
#define GAMESERVER_CLIENTHANDLER_H

#include "../../network/header.h"
#include "../../network/IMsgParser.h"
#include "../../network/IConnection.h"
#include "../../network/IMsgHandler.h"
using namespace cpnet;
/*
	处理从客户端(玩家)发送过来的请求
*/
class ClientHandler : public IMsgHandler
{
public:
	ClientHandler()
	{
		_RegisterAllCmd();								// 注册所有命令
	}
	virtual ~ClientHandler()
	{
		m_cmdFuncMap.clear();
	}

	void HandleConnect(IConnection* pConn);

	void HandleDisconnect(IConnection* pConn, const BoostErrCode& error);

	void HandleWrite(const boost::system::error_code& error, size_t bytes_transferred);

	void HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen);

private:
	typedef void (ClientHandler::*pCmdFunc)(IConnection*, MessageHeader*);
	void _RegisterCmd(uint32_t uCmdId, pCmdFunc cmdFunc);
	void _RegisterAllCmd();

private:
	// 网络基本功能测试	
	void _RequestTestPingPong(IConnection* pConn, MessageHeader* pMsgHeader);

	void _RequestEnterGame(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestQueryPath(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestPlaneMove(IConnection* pConn, MessageHeader* pMsgHeader);
	void _RequestPlaneShoot(IConnection* pConn, MessageHeader* pMsgHeader);

private:
	template<typename T>
	void _SendPlayerNotFound(IConnection* pConn, T& response, uint32_t uMsgCmd)
	{
		response.set_errcode(ERROR_PLAYER_NOT_FOUND);
		string strMessage;
		BuildResponseProto(response, strMessage, uMsgCmd);

		pConn->SendMsg(strMessage.c_str(), strMessage.size());
		return;
	}

	template<typename T>
	void _SendPlayerNoAccess(IConnection* pConn, T& response, uint32_t uErrcode, uint32_t uMsgCmd)
	{
		response.set_errcode(uErrcode);
		string strMessage;
		BuildResponseProto(response, strMessage, uMsgCmd);

		pConn->SendMsg(strMessage.c_str(), strMessage.size());
		return;
	}

private:
	map<uint32_t, pCmdFunc> m_cmdFuncMap;
};

#endif