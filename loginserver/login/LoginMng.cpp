#include "LoginMng.h"
#include "../mysql/DBMng.h"

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

	// ��ѯ��ҵ�½��Ϣ�Ƿ���ȷ
	bool bRet = DBMng::getInstance()->CheckLogin(loginReq.username().c_str(), loginReq.passwd().c_str());
	if (!bRet) {
		loginAck.set_errorcode(login::LoginError::PASSWD_ERROR);
	}

	return;
}