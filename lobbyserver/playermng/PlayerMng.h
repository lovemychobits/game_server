#ifndef LOBBYSERVER_PLAYERMNG_H
#define LOBBYSERVER_PLAYERMNG_H

#include "../header.h"
#include "../../network/IConnection.h"
#include "../mysql/DataMng.h"

class PlayerMng
{
public:
	~PlayerMng();
	static PlayerMng* GetInstance() {
		static PlayerMng instance;
		return &instance;
	}

	bool Init();

	void PlayerConnect(IConnection* pConn);												// ���������
	void VerifyToken(const string& strAccountId, const string& strToken);				// ��֤token
	void GetPlayerDataByAccountId(IConnection* pConn, const string& strAccountId);					// ��ȡ�������

private:
	PlayerMng();

private:
	cputil::CMysqlConnection* m_pConn;

private:
	DataMng m_dataMng;
};

#define  gpPlayerMng PlayerMng::GetInstance()

#endif