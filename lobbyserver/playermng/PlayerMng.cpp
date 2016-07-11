#include "PlayerMng.h"
#include "../mysql/DataMng.h"
#include "../mysql/SqlTables.h"
#include "../../protocol/slither_lobby.pb.h"
#include "../../utils/ProtoTemplate.hpp"
#include "../config/ServerConfig.h"


PlayerMng::PlayerMng() {

}

PlayerMng::~PlayerMng() {

}

bool PlayerMng::Init() {
	cputil::CMysqlConnection sqlConn;
	if (!sqlConn.Init(gpServerConfig->GetMysqlIp().c_str(), gpServerConfig->GetMysqlPort(), gpServerConfig->GetMysqlUser().c_str(), gpServerConfig->GetMysqlPwd().c_str(), gpServerConfig->GetMysqlDB().c_str())) {
		ERRORLOG("init mysql connection failed, mysql ip=[" << gpServerConfig->GetMysqlIp().c_str() << "], port=[" << gpServerConfig->GetMysqlPort() << "], user=[" << gpServerConfig->GetMysqlUser().c_str() << "], pwd=[" << gpServerConfig->GetMysqlPwd().c_str() << "], db=[" << gpServerConfig->GetMysqlDB().c_str() << "]");
		return false;
	}

	SqlTables sqlTables(sqlConn);
	bool bRet = sqlTables.CreateTables("./sqls/create_tables.sql");
	if (!bRet) {
		return false;
	}

	bRet = m_dataMng.InitMysql(gpServerConfig->GetMysqlIp().c_str(), gpServerConfig->GetMysqlPort(), gpServerConfig->GetMysqlUser().c_str(), gpServerConfig->GetMysqlPwd().c_str(), gpServerConfig->GetMysqlDB().c_str());
	if (!bRet) {
		return false;
	}
	return bRet;
}

// 有玩家连接
void PlayerMng::PlayerConnect(IConnection* pConn) {

}

// 验证token
void PlayerMng::VerifyToken(const string& strAccountId, const string& strToken) {

}

// 获取玩家数据
//void PlayerMng::GetPlayerDataByAccountId(IConnection* pConn, const string& strAccountId) {
//	if (!pConn) {
//		return;
//	}
//	slither::GetPlayerInfoAck getPlayerInfoAck;
//	slither::PBPlayerInfo* pPBPlayerInfo = getPlayerInfoAck.mutable_playerinfo();
//
//	const char* ip = pConn->GetRemoteIp();
//	string ipStr(ip);
//
//	bool bRet = m_dataMng.GetPlayerDataByAccount(strAccountId, ipStr, *pPBPlayerInfo,NULL);
//
//	if (!bRet) {
//		getPlayerInfoAck.set_errorcode(slither::PLAYER_NOT_FOUND);
//	}
//
//	string strResponse;
//	cputil::BuildResponseProto(getPlayerInfoAck, strResponse, slither::REQ_GET_PLAYER_INFO);
//	pConn->SendMsg(strResponse.c_str(), strResponse.size());
//	return;
//}




