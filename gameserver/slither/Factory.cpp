#include "Factory.h"
#include "../config/SlitherConfig.h"

namespace slither {
	Factory::Factory() {

	}

	Factory::~Factory() {

	}

	Snake* Factory::CreateSnake(Scene* pScene, uint32_t uSnakeId, const Vector2D& pos, uint32_t uBodySize, float fRadius, bool bRobot) {
		Snake* pSnake = new Snake(pScene, uSnakeId, fRadius, pos, uBodySize, bRobot);
		return pSnake;
	}

	Snake* Factory::CreateSnake(Scene* pScene, uint32_t uSnakeId, Vector2D& pos) {
		Snake* pSnake = new Snake(pScene, uSnakeId, g_slitherConfig.m_fInitRadius, pos, g_slitherConfig.m_uInitSnakeBodySize, false);
		return pSnake;
	}

	void Factory::ReleaseSnake(Snake* pSnake) {
		if (!pSnake) {
			return;
		}

		delete pSnake;
	}

	Food* Factory::CreateFood(uint32_t uFoodId, const Vector2D& pos, uint32_t uValue) {
		Food* pFood = new Food(uFoodId, pos, 0.01f, uValue);
		return pFood;
	}

	void Factory::ReleaseFood(Food* pFood) {
		if (!pFood) {
			return;
		}

		delete pFood;
	}

	Buff* Factory::CreateBuff(BuffType buffType) {
		Buff* pBuff = NULL;
		switch (buffType) {
		case slither::SNAKE_INVINCIBLE:
			pBuff = new SnakeInvincibleBuff();
			break;
		case slither::SNAKE_MULTI_POWER:
			pBuff = new SnakeMultiPowerBuff();
			break;
		default:
			break;
		}

		return pBuff;
	}

	void Factory::ReleaseBuff(Buff* pBuff) {
		if (!pBuff) {
			return;
		}

		delete pBuff;
	}
}