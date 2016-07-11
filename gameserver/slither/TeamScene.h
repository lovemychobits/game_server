#ifndef GAMESERVER_TEAMSCENE_H
#define GAMESERVER_TEAMSCENE_H

#include "Scene.h"

namespace slither {

	class GameRoom;

	typedef set<uint64_t> SnakeTeam;

	class TeamScene : public Scene {
	public:
		TeamScene(GameRoom* pGameRoom);
		virtual ~TeamScene();

		virtual void OnTimer();

		virtual void InitSceneFoods();

	protected:
		void JoinTeam(Snake* pSnake, uint32_t uTeamId);										// 加入到某支队伍中
		virtual void CheckCollide(Snake* pSnake, uint16_t uGridIndex);						// 在指定格子内判定碰撞

	private:
		map<uint32_t, SnakeTeam> m_snakeTeamMap;
	};
}


#endif