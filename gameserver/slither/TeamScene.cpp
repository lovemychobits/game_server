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
		boost::posix_time::time_duration diff;											// ͳ��ÿ��tick�ĺ�ʱ

		// ģ�������ߵ���Ϊ
		SimulateSnakes();

		end = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::time_duration rankDiff = end - m_lastBroadcastRankTime;

		if (rankDiff.total_milliseconds() > 500) {					//  500ms����һ��
			BroadcastRankingList();
			m_lastBroadcastRankTime = boost::posix_time::microsec_clock::local_time();

			diff = end - start;
			DEBUGLOG("roomId=[0x" << hex << m_pGameRoom->GetRoomId() << dec << "] time cost=[" << diff.total_milliseconds() << "], snake count=[" << m_snakeMap.size() << "]");
		}
		
		// �������˲��Ե�ʱ�������
		if (m_snakeMap.size() < 30 && g_slitherConfig.m_uRobotNum > 0) {
			for (int i = 1; i < 9; ++i) {
				Snake * pSnake = GenSnake(true);
				pSnake->SetTeamId(i);
				pSnake->SetSkinId(i);
			}
		}
	}

	void TeamScene::InitSceneFoods() {

		// ����N��ʳ��
		GenFoods(g_slitherConfig.m_uInitFoodNum);

		// ����N����
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

	// ���߼���ĳ֧����
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

		// �жϺ������ߵ���ײ
		SnakeSet& snakeSet = GetSnakeSet(uGridIndex);									// �ܿ����˸��ӵ��ߵ��б�

		ObjectGrids objGrids = GetObjectGrids(pSnake->GetSnakeHead());
		for (int i = 0; i < MAX_GRID_IN; ++i) {
			int32_t uIndex = objGrids.grids[i];
			if (uIndex < 0) {
				continue;
			}

			// Ȼ���ж��ߺ������ߵ���ײ
			const map<Snake*, set<Object*> >& gridSnakeMap = m_pGrids[uIndex].GetSnakeList();					// ��ȡ��ǰ���ӵ�������

			map<Snake*, set<Object*> >::const_iterator collideIt = gridSnakeMap.begin();
			map<Snake*, set<Object*> >::const_iterator collideItEnd = gridSnakeMap.end();
			for (; collideIt != collideItEnd; collideIt++) {
				Snake* pTmpSnake = collideIt->first;
				if (!pTmpSnake || pTmpSnake == pSnake || pTmpSnake->GetStatus() == OBJ_DESTROY) {				// ������Լ�, ���������˾Ͳ����ж���
					continue;
				}

				// �����ͬһ������ģ�Ҳ�����ж�
				if (pTmpSnake->GetTeamId() == pSnake->GetTeamId()) {
					continue;
				}

				// ����ߴ����޵�״̬��ײ�������߲���
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

					// �����������ײ����ô���������ٵ���Ȼ������ֽ��ʳ��
					if (pSnake->GetSnakeHead()->IsCollide(pTmpObj)) {

						pTmpObj->GetOwner()->IncKillTimes();								// ���ӱ���ײ���ߵĻ�ɱ����
						pSnake->SetStatus(OBJ_DESTROY);										// ��״̬����Ϊ�����١�
						pSnake->IncDeadTimes();

						// ����Χ�㲥��ײ
						BroadcastCollide(snakeSet, pSnake);

						// ���߷ֽ��
						BreakUpSnake(pSnake);
						DEBUGLOG("snake id=" << pSnake->GetSnakeId() << " addr=" << pSnake << " collide in grid=" << uIndex);
						return;																// ��������������ײ������
					}
				}
			}
		}
	}
}