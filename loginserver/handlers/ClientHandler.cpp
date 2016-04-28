#include "ClientHandler.h"
#include "../header.h"
#include "../LoginServer.h"
#include "../../protocol/login.pb.h"
#include "../login/LoginMng.h"

ClientHandler::ClientHandler(LoginServer* pLoginServer) : m_pLoginServer(pLoginServer)
{

}

void ClientHandler::HandleConnect(IConnection* pConn)
{
	TRACELOG("client connect:" << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort());
}

void ClientHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{
	TRACELOG("client " << pConn->GetRemoteIp() << ":" << pConn->GetRemotePort() << " disconnect");
}

void ClientHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		ERRORLOG("error:" << boost::system::system_error(error).what());
	}
}

void ClientHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen)
{
	if (!pBuf || !uLen)
	{
		return;
	}

	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	// TODO, using handler array like
	/*
		const Handlers table[] =
		{
		{ REQ_LOGIN,     &ClientHandler::_HandleLogin    },
		{ REQ_REGISTER,  &ClientHandler::_HandleRegister },
		};
	*/

	switch (pMsgHeader->uMsgCmd)
	{
	case login::LoginProtocol::REQ_LOGIN:				// 处理登陆请求
	{
		login::LoginReq loginReq;
		loginReq.ParseFromArray(pBuf + sizeof(MessageHeader), uLen - sizeof(MessageHeader));
		LoginMng::getInstance()->ReqLogin(pConn, loginReq);
	}
	break;
	case login::LoginProtocol::REQ_REGISTER:			// 处理注册请求
	{
		login::RegisterReq registerReq;
		registerReq.ParseFromArray(pBuf + sizeof(MessageHeader), uLen - sizeof(MessageHeader));
		LoginMng::getInstance()->ReqRegister(pConn, registerReq);
	}
	break;
	default:
		break;
	}
}
