#include "TeamScene.h"
#include "GameRoom.h"
#include "Snake.h"
#include "PlayerMng.h"
#include "Factory.h"

namespace slither {
	TeamScene::TeamScene(GameRoom* pGameRoom) :Scene(pGameRoom) {

	}

	TeamScene::~TeamScene() {

	}

	void TeamScene::OnTimer() {
		boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration diff;											// 统计每个tick的耗时

		// 模拟所有蛇的行为
		SimulateSnakes();

		end = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration rankDiff = end - m_lastBroadcastRankTime;

		if (rankDiff.total_milliseconds() > 500) {					//  500ms更新一次
			BroadcastRankingList();
			m_lastBroadcastRankTime = boost::posix_time::microsec_clock::local_time();

			diff = end - start;
			DEBUGLOG("roomId=[0x" << hex << m_pGameRoom->GetRoomId() << dec << "] time cost=[" << diff.total_milliseconds() << "], snake count=[" << m_snakeMap.size() << "]");
		}
		
		// 做机器人测试的时候才有用
		if (m_snakeMap.size() < 30 && g_slitherConfig.m_uRobotNum > 0) {
			for (int i = 1; i < 9; ++i) {
				Snake * pSnake = GenSnake(true);
				pSnake->SetTeamId(i);
				pSnake->SetSkinId(i);
			}
		}
	}

	void TeamScene::InitSceneFoods() {

		// 创建N个食物
		GenFoods(g_slitherConfig.m_uInitFoodNum);

		// 创建N条蛇
		uint32_t uTeamNum = g_slitherConfig.m_uRobotNum / 8;
		uint32_t uTeamId = 1;
		for (uint32_t i = 0; i < g_slitherConfig.m_uRobotNum; ++i) {
			Snake* pSnake = GenSnake(true);
			JoinTeam(pSnake, uTeamId);
			pSnake->SetTeamId(uTeamId);
			pSnake->SetSkinId(uTeamId);
			if (i % uTeamNum == 0) {
				++uTeamId;
			}
		}
	}

	// 让蛇加入某支队伍
	void TeamScene::JoinTeam(Snake* pSnake, uint32_t uTeamId) {
		if (!pSnake) {
			return;
		}

		pSnake->SetTeamId(uTeamId);
	}

	void TeamScene::CheckCollide(Snake* pSnake, uint16_t uGridIndex) {
		if (!pSnake || pSnake->GetStatus() == OBJ_DESTROY) {
			return;
		}

		// 判断和其他蛇的碰撞
		SnakeSet& snakeSet = GetSnakeSet(uGridIndex);									// 能看见此格子的蛇的列表

		ObjectGrids objGrids = GetObjectGrids(pSnake->GetSnakeHead());
		for (int i = 0; i < MAX_GRID_IN; ++i) {
			int32_t uIndex = objGrids.grids[i];
			if (uIndex < 0) {
				continue;
			}

			// 然后判断蛇和其他蛇的碰撞
			const map<Snake*, set<Object*> >& gridSnakeMap = m_pGrids[uIndex].GetSnakeList();					// 获取当前格子的其他蛇

			map<Snake*, set<Object*> >::const_iterator collideIt = gridSnakeMap.begin();
			map<Snake*, set<Object*> >::const_iterator collideItEnd = gridSnakeMap.end();
			for (; collideIt != collideItEnd; collideIt++) {
				Snake* pTmpSnake = collideIt->first;
				if (!pTmpSnake || pTmpSnake == pSnake || pTmpSnake->GetStatus() == OBJ_DESTROY) {				// 如果是自己, 或者死亡了就不做判断了
					continue;
				}

				// 如果是同一个队伍的，也不做判断
				if (pTmpSnake->GetTeamId() == pSnake->GetTeamId()) {
					continue;
				}

				// 如果蛇处于无敌状态，撞到其他蛇不管
				if (pSnake->GetSnakeBuffStat().HasBuff(SNAKE_INVINCIBLE)) {
					continue;
				}

				set<Object*>::iterator objIt = (collideIt->second).begin();
				set<Object*>::iterator objItEnd = (collideIt->second).end();
				for (; objIt != objItEnd; objIt++) {
					SnakeBodyNode* pTmpObj = (SnakeBodyNode*)*objIt;
					if (!pTmpObj) {
						continue;
					}

					// 如果发生了碰撞，那么将此蛇销毁掉，然后身体分解成食物
					if (pSnake->GetSnakeHead()->IsCollide(pTmpObj)) {

						pTmpObj->GetOwner()->IncKillTimes();								// 增加被碰撞的蛇的击杀次数
						pSnake->SetStatus(OBJ_DESTROY);										// 将状态设置为“销毁”
						pSnake->IncDeadTimes();

						// 向周围广播碰撞
						BroadcastCollide(snakeSet, pSnake);

						// 将蛇分解掉
						BreakUpSnake(pSnake);
						DEBUGLOG("snake id=" << pSnake->GetSnakeId() << " addr=" << pSnake << " collide in grid=" << uIndex);
						return;																// 不用再做其他碰撞测试了
					}
				}
			}
		}
	}
}