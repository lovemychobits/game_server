#include "../header.h"
#include "ClientHandler.h"
#include "../../utils/Utility.h"
#include "../../protocol/slither_battle.pb.h"
#include "../slither/Scene.h"
#include "../slither/PlayerMng.h"
#include "../slither/Snake.h"
#include "../slither/GameRoom.h"

void ClientHandler::HandleConnect(IConnection* pConn)
{
	TRACELOG("client connect=[" << pConn << "] ip=[" << pConn->GetRemoteIp() << "], port=[" << pConn->GetRemotePort() << "]");
}

void ClientHandler::HandleDisconnect(IConnection* pConn, const BoostErrCode& error)
{   
	TRACELOG("client " << pConn->GetRemoteIp() << " disconnect, conntion=[" << pConn << "]");
	if (error)
	{
		ERRORLOG("handle disconnect error, error=[" << boost::system::system_error(error).what() << "]");
	}
	slither::PlayerMng::GetInstance()->PlayerDisconnect(pConn);
}

void ClientHandler::HandleWrite(const boost::system::error_code& error, size_t bytes_transferred) 
{
	if (error)
	{
		ERRORLOG("handle write error, error=[" << boost::system::system_error(error).what() << "], bytes_transferred=[" << bytes_transferred << "]");
	}
}

void ClientHandler::HandleRecv(IConnection* pConn, const char* pBuf, uint32_t uLen)
{
	if (!pBuf || uLen == 0)
	{
		ERRORLOG("message error");
		return;
	}
	
	MessageHeader* pMsgHeader = (MessageHeader*)pBuf;

	map<uint32_t, pCmdFunc>::iterator funcIt = m_cmdFuncMap.find(ntohs(pMsgHeader->uMsgCmd));
	if (funcIt == m_cmdFuncMap.end())
	{
		ERRORLOG("cannot find cmd=[0x" << hex << ntohs(pMsgHeader->uMsgCmd) << "] function");
		return;
	}
	static uint32_t uMsgProc = 0;
	static uint64_t uStartProcTime = cputil::GetSysTime();
	++uMsgProc;
	if (uMsgProc % 100000 == 0)
	{
		uint64_t uNow = cputil::GetSysTime();
		cout << "proc msg times=[" << uMsgProc << "], time cost=[" << uNow - uStartProcTime << "]" << endl;
		uMsgProc = 0;
		uStartProcTime = cputil::GetSysTime();
	}

	pCmdFunc& cmdFunc = funcIt->second;
	(this->*cmdFunc)(pConn, pMsgHeader);
	return;
}

void ClientHandler::_RegisterCmd(uint32_t uCmdId, pCmdFunc cmdFunc)
{
	m_cmdFuncMap.insert(make_pair(uCmdId, cmdFunc));
}

void ClientHandler::_RegisterAllCmd()
{
	// 网络基本功能测试
	//_RegisterCmd(ID_REQ_Test_PingPong,					&ClientHandler::_RequestTestPingPong);			// 测试使用的ping-pong协议, 简单的将数据包返回
	_RegisterCmd(slither::REQ_ENTER_GAME,	&ClientHandler::_RequestEnterGame);
	_RegisterCmd(slither::REQ_MOVE,			&ClientHandler::_RequestNewDirect);
	
	return;
}

void ClientHandler::_RequestTestPingPong(IConnection* pConn, MessageHeader* pMsgHeader)
{
	pConn->SendMsg((char*)pMsgHeader, pMsgHeader->uMsgSize);
}

void ClientHandler::_RequestEnterGame(IConnection* pConn, MessageHeader* pMsgHeader)
{
	if (!pConn || !pMsgHeader)
	{
		return;
	}

	slither::EnterGameReq enterGameReq;
	const char* pBuf = (const char*)pMsgHeader;
	enterGameReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	slither::gpGameRoomMng->EnterGame(pConn, enterGameReq.userid(), enterGameReq.nickname(), enterGameReq.skinid());
	return;
}

void ClientHandler::_RequestNewDirect(IConnection* pConn, MessageHeader* pMsgHeader) 
{
	if (!pConn || !pMsgHeader) {
		return;
	}

	slither::SnakeMoveReq moveReq;
	const char* pBuf = (const char*)pMsgHeader;
	moveReq.ParseFromArray(pBuf + sizeof(MessageHeader), ntohs(pMsgHeader->uMsgSize) - sizeof(MessageHeader));

	slither::Snake* pSnake = slither::PlayerMng::GetInstance()->GetPlayerSnake(pConn);
	if (!pSnake) {
		return;
	}
	pSnake->GetScene()->SnakeMove(pSnake, moveReq.newangle(), moveReq.speedup(), moveReq.stopmove());
	return;
}

