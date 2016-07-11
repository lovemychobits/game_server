#include "PlayerStatisticsMgr.h"
#include "playerMsg.h"
#include "../mysql/DataMng.h"
#include "../../protocol/slither_lobby.pb.h"
#include "../../protocol/slither_player.pb.h"

#include "../config/GradeConfig.h"
#include "../config/LvExpConfig.h"
#include "../config/LvExpSettlement.h"
#include "../config/LvExpLenConfig.h"
#include "../config/LvExpRankConfig.h"
#include "../config/SkinConfig.h"



DataMng PlayerStatisticsMgr::m_dataMng;

PlayerStatisticsMgr :: ~PlayerStatisticsMgr()
{

}


void PlayerStatisticsMgr::savePlayerStatisticsData(slither::GameRoomEndNty& roomEndNotifyInfo)
{
	int playerNum = roomEndNotifyInfo.gameroundinfo_size();
	bool isValidRound = roomEndNotifyInfo.isvalid();
	int roundRoomId = roomEndNotifyInfo.gameround().gameroomid();

	
	uint32_t gameRoundId = m_dataMng.FindMaxGameRoindInfo() + 1;

	m_dataMng.InsertGameRoundInfo(gameRoundId, isValidRound, roomEndNotifyInfo.gameround());

	if (playerNum > 0)
	{
		for (int i = 0; i < playerNum; i++)
		{
			slither::PBPlayerGameRound roundInfo = roomEndNotifyInfo.gameroundinfo(i);
			uint64_t playerId = roundInfo.playerid();

			
			slither::PBPlayerBase playerBaseInfo;
			int findPlayerResult = m_dataMng.FindPlayerBaseInfoByPlayerId(playerId, &playerBaseInfo);
			if (findPlayerResult == 0 && playerBaseInfo.playerid() > 0){
				updatePlayerBaseInfo(gameRoundId, playerBaseInfo, isValidRound, roundInfo);
				updatePlayerStatistics(playerId, roundInfo);
			}
		}
	}
}


void PlayerStatisticsMgr::updatePlayerBaseInfo(const int gameRoundId, slither::PBPlayerBase playerBaseInfo, bool isNeedUpdateTotalStar, slither::PBPlayerGameRound playerRoundInfo){
	bool isNeedUnLockSkin = false;
	bool isNeedUpdatePlayerData = false;


	uint32_t rank = playerRoundInfo.rank();
	uint32_t maxMass = playerRoundInfo.maxmass();

	int startStarTemp = playerBaseInfo.totalstars();
	int startExpTemp = playerBaseInfo.exp();
	
	
	if (isNeedUpdateTotalStar)
	{
		int startGrade = playerBaseInfo.grade();
		int startTotalStars = playerBaseInfo.totalstars();
		//int addStar = gradeConfig->addStarByGradeAndRank(playerBaseInfo.grade(), boost::lexical_cast<int>(rank));
		int addStar = gradeConfig->addStarByGradeAndRank(startGrade, boost::lexical_cast<int>(rank));
		
		int maxTotalStar = gradeConfig->getMaxTotalStar();
		
		if (addStar != 0){
			int forNum = abs(addStar);
			
			for (int i = 1; i <= forNum; i++)
			{
				if ((startTotalStars <= 1 && addStar < 0) || startTotalStars >= maxTotalStar){				//初始化是1颗星,到1颗星时不再减少
					break;
				}

				int gradeTemp = gradeConfig->getGradeByTotalStar(startTotalStars);
				if (gradeTemp != startGrade){							//每次只能升一个段位
					break;
				}

				if (addStar > 0)
				{
					++startTotalStars;
				}
				else
				{
					--startTotalStars;
				}

				int gradeTemp2 = gradeConfig->getGradeByTotalStar(startTotalStars);
				if (gradeTemp2 != startGrade){
					isNeedUnLockSkin = true;
				}

				if (startTotalStars >= 1)
				{
					isNeedUpdatePlayerData = true;
					playerBaseInfo.set_totalstars(startTotalStars);
				}
			}
		}
	}



	//（Base_Exp*Base_Multiple + 排名加成 + (len/5000)*取长度加成）*Total_Multiple
	int maxExp = lvExpConfig->getMaxExp();
	if (playerBaseInfo.exp() <= maxExp)
	{
		int baseExp = lvExpSettConfig->getBaseExp();
		int baseMultiple = lvExpSettConfig->getBaseMultiple();
		int totalMultiple = lvExpSettConfig->getTotalMultiple();

		int rankAddExp = lvExpRankConfig->getRankExp(rank);
		int baseLen = lvExpLenConfig->getSubsection();
		int lenAddExp = lvExpLenConfig->getValue();

		int lenAddExpMultiple = maxMass / baseLen;

		int addExp = ((baseExp*baseMultiple) + rankAddExp + (lenAddExpMultiple*lenAddExp))*totalMultiple;
		if (addExp > 0){
			isNeedUpdatePlayerData = true;

			int startLv = lvExpConfig->getLvByExp(playerBaseInfo.exp());

			playerBaseInfo.set_exp(playerBaseInfo.exp() + addExp);
			if (playerBaseInfo.exp() > maxExp)
			{
				playerBaseInfo.set_exp(maxExp);
			}

			int level = lvExpConfig->getLvByExp(playerBaseInfo.exp());
			playerBaseInfo.set_level(level);

			if (startLv != level){
				isNeedUnLockSkin = true;
			}
		}
	}


	if (isNeedUpdatePlayerData)
	{
		m_dataMng.UpdatePlayerBaseInfo(playerBaseInfo);
	}


	int starChange = playerBaseInfo.totalstars() - startStarTemp;
	int expChange = playerBaseInfo.exp() - startExpTemp;
	m_dataMng.InsertPlayerGameRoundInfo(gameRoundId, starChange, expChange, 0, playerRoundInfo,startStarTemp,startExpTemp);
	

	//UNLOCK SKIN
	if (isNeedUnLockSkin){
		vector<int> allSkinIds = skinConfig->getAllSkinIds();
		int len = allSkinIds.size();
		for (int i = 0; i < len; i++)
		{
			int skinId = allSkinIds.at(i);
			playerMsgDeal->playerUnLockSkin(playerBaseInfo.playerid(),skinId);
		}
	}
}



void PlayerStatisticsMgr::updatePlayerStatistics(uint64_t playerId, slither::PBPlayerGameRound roundInfo){
	slither::PBStatistics dbInfo;

	int dbFindResult = m_dataMng.FindPlayerStatisticsByPlayerId(playerId, &dbInfo);

	if (dbFindResult == -2)
	{
		bool isCreateSuss = m_dataMng.CreateNewPlayerStatistics(playerId);
		if (!isCreateSuss)
		{
			return;
		}
		else{
			dbFindResult = m_dataMng.FindPlayerStatisticsByPlayerId(playerId, &dbInfo);
			if (dbFindResult == -2)
			{
				return;
			}
		}
	}

	if (dbFindResult == 0)
	{
		/*结束信息中：结束时候的排名*/
		uint32_t roundRank = roundInfo.rank();
		/*结束信息中：击杀人数*/
		uint32_t roundKillNum = roundInfo.killsnaketimes();
		/*结束信息中：吞噬数*/
		uint64_t roundOnceMaxEatNum = roundInfo.totaleatnum();
		/*结束信息中：最高体重[长度]*/
		uint32_t roundMaxMass = roundInfo.maxmass();

		////////////////////////DB 中的信息////////////////////
		/*结束信息中：单场击杀人数记录*/
		uint32_t dbKillMaxNum = dbInfo.killmaxtimes();
		/*结束信息中：单场吞噬记录数*/
		uint64_t dbOnceMaxEatNum = dbInfo.oncemaxeatnum();
		/*结束信息中：最高体重[长度]*/
		uint32_t dbMaxMass = dbInfo.maxlength();



		uint32_t totalGameRound = dbInfo.totalgameround();
		dbInfo.set_totalgameround(++totalGameRound);

		if (roundRank == 1)
		{
			uint32_t totalchampiontimes = dbInfo.totalchampiontimes();
			dbInfo.set_totalchampiontimes(++totalchampiontimes);
		}



		uint32_t totalKillNum = dbInfo.killsnaketimes();
		dbInfo.set_killsnaketimes(totalKillNum + roundKillNum);

		if (roundKillNum > dbKillMaxNum){
			dbInfo.set_killmaxtimes(roundKillNum);
		}


		uint64_t totalEatNum = dbInfo.totaleatnum();
		dbInfo.set_totaleatnum(totalEatNum + roundOnceMaxEatNum);
		if (roundOnceMaxEatNum > dbOnceMaxEatNum)
		{
			dbInfo.set_oncemaxeatnum(roundOnceMaxEatNum);
		}

		if (roundMaxMass > dbMaxMass)
		{
			dbInfo.set_maxlength(roundMaxMass);
		}

		m_dataMng.UpdatePlayerStatistics(playerId, dbInfo);
	}


}