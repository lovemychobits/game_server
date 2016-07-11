#ifndef LOBBYSERVER_DATAMNG_H
#define LOBBYSERVER_DATAMNG_H

#include "../header.h"
#include "mysqlVO/MysqlObjVO.h"
#include "../../utils/Mysql.h"
#include "../../utils/Utility.h"
#include "../../protocol/slither_player.pb.h"
#include "../../protocol/slither_server.pb.h"
#include "../../protocol/slither_lobby.pb.h"

class DataMng
{
public:
	DataMng();
	~DataMng();

	const string PLAYER_CREATE_TIME_FIELD = "createTime";
	const string PLAYER_LAST_LOGIN_TIME_FIELD = "last_login_time";
	const string PLAYER_LAST_LOGINOUT_TIME_FIELD = "last_loginOut_time";

	bool InitMysql(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb);

	bool GetPlayerDataByAccount(const string& strAccountId, const string& ip, slither::PBPlayerInfo& pbPlayerInfo, slither::VerifyTokenReq& reqInfo);							// 获取角色数据
	bool GetPlayerDataByPlayerId(const uint64_t playerId, slither::PBPlayerInfo& pbPlayerInfo);
	bool GetPlayerDataByName(const string playerName, slither::PBPlayerInfo& pbPlayerInfo, bool isNeedNameFullSame);
	void SavePlayerData(const slither::PBPlayerInfo& pbPlayerInfo);													// 保存玩家数据


	void UpdatePlayerStatistics(const uint64_t playerId, const slither::PBStatistics& pPBStatistics);
	//返回值为-1时，表示程序执行发行错误
	//返回值为-2时，表示没有数据
	int FindPlayerStatisticsByPlayerId(const uint64_t playerId, slither::PBStatistics* pPBStatistics);


	bool CreateNewPlayerStatistics(const uint64_t playerId);
	//返回值为-1时，表示程序执行发行错误
	//返回值为-2时，表示没有数据
	int FindPlayerBaseInfoByPlayerId(const uint64_t playerId, slither::PBPlayerBase* playerBaseInfo);
	int FindPlayerBaseInfoByAccountId(const string accountStr, slither::PBPlayerBase* playerBaseInfo);
	int FindPlayerBaseInfoByPlayerName(const string playerName, slither::PBPlayerBase* playerBaseInfo, bool isNeedNameFullSame);
	//Update玩家基本信息
	bool UpdatePlayerBaseInfo(const slither::PBPlayerBase& pbPlayerBase);

	bool UpdatePlayerCreateTime(const uint64_t playerId, const string datetimeStr);
	bool UpdatePlayerLastLoginTime(const uint64_t playerId, const string datetimeStr);
	bool UpdatePlayerLastLoginOut(const uint64_t playerId, const string datetimeStr);


	bool CreateNewPlayerSkins(const uint64_t playerId);
	bool UpdatePlayerSkin(const uint64_t playerId, const slither::PBPlayerSkins& skinInfo);
	//返回值为-1时，表示程序执行发行错误
	//返回值为-2时，表示没有数据
	int FindPlayerSkinByPlayerId(const uint64_t playerId, slither::PBPlayerSkins* skinInfo);

	void InsertGameRoundInfo(const uint32_t gameRoundId, const bool isValid, const slither::PBGameRound gameRoundInfo);

	void InsertPlayerGameRoundInfo(const uint32_t gameRoundId, const int starChange,
		const int expChange, const int goldChange,
		const slither::PBPlayerGameRound playerGameRoundInfo,
		const int startStar, const int startExp);

	uint32_t FindMaxGameRoindInfo();


	//返回值为-1时，表示程序执行发行错误
	//返回值为-2时，表示没有数据
	vector<PlayerAttentions> FindPlayerAttentivedAndFans(const uint64_t playerId);
	
	bool InsertPlayerAttentionInfo(const uint64_t playerId, const uint64_t fansId);
	bool UpdatePlayerAttentionInfoStatus(const uint64_t playerId, const uint64_t fansId,const int status);


private:
	bool CreateNewPlayer(const string& strAccountId);																// 创建角色

	// 读取数据相关
	void ReadPlayerBaseData(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBPlayerBase* pPBPlayerBase);		// 读取角色数据
	void ReadPlayerStatistics(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBStatistics* pPBStatistics);		// 读取角色统计数据
	void ReadPlayerSkinList(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBPlayerSkins* pPBSkinso);			// 读取角色皮肤信息

	// 保存数据相关
	bool WritePlayerBaseData(const slither::PBPlayerBase& pbPlayerBase);											// 保存玩家基础数据
	bool updatePlayerTimeField(const uint64_t playerId, const string fieldName, const string datetimeStr);
	void WritePlayerStatistics(const uint64_t playerId, const slither::PBStatistics& pbStatistics);					// 保存玩家统计数据
	bool WritePlayerSkins(const uint64_t playerId, const slither::PBPlayerSkins& skinsInfo);						// 保存玩家皮肤信息

	void WriteGameRoundInfo(const uint32_t gameRoundId, const bool isValid, const slither::PBGameRound gameRoundInfo);	// 

	void WritePlayerGameRoundInfo(const uint32_t gameRoundId, const int starChange,
		const int expChange, const int goldChange,
		const slither::PBPlayerGameRound playerGameRoundInfo,
		const int startStar, const int startExp);						// 

	//
	//通用Update方法
	//const string tableName, const string fieldName, const string value, const string conditionStr
	//tableName :表名
	//fieldName ：要更新的列名
	//value	   ：要更新的值
	//conditionStr ：限制条件
	bool UpdateTalbeField(const string tableName, const string fieldName, const string value, const string conditionStr);


	PlayerAttentions* ReadPlayerAttentions(cputil::CRecordSet& record, MYSQL_ROW row);
	bool WritePlayerAttention(const uint64_t playerId, const uint64_t fansId);

private:
	cputil::CMysqlConnection m_sqlConn;
};

#endif