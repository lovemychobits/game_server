#include "LoginMng.h"
#include "../mysql/DBMng.h"
#include "../../utils/Utility.h"
#include "../../utils/ProtoTemplate.hpp"

LoginMng::LoginMng()
{
}

LoginMng::~LoginMng()
{

}

void LoginMng::ReqLogin(IConnection* pConn, const login::LoginReq& loginReq)
{
	if (!pConn) {
		return;
	}

	login::LoginAck loginAck;

	// 查询玩家登陆信息是否正确
	bool bRet = DBMng::getInstance()->CheckLogin(loginReq.username().c_str(), loginReq.passwd().c_str());
	if (!bRet) {
		loginAck.set_errorcode(login::LoginError::PASSWD_ERROR);
	}
	else {
		loginAck.set_logintoken(GenLoginToken());
	}

	string strResponse;
	cputil::BuildResponseProto(loginAck, strResponse, login::LoginProtocol::REQ_LOGIN);
	pConn->SendMsg(strResponse.c_str(), strResponse.size());
	return;
}

void LoginMng::ReqRegister(IConnection* pConn, const login::RegisterReq& registerReq)
{
	if (!pConn) {
		return;
	}

	login::RegisterAck registerAck;
	// 玩家进行注册
	bool bRet = DBMng::getInstance()->Register(registerReq.username().c_str(), registerReq.passwd().c_str());
	if (!bRet) {
		registerAck.set_errorcode(login::LoginError::USER_HAS_REGISTER);
	}

	string strResponse;
	cputil::BuildResponseProto(registerAck, strResponse, login::LoginProtocol::REQ_REGISTER);
	pConn->SendMsg(strResponse.c_str(), strResponse.size());

	return;
}

uint32_t LoginMng::GenLoginToken()
{
	return cputil::GenRandom(100000, 999999);
}