#include "Factory.h"

namespace slither {
	Factory::Factory() {

	}

	Factory::~Factory() {

	}

	Snake* Factory::CreateSnake(Scene* pScene, uint32_t uSnakeId, const Vector2D& pos, uint32_t uBodySize, bool bRobot) {
		Snake* pSnake = new Snake(pScene, uSnakeId, 1.0f, pos, uBodySize, bRobot);
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
}