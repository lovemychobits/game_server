#ifndef LOGINSERVER_LOGINMNG_H
#define LOGINSERVER_LOGINMNG_H

#include "../../network/MessageHeader.h"
#include "../../network/IConnection.h"
#include "../../protocol/login.pb.h"
using namespace cpnet;

class LoginMng
{
public:
	~LoginMng();
	static LoginMng* getInstance() {
		static LoginMng instance;
		return &instance;
	}

public:
	void ReqLogin(IConnection* pConn, const login::LoginReq& loginReq);							// ��������½
	void ReqRegister(IConnection* pConn, const login::RegisterReq& registerReq);				// �������ע��

private:
	uint32_t GenLoginToken();

private:
	LoginMng();
};

#endif