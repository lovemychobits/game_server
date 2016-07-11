#ifndef LOBBYSERVER_PLAYERSTATISTICS_H
#define LOBBYSERVER_PLAYERSTATISTICS_H

#include "../header.h"
#include "../mysql/DataMng.h"
#include "../config/ServerConfig.h"
#include "../../protocol/slither_server.pb.h"


class PlayerStatisticsMgr
{
public:
	static DataMng m_dataMng;

	~PlayerStatisticsMgr();

	static PlayerStatisticsMgr* GetInstance() {
		m_dataMng.InitMysql(gpServerConfig->GetMysqlIp().c_str(), gpServerConfig->GetMysqlPort(), gpServerConfig->GetMysqlUser().c_str(), gpServerConfig->GetMysqlPwd().c_str(), gpServerConfig->GetMysqlDB().c_str());
		static PlayerStatisticsMgr instance;
		return &instance;
	}
	void savePlayerStatisticsData(slither::GameRoomEndNty& roomEndNotifyInfo);
	
private:
	void updatePlayerBaseInfo(const int roundRoomId,slither::PBPlayerBase playerBaseInfo, bool isNeedUpdateTotalStar, slither::PBPlayerGameRound palyerRoundInfo);
	void updatePlayerStatistics(uint64_t playerId, slither::PBPlayerGameRound roundInfo);
	
};

#define  playerStatisticsMgr PlayerStatisticsMgr::GetInstance()

#endif