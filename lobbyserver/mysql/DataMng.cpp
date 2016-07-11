#include "DataMng.h"
#include "../config/GradeConfig.h"
#include <boost/algorithm/string.hpp>
#include "../config/SkinConfig.h"
#include "../config/ServerConfig.h"
#include <boost/algorithm/string.hpp>

#include "../lzSDK/lzSDK.h"

string getPlayerNewRandName(uint64_t playerId);


DataMng::DataMng() {

}

DataMng::~DataMng() {

}

bool DataMng::InitMysql(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb) {
	if (!m_sqlConn.Init(szHost, nPort, szUser, szPasswd, szDb)) {
		ERRORLOG("init mysql connection failed, mysql ip=[" << szHost << "], port=[" << nPort << "], user=[" << szUser << "], pwd=[" << szPasswd << "], db=[" << szDb << "]");
		return false;
	}

	return true;
}

bool DataMng::GetPlayerDataByAccount(const string& strAccountId, const string& ip, slither::PBPlayerInfo& pbPlayerInfo, slither::VerifyTokenReq& reqInfo) {
	slither::PBPlayerBase* playerBaseInfo = pbPlayerInfo.mutable_playerbase();
	bool isNewPlayer = false;
	int findPlayerBaseInfoResult = FindPlayerBaseInfoByAccountId(strAccountId, playerBaseInfo);
	if (findPlayerBaseInfoResult == -1){
		return false;
	}
	else if (findPlayerBaseInfoResult == -2){	// 玩家不存在，新建角色
		if (!CreateNewPlayer(strAccountId)) {
			ERRORLOG("create new player failed, account id=[" << strAccountId.c_str() << "]");
			return false;
		}
		pbPlayerInfo.set_isnewplayer(1);
		isNewPlayer = true;
	}

	if (findPlayerBaseInfoResult == -2){
		findPlayerBaseInfoResult = FindPlayerBaseInfoByAccountId(strAccountId, playerBaseInfo);
		if (findPlayerBaseInfoResult != 0){		// 如果再次失败说明创建有问题，不再处理
			return false;
		}
	}


	if (isNewPlayer)
	{
		string firstRandName = getPlayerNewRandName(playerBaseInfo->playerid());
		playerBaseInfo->set_playername(firstRandName);

		UpdatePlayerBaseInfo(*playerBaseInfo);

		string nowTimeStr = cputil::GetLocalTime();
		UpdatePlayerCreateTime(playerBaseInfo->playerid(), nowTimeStr);

		if (!CreateNewPlayerSkins(playerBaseInfo->playerid())){
			ERRORLOG("create new player skin Info failed");
			return false;
		}
		else{
			int randSkinId = skinConfig->getRandCanInitSkinId();

			slither::PBPlayerSkins dbSkinInfo;
			FindPlayerSkinByPlayerId(pbPlayerInfo.playerbase().playerid(), &dbSkinInfo);

			dbSkinInfo.set_useskinid(randSkinId);
			if (!UpdatePlayerSkin(pbPlayerInfo.playerbase().playerid(), dbSkinInfo)){
				return false;
			}
		}

		lzSDKMgr->postRoleCreateToSDK(strAccountId, ip, playerBaseInfo, reqInfo);
	}

	if (findPlayerBaseInfoResult == 0 && playerBaseInfo->playerid() > 0){
		// 获取玩家统计数据
		slither::PBStatistics* pPBStatistics = pbPlayerInfo.mutable_statistics();
		int excuCode = FindPlayerStatisticsByPlayerId(pbPlayerInfo.playerbase().playerid(), pPBStatistics);
		if (excuCode == -2){
			pbPlayerInfo.clear_statistics();
		}

		int randSkinId = skinConfig->getRandCanInitSkinId();

		slither::PBPlayerSkins dbSkinInfo;
		FindPlayerSkinByPlayerId(pbPlayerInfo.playerbase().playerid(), &dbSkinInfo);
		if (skinConfig->isCanInitSkinId(dbSkinInfo.useskinid()))
		{
			dbSkinInfo.set_useskinid(randSkinId);
			UpdatePlayerSkin(pbPlayerInfo.playerbase().playerid(), dbSkinInfo);
		}

		// 获取皮肤数据
		slither::PBPlayerSkins* playerSkins = pbPlayerInfo.mutable_playerskins();
		FindPlayerSkinByPlayerId(pbPlayerInfo.playerbase().playerid(), playerSkins);
	}
	return true;
}


bool DataMng::GetPlayerDataByPlayerId(const uint64_t playerId, slither::PBPlayerInfo& pbPlayerInfo) {
	slither::PBPlayerBase* playerBaseInfo = pbPlayerInfo.mutable_playerbase();
	int findPlayerBaseInfoResult = FindPlayerBaseInfoByPlayerId(playerId, playerBaseInfo);
	if (findPlayerBaseInfoResult != 0 && playerBaseInfo->playerid() != playerId){
		return false;
	}


	if (findPlayerBaseInfoResult == 0 && playerBaseInfo->playerid() == playerId){
		// 获取玩家统计数据
		slither::PBStatistics* pPBStatistics = pbPlayerInfo.mutable_statistics();
		int excuCode = FindPlayerStatisticsByPlayerId(pbPlayerInfo.playerbase().playerid(), pPBStatistics);
		if (excuCode == -2){
			pbPlayerInfo.clear_statistics();
		}

		// 获取皮肤数据
		slither::PBPlayerSkins* playerSkins = pbPlayerInfo.mutable_playerskins();
		FindPlayerSkinByPlayerId(pbPlayerInfo.playerbase().playerid(), playerSkins);
	}
	return true;
}


bool DataMng::GetPlayerDataByName(const string playerName, slither::PBPlayerInfo& pbPlayerInfo,bool isNeedNameFullSame) {
	slither::PBPlayerBase* playerBaseInfo = pbPlayerInfo.mutable_playerbase();
	int findPlayerBaseInfoResult = FindPlayerBaseInfoByPlayerName(playerName, playerBaseInfo, isNeedNameFullSame);
	if (findPlayerBaseInfoResult != 0 && playerBaseInfo->playerid()<= 0){
		return false;
	}


	if (findPlayerBaseInfoResult == 0){
		// 获取玩家统计数据
		slither::PBStatistics* pPBStatistics = pbPlayerInfo.mutable_statistics();
		int excuCode = FindPlayerStatisticsByPlayerId(pbPlayerInfo.playerbase().playerid(), pPBStatistics);
		if (excuCode == -2){
			pbPlayerInfo.clear_statistics();
		}

		// 获取皮肤数据
		slither::PBPlayerSkins* playerSkins = pbPlayerInfo.mutable_playerskins();
		FindPlayerSkinByPlayerId(pbPlayerInfo.playerbase().playerid(), playerSkins);
	}
	return true;
}





bool DataMng::CreateNewPlayer(const string& strAccountId) {
	string strInsertSql = "insert into player(sdk_account) values('" + strAccountId + "')";
	if (!m_sqlConn.ExecuteQuery(strInsertSql.c_str())) {
		ERRORLOG("execute sql=[" << strInsertSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return false;
	}

	return true;
}

void DataMng::ReadPlayerBaseData(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBPlayerBase* pPBPlayerBase) {
	// 获取玩家基本信息
	uint64_t uPlayerId			= boost::lexical_cast<uint64_t>(row[0]);
	uint32_t uLevel				= boost::lexical_cast<uint32_t>(row[2]);
	uint32_t exp				= boost::lexical_cast<uint32_t>(row[3]);
	string strPlayerName		= row[4];
	uint32_t uGender			= boost::lexical_cast<uint32_t>(row[5]);
	uint32_t totalStar			= boost::lexical_cast<uint32_t>(row[6]);
	uint32_t uAge				= boost::lexical_cast<uint32_t>(row[7]);
	string strInvitationCode	= row[8];
	uint32_t uAcceptAttention	= boost::lexical_cast<uint32_t>(row[9]);
	uint32_t uAcceptPraise		= boost::lexical_cast<uint32_t>(row[10]);
	uint32_t uOfflineTime		= boost::lexical_cast<uint32_t>(row[11]);
	uint32_t nameModifyTimes	 = boost::lexical_cast<uint32_t>(row[12]);
	uint32_t genderModifyTimes	= boost::lexical_cast<uint32_t>(row[13]);



	//slither::PBPlayerBase* pPBPlayerBase = pbPlayerInfo.mutable_playerbase();
	pPBPlayerBase->set_playerid(uPlayerId);
	pPBPlayerBase->set_level(uLevel);
	pPBPlayerBase->set_exp(exp);
	pPBPlayerBase->set_playername(strPlayerName);
	pPBPlayerBase->set_gender(uGender > 0 ? true : false);

	int grade = gradeConfig->getGradeByTotalStar(totalStar);
	int currentGradeStar = totalStar;
	if (grade > 1)
	{
		int previousGradeMaxStar = gradeConfig->getGradeMaxStarByGrade((grade - 1));
		currentGradeStar = totalStar - previousGradeMaxStar;
	}
	if (currentGradeStar > 0){
		--currentGradeStar;
	}

	pPBPlayerBase->set_grade(grade);
	pPBPlayerBase->set_gradestars(currentGradeStar);
	pPBPlayerBase->set_totalstars(totalStar);


	pPBPlayerBase->set_age(uAge);
	pPBPlayerBase->set_invitationcode(strInvitationCode);
	pPBPlayerBase->set_acceptattention(uAcceptAttention > 0 ? true : false);
	pPBPlayerBase->set_acceptpraise(uAcceptPraise > 0 ? true : false);
	pPBPlayerBase->set_offlinetime(uOfflineTime);
	pPBPlayerBase->set_namemodifytimes(nameModifyTimes);
	pPBPlayerBase->set_gendermodifytimes(genderModifyTimes);

	return;
}

void DataMng::ReadPlayerStatistics(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBStatistics* pPBStatistics) {
	uint32_t uTotalGameRound		= boost::lexical_cast<uint32_t>(row[1]);
	uint32_t uTotalChampionTimes	= boost::lexical_cast<uint32_t>(row[2]);
	uint32_t uKillSnakeTimes		= boost::lexical_cast<uint32_t>(row[3]);
	uint32_t uOnceKillMaxNum        = boost::lexical_cast<uint32_t>(row[4]);
	uint64_t uTotalEatNum			= boost::lexical_cast<uint64_t>(row[5]);
	uint64_t uOnceMaxEatNum			= boost::lexical_cast<uint64_t>(row[6]);
	uint32_t uMaxLength				= boost::lexical_cast<uint32_t>(row[7]);
	uint32_t uAttentionNum			= boost::lexical_cast<uint32_t>(row[8]);
	uint32_t uFansNum				= boost::lexical_cast<uint32_t>(row[9]);
	uint32_t uPraiseNum				= boost::lexical_cast<uint32_t>(row[10]);

	//slither::PBStatistics* pPBStatistics = pbPlayerInfo.mutable_statistics();
	pPBStatistics->set_totalgameround(uTotalGameRound);
	pPBStatistics->set_totalchampiontimes(uTotalChampionTimes);
	pPBStatistics->set_killsnaketimes(uKillSnakeTimes);
	pPBStatistics->set_killmaxtimes(uOnceKillMaxNum);
	pPBStatistics->set_totaleatnum(uTotalEatNum);
	pPBStatistics->set_oncemaxeatnum(uOnceMaxEatNum);
	pPBStatistics->set_maxlength(uMaxLength);
	pPBStatistics->set_attentionnum(uAttentionNum);
	pPBStatistics->set_fansnum(uFansNum);
	pPBStatistics->set_praisenum(uPraiseNum);

	return;
}



void DataMng::ReadPlayerSkinList(cputil::CRecordSet& record, MYSQL_ROW row, slither::PBPlayerSkins* pPBSkins) 
{
	string skinIdsStr = "";
		
	if (row[2]){
		skinIdsStr = row[2];
	}

	int useSkinId = boost::lexical_cast<int>(row[3]);

	
	vector<string> splitIds;
	boost::split(splitIds, skinIdsStr, boost::is_any_of(","));

	for (size_t i = 0; i<splitIds.size(); i++)
	{
		string temp = splitIds[i];
		if (!temp.empty() && temp.length() > 0 && temp != "")
		{
			int id = boost::lexical_cast<int>(temp);
			pPBSkins->add_skinlist(id);
		}
	}
	pPBSkins->set_useskinid(useSkinId);

	return;
}


bool DataMng::WritePlayerBaseData(const slither::PBPlayerBase& pbPlayerBase) {
	//const slither::PBPlayerBase& pbPlayerBase = pbPlayerInfo.playerbase();
	string name = pbPlayerBase.playername();
	int nameLen = name.length();
	char *to = new char[nameLen];
	unsigned long realNameNum = mysql_real_escape_string(m_sqlConn.Mysql(), to, pbPlayerBase.playername().c_str(), nameLen);
	string realName(to);

	string strSaveSql = \
		"update player set level="	+ boost::lexical_cast<string>(pbPlayerBase.level()) + ", " +
		"exp="						+ boost::lexical_cast<string>(pbPlayerBase.exp()) + "," +
		//"player_name='"				+ pbPlayerBase.playername() + "'," +
		"player_name='"				+ realName + "'," +
		"gender="					+ boost::lexical_cast<string>(pbPlayerBase.gender()) + ", " +
		"total_star="				+ boost::lexical_cast<string>(pbPlayerBase.totalstars()) + ", " +
		"age="						+ boost::lexical_cast<string>(pbPlayerBase.age()) + ", " +
		"accept_attention="			+ boost::lexical_cast<string>(pbPlayerBase.acceptattention()) + ", " +
		"accept_praise="			+ boost::lexical_cast<string>(pbPlayerBase.acceptpraise()) + ", " +
		"offline_time="				+ boost::lexical_cast<string>(pbPlayerBase.offlinetime()) + ", " +
		"name_modify_times="		+ boost::lexical_cast<string>(pbPlayerBase.namemodifytimes()) + ", " +
		"gender_modify_times="		+ boost::lexical_cast<string>(pbPlayerBase.gendermodifytimes()) +
		" where player_id="			+ boost::lexical_cast<string>(pbPlayerBase.playerid());
	
	if (!m_sqlConn.ExecuteQuery(strSaveSql.c_str())) {
		ERRORLOG("execute sql=[" << strSaveSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return false;
	}

	return true;
}

//

void DataMng::WritePlayerStatistics(const uint64_t playerId,const slither::PBStatistics& pbStatistics) {
	//const slither::PBStatistics& pbStatistics = pbPlayerInfo.statistics();
	//mysql_real_escape_string
	string strSaveSql = \
		"update statistics set total_game_round="	+ boost::lexical_cast<string>(pbStatistics.totalgameround()) + ", " +
		"total_champion_times="						+ boost::lexical_cast<string>(pbStatistics.totalchampiontimes()) + ", " +
		"kill_snake_times="							+ boost::lexical_cast<string>(pbStatistics.killsnaketimes()) + ", " +
		"once_kill_max_times="						+ boost::lexical_cast<string>(pbStatistics.killmaxtimes()) + ", " +
		"total_eat_num="							+ boost::lexical_cast<string>(pbStatistics.totaleatnum()) + ", " +
		"once_max_eat_num="							+ boost::lexical_cast<string>(pbStatistics.oncemaxeatnum()) + ", " +
		"max_length="								+ boost::lexical_cast<string>(pbStatistics.maxlength()) + ", " + 
		"attention_num="							+ boost::lexical_cast<string>(pbStatistics.attentionnum()) + ", " +
		"praise_num="								+ boost::lexical_cast<string>(pbStatistics.praisenum()) +
		" where player_id="							+ boost::lexical_cast<string>(playerId);

	if (!m_sqlConn.ExecuteQuery(strSaveSql.c_str())) {
		ERRORLOG("execute sql=[" << strSaveSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return;
	}

	return;
}


string getSkinListIdsStr(const slither::PBPlayerSkins& skinsInfo)
{
	int skinIdNum = skinsInfo.skinlist().size();
	string skinIdsStr = "";
	for (int i = 0; i < skinIdNum; i++)
	{
		skinIdsStr += ((boost::lexical_cast<string>(skinsInfo.skinlist().Get(i)))+",");
	}
	
	return skinIdsStr;
}

bool DataMng::WritePlayerSkins(const uint64_t playerId, const slither::PBPlayerSkins& skinsInfo) {
	string strSaveSql = \
		"update player_skin_info set skin_list='"	+ getSkinListIdsStr(skinsInfo) + "', " +
		"use_skin_id="								+ boost::lexical_cast<string>(skinsInfo.useskinid()) + 
		" where player_id="							+ boost::lexical_cast<string>(playerId);

	if (!m_sqlConn.ExecuteQuery(strSaveSql.c_str())) {
		ERRORLOG("execute sql=[" << strSaveSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return false;
	}

	return true;
}


bool DataMng::updatePlayerTimeField(const uint64_t playerId, const string fieldName,const string datetimeStr) {
	string strSaveSql = \
		"update player set " + fieldName + "='" + datetimeStr+
		"'  where player_id=" + boost::lexical_cast<string>(playerId);

	if (!m_sqlConn.ExecuteQuery(strSaveSql.c_str())) {
		ERRORLOG("execute sql=[" << strSaveSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return false;
	}

	return true;
}



bool DataMng::CreateNewPlayerStatistics(const uint64_t playerId) {
	string strInsertSql = "insert into statistics(player_id) values(" + boost::lexical_cast<string>(playerId)+")";
	if (!m_sqlConn.ExecuteQuery(strInsertSql.c_str())) {
		ERRORLOG("execute sql=[" << strInsertSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return false;
	}
	return true;
}



void DataMng::WriteGameRoundInfo(const uint32_t gameRoundId, const bool isValid, const slither::PBGameRound gameRoundInfo) {
	int isValidInt = isValid ? 1 : 0;
	string strInsertSql = "INSERT into game_round_info VALUES("
																+ boost::lexical_cast<string>(gameRoundId)+","
																+ boost::lexical_cast<string>(gameRoundInfo.gameroomid()) + ","
																+ boost::lexical_cast<string>(gameRoundInfo.gametype() )+ ","
																+ "'" +boost::lexical_cast<string>(gameRoundInfo.starttime())+ "',"
																+ "'" + boost::lexical_cast<string>(gameRoundInfo.roomfulltime()) + "',"
																+ boost::lexical_cast<string>(isValidInt)+","
																+ boost::lexical_cast<string>(gameRoundInfo.maxenternum()) + ","
																+ boost::lexical_cast<string>(gameRoundInfo.roundendnum())
															   +")";
	if (!m_sqlConn.ExecuteQuery(strInsertSql.c_str())) {
		ERRORLOG("execute sql=[" << strInsertSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
	}
}



void DataMng::WritePlayerGameRoundInfo(const uint32_t gameRoundId, const int starChange,
										const int expChange, const int goldChange,
										const slither::PBPlayerGameRound playerGameRoundInfo,
										const int startStar, const int startExp) 
{
	string strInsertSql = "INSERT into player_game_round_info VALUES(" + boost::lexical_cast<string>(gameRoundId)+","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.playerid()) + ","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.skinid()) + ","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.teamid()) + ","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.gameendstatus()) + ","
																		+ "'" + boost::lexical_cast<string>(playerGameRoundInfo.enterroomtime()) + "',"
																		+ boost::lexical_cast<string>(playerGameRoundInfo.gameendmass()) + ","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.totaleatnum()) + ","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.deadtimes()) + ","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.killsnaketimes()) + ","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.speedupcost()) + ","
																		+ boost::lexical_cast<string>(playerGameRoundInfo.rank()) + ","

																		+ boost::lexical_cast<string>(starChange)+","
																		+ boost::lexical_cast<string>(expChange)+","
																		+ boost::lexical_cast<string>(goldChange)+","
																		+ boost::lexical_cast<string>(startStar)+","
																		+ boost::lexical_cast<string>(startExp)
																		+ ")";
	if (!m_sqlConn.ExecuteQuery(strInsertSql.c_str())) {
		ERRORLOG("execute sql=[" << strInsertSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
	}
}






//返回值为-1时，表示程序执行发行错误
//返回值为-2时，表示没有数据
int DataMng::FindPlayerStatisticsByPlayerId(const uint64_t playerId, slither::PBStatistics* pPBStatistics)
{
	cputil::CRecordSet statisticsRecord;
	string strQuerySql = "select *from statistics where player_id=" + boost::lexical_cast<string>(playerId);
	if (!m_sqlConn.Execute(strQuerySql.c_str(), statisticsRecord)) {
		ERRORLOG("execute sql=[" << strQuerySql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return -1;
	}
	MYSQL_ROW statisticsRow = NULL;
	statisticsRecord.FetchRow(statisticsRow);
	if (statisticsRow) {				// 如果有数据
		ReadPlayerStatistics(statisticsRecord, statisticsRow, pPBStatistics);
	}
	else{
		return -2;
	}
	return 0;
}

void DataMng::UpdatePlayerStatistics(const uint64_t playerId, const slither::PBStatistics& pPBStatistics)
{
	WritePlayerStatistics(playerId, pPBStatistics);
}


//返回值为-1时，表示程序执行发行错误
//返回值为-2时，表示没有数据
int DataMng::FindPlayerBaseInfoByPlayerId(const uint64_t playerId, slither::PBPlayerBase* playerBaseInfo)
{
	string strQuerySql = "select * from player where player_id = "+ boost::lexical_cast<string>(playerId);

	cputil::CRecordSet record;
	if (!m_sqlConn.Execute(strQuerySql.c_str(), record)) {
		ERRORLOG("execute sql=[" << strQuerySql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return -1;
	}

	MYSQL_ROW playerRow = NULL;
	record.FetchRow(playerRow);
	if (playerRow) {				// 如果有数据
		ReadPlayerBaseData(record, playerRow, playerBaseInfo);
	}
	else{
		return -2;
	}
	return 0;
}

int DataMng::FindPlayerBaseInfoByPlayerName(const string playerName, slither::PBPlayerBase* playerBaseInfo, bool isNeedNameFullSame)
{
	int nameLen = playerName.length();
	char *to = new char[nameLen];
	unsigned long realNameNum = mysql_real_escape_string(m_sqlConn.Mysql(), to, playerName.c_str(), nameLen);
	string realName(to);
	string strQuerySql = "";
	if (isNeedNameFullSame)
	{
		strQuerySql = "select * from player where player_name = '" + boost::lexical_cast<string>(realName)+"'";
	}
	else{
		strQuerySql = "select * from player where player_name like '%" + boost::lexical_cast<string>(realName)+"%'";
	}


	cputil::CRecordSet record;
	if (!m_sqlConn.Execute(strQuerySql.c_str(), record)) {
		ERRORLOG("execute sql=[" << strQuerySql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return -1;
	}

	MYSQL_ROW playerRow = NULL;
	record.FetchRow(playerRow);
	if (playerRow) {				// 如果有数据
		ReadPlayerBaseData(record, playerRow, playerBaseInfo);
	}
	else{
		return -2;
	}
	return 0;
}



//返回值为-1时，表示程序执行发行错误
//返回值为-2时，表示没有数据
int DataMng::FindPlayerBaseInfoByAccountId(const string accountStr, slither::PBPlayerBase* playerBaseInfo)
{
	string strQuerySql = "select * from player where sdk_account = '" + accountStr + "'";

	cputil::CRecordSet record;
	if (!m_sqlConn.Execute(strQuerySql.c_str(), record)) {
		ERRORLOG("execute sql=[" << strQuerySql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return -1;
	}

	MYSQL_ROW playerRow = NULL;
	record.FetchRow(playerRow);
	if (playerRow) {				// 如果有数据
		ReadPlayerBaseData(record, playerRow, playerBaseInfo);
	}
	else{
		return -2;
	}
	return 0;
}


uint32_t DataMng::FindMaxGameRoindInfo()
{
	uint32_t maxRoundId = 0;
	cputil::CRecordSet maxRecord;
	string strQuerySql = "select max(round_id) from game_round_info";
	if (!m_sqlConn.Execute(strQuerySql.c_str(), maxRecord)) {
		ERRORLOG("execute sql=[" << strQuerySql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return -1;
	}
	MYSQL_ROW maxRow = NULL;
	maxRecord.FetchRow(maxRow);
	if (maxRow && maxRow[0]) {				// 如果有数据
		maxRoundId = boost::lexical_cast<uint32_t>(maxRow[0]);
	}
	return maxRoundId;
}



bool DataMng::UpdatePlayerBaseInfo(const slither::PBPlayerBase& pbPlayerBase)
{
	return WritePlayerBaseData(pbPlayerBase);
}

//////////////////////
bool DataMng::CreateNewPlayerSkins(const uint64_t playerId) {
	string strInsertSql = "insert into player_skin_info(player_id) values(" + boost::lexical_cast<string>(playerId)+")";
	if (!m_sqlConn.ExecuteQuery(strInsertSql.c_str())) {
		ERRORLOG("execute sql=[" << strInsertSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return false;
	}
	return true;
}
//返回值为-1时，表示程序执行发行错误
//返回值为-2时，表示没有数据
int DataMng::FindPlayerSkinByPlayerId(const uint64_t playerId, slither::PBPlayerSkins* skinInfo)
{
	int resultCode = -1;
	cputil::CRecordSet skinRecord;
	string strQuerySql = "select *from player_skin_info where player_id=" + boost::lexical_cast<string>(playerId);

	if (!m_sqlConn.Execute(strQuerySql.c_str(), skinRecord)) {
		ERRORLOG("execute sql=[" << strQuerySql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		resultCode = -1;
	}
	else{
		MYSQL_ROW skinRow = NULL;
		skinRecord.FetchRow(skinRow);
		if (skinRow) {							// 如果有数据
			ReadPlayerSkinList(skinRecord, skinRow, skinInfo);
			resultCode = 0;
		}
		else{
			resultCode = -2;
		}
	}

	return resultCode;
}

bool DataMng::UpdatePlayerSkin(const uint64_t playerId, const slither::PBPlayerSkins& skinInfo)
{
	return WritePlayerSkins(playerId, skinInfo);
}




bool DataMng::UpdatePlayerCreateTime(const uint64_t playerId,const string datetimeStr) {
	return updatePlayerTimeField(playerId, PLAYER_CREATE_TIME_FIELD, datetimeStr);
}

bool DataMng::UpdatePlayerLastLoginTime(const uint64_t playerId,const string datetimeStr) {
	return updatePlayerTimeField(playerId, PLAYER_LAST_LOGIN_TIME_FIELD,datetimeStr);
}


bool DataMng::UpdatePlayerLastLoginOut(const uint64_t playerId,const string datetimeStr) {
	return updatePlayerTimeField(playerId, PLAYER_LAST_LOGINOUT_TIME_FIELD, datetimeStr);
}


void DataMng::InsertGameRoundInfo(const uint32_t gameRoundId, const bool isValid, const slither::PBGameRound gameRoundInfo)
{
	WriteGameRoundInfo(gameRoundId, isValid, gameRoundInfo);
}

void DataMng::InsertPlayerGameRoundInfo(const uint32_t gameRoundId, const int starChange, 
										const int expChange, const int goldChange, 
										const slither::PBPlayerGameRound playerGameRoundInfo,
										const int startStar,const int startExp)
{
	WritePlayerGameRoundInfo(gameRoundId, starChange, expChange, goldChange, playerGameRoundInfo,startStar,startExp);
}






//返回值为-1时，表示程序执行发行错误
//返回值为-2时，表示没有数据
vector<PlayerAttentions> DataMng::FindPlayerAttentivedAndFans(const uint64_t playerId)
{
	vector<PlayerAttentions> playerAttentions;
	string playerIdStr = boost::lexical_cast<string>(playerId);
	cputil::CRecordSet fansRecords;
	string strQuerySql = "select * from player_attentions where status = 0 and (player_id=" + playerIdStr + " or fans_id = " + playerIdStr + ")";

	if (!m_sqlConn.Execute(strQuerySql.c_str(), fansRecords)) {
		ERRORLOG("execute sql=[" << strQuerySql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
	}
	else{
		bool isHaveData = false;

		MYSQL_ROW  fansRow = NULL;
		fansRecords.FetchRow(fansRow); 
		do
		{
			if (fansRow) {
				PlayerAttentions* attention = ReadPlayerAttentions(fansRecords, fansRow);
				if (attention){
					playerAttentions.push_back(*attention);
				}

				MYSQL_ROW  fansRow = NULL;
				fansRecords.FetchRow(fansRow);

				isHaveData = fansRow ? true : false;
			}
		} while (isHaveData);
	}

	return playerAttentions;
}


PlayerAttentions* DataMng::ReadPlayerAttentions(cputil::CRecordSet& record, MYSQL_ROW row) {
	uint64_t id			=	boost::lexical_cast<uint64_t>(row[0]);
	uint64_t palyerId = boost::lexical_cast<uint64_t>(row[1]);
	uint64_t fansId = boost::lexical_cast<uint64_t>(row[2]);
	int	status			=	boost::lexical_cast<int>(row[3]);
	
	return new PlayerAttentions(id, palyerId, fansId, status);
}


bool DataMng::WritePlayerAttention(const uint64_t playerId, const uint64_t fansId) {
	string strInsertSql = "INSERT into player_attentions (player_id,fans_id) VALUES("
		+ boost::lexical_cast<string>(playerId)+","
		+ boost::lexical_cast<string>(fansId)
		+")";
	if (!m_sqlConn.ExecuteQuery(strInsertSql.c_str())) {
		ERRORLOG("execute sql=[" << strInsertSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return false;
	}
	return true;
}

bool DataMng::InsertPlayerAttentionInfo(uint64_t playerId, uint64_t fansId)
{
	return WritePlayerAttention(playerId,fansId);
}


bool DataMng::UpdatePlayerAttentionInfoStatus(const uint64_t playerId, const uint64_t fansId, const int status)
{
	string tableName = "player_attentions";
	string conditionStr = "where player_id = " + boost::lexical_cast<string> (playerId)+" and fans_id=" + boost::lexical_cast<string> (fansId);
	string updateFieldName = "status";
	string updateValue = boost::lexical_cast<string> (status);

	return UpdateTalbeField(tableName, updateFieldName, updateValue, conditionStr);
}





//////////////////////////////////////////////////
const int TOTAL_LEN= 16;


string getPlayerNewRandName(uint64_t playerId){
	//string perfix = gpServerConfig->GetPlayerNamePerFix();
	//string nameId = boost::lexical_cast<string>(playerId);
	//string p = "p";

	//int len = nameId.size() + perfix.size() + p.size();
	//const int needStrLen = 16 - len;

	//char randStrArr[TOTAL_LEN+1];
	//cputil::rand_str(randStrArr, TOTAL_LEN);

	//string randTotalStr(randStrArr, TOTAL_LEN);
	//string randStr = boost::erase_tail_copy(randTotalStr, TOTAL_LEN-needStrLen);

	//string name = perfix.append(randStr).append(p).append(nameId);
	//char reName[sizeof(name)];
	//strcpy(reName, name.c_str());

	//return	reName;

	return boost::lexical_cast<string>(playerId);
}







bool DataMng::UpdateTalbeField(const string tableName, const string fieldName, const string value,const string conditionStr){

	string strSaveSql = \
		"update " + tableName + " set " + fieldName + "=" + value + conditionStr;

	if (!m_sqlConn.ExecuteQuery(strSaveSql.c_str())) {
		ERRORLOG("execute sql=[" << strSaveSql.c_str() << "] failed. error=[" << m_sqlConn.GetErrInfo() << "]");
		return false;
	}

	return true;

}