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

	bool GetPlayerDataByAccount(const string& strAccountId, const string& ip, slither::PBPlayerInfo& pbPlayerInfo, slither::VerifyTokenReq& reqInfo);							// ��ȡ��ɫ����
	bool GetPlayerDataByPlayerId(const uint64_t playerId, slither::PBPlayerInfo& pbPlayerInfo);
	bool GetPlayerDataByName(const string playerName, slither::PBPlayerInfo& pbPlayerInfo, bool isNeedNameFullSame);
	void SavePlayerData(const slither::PBPlayerInfo& pbPlayerInfo);													// �����������


	void UpdatePlayerStatistics(const uint64_t playerId, const slither::PBStatistics& pPBStatistics);
	//����ֵΪ-1ʱ����ʾ����ִ�з��д���
	//����ֵΪ-2ʱ����ʾû������
	int FindPlayerStatisticsByPlayerId(const uint64_t playerId, slither::PBStatistics* pPBStatistics);


	bool CreateNewPlayerStatistics(const uint64_t playerId);
	//����ֵΪ-1ʱ����ʾ����ִ�з��д���
	//����ֵΪ-2ʱ����ʾû������
	int FindPlayerBaseInfoByPlayerId(const uint64_t playerId, slither::PBPlayerBase* playerBaseInfo);
	int FindPlayerBaseInfoByAccountId(const string accountStr, slither::PBPlayerBase* playerBaseInfo);
	int FindPlayerBaseInfoByPlayerName(const string playerName, slither::PBPlayerBase* playerBaseInfo, bool isNeedNameFullSame);
	//Update��һ�����Ϣ
	bool UpdatePlayerBaseInfo(const slither::PBPlayerBase& pbPlayerBase);

	bool UpdatePlayerCreateTime(const uint64_t playerId, const string datetimeStr);
	bool UpdatePlayerLastLoginTime(const uint64_t playerId, const string datetimeStr);
	bool UpdatePlayerLastLoginOut(const uint64_t playerId, const string datetimeStr);


	bool CreateNewPlayerSkins(const uint64_t playerId);
	bool UpdatePlayerSkin(const uint64_t playerId, const slither::PBPlayerSkins& skinInfo);
	//����ֵΪ-1ʱ����ʾ����ִ�з��д���
	//����ֵΪ-2ʱ����ʾû������
	int FindPlayerSkinByPlayerId(const uint64_t playerId, slither::PBPlayerSkins* skinInfo);

	void InsertGameRoundInfo(const uint32_t gameRoundId, const bool isValid, const slither::PBGameRound gameRoundInfo);

	void InsertPlayerGameRoundInfo(const uint32_t gameRoundId, const int starChange,
		const int expChange, const int goldChange,
		const slither::PBPlayerGameRound playerGameRoundInfo,
		const int startStar, const int startExp);

	uint32_t FindMaxGameRoindInfo();


	//����ֵΪ-1ʱ����ʾ����ִ�з��д���
	//����ֵΪ-2ʱ����ʾû������
	vector<PlayerAttentions> FindPlayerAttentivedAndFans(const uint64_t playerId);
	
	bool InsertPlayerAttentionInfo(const uint64_t playerId, const uint64_t fansId);
	bool UpdatePlayerAttentionInfoStatus(const uint64_t playerId, const uint64_t fansId,const int status);


private:
	bool CreateNewPlayer(const string& strAccountId);																// ������ɫ

	// ��ȡ�������
	void ReadPlayerBaseData(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBPlayerBase* pPBPlayerBase);		// ��ȡ��ɫ����
	void ReadPlayerStatistics(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBStatistics* pPBStatistics);		// ��ȡ��ɫͳ������
	void ReadPlayerSkinList(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBPlayerSkins* pPBSkinso);			// ��ȡ��ɫƤ����Ϣ

	// �����������
	bool WritePlayerBaseData(const slither::PBPlayerBase& pbPlayerBase);											// ������һ�������
	bool updatePlayerTimeField(const uint64_t playerId, const string fieldName, const string datetimeStr);
	void WritePlayerStatistics(const uint64_t playerId, const slither::PBStatistics& pbStatistics);					// �������ͳ������
	bool WritePlayerSkins(const uint64_t playerId, const slither::PBPlayerSkins& skinsInfo);						// �������Ƥ����Ϣ

	void WriteGameRoundInfo(const uint32_t gameRoundId, const bool isValid, const slither::PBGameRound gameRoundInfo);	// 

	void WritePlayerGameRoundInfo(const uint32_t gameRoundId, const int starChange,
		const int expChange, const int goldChange,
		const slither::PBPlayerGameRound playerGameRoundInfo,
		const int startStar, const int startExp);						// 

	//
	//ͨ��Update����
	//const string tableName, const string fieldName, const string value, const string conditionStr
	//tableName :����
	//fieldName ��Ҫ���µ�����
	//value	   ��Ҫ���µ�ֵ
	//conditionStr ����������
	bool UpdateTalbeField(const string tableName, const string fieldName, const string value, const string conditionStr);


	PlayerAttentions* ReadPlayerAttentions(cputil::CRecordSet& record, MYSQL_ROW row);
	bool WritePlayerAttention(const uint64_t playerId, const uint64_t fansId);

private:
	cputil::CMysqlConnection m_sqlConn;
};

#endif