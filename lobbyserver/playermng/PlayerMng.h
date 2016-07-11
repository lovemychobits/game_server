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

	void PlayerConnect(IConnection* pConn);												// 有玩家连接
	void VerifyToken(const string& strAccountId, const string& strToken);				// 验证token
	void GetPlayerDataByAccountId(IConnection* pConn, const string& strAccountId);					// 获取玩家数据

private:
	PlayerMng();

private:
	cputil::CMysqlConnection* m_pConn;

private:
	DataMng m_dataMng;
};

#define  gpPlayerMng PlayerMng::GetInstance()

#endif