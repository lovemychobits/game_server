#ifndef GAMESERVER_SLITHER_FACTORY_H
#define GAMESERVER_SLITHER_FACTORY_H

#include "Food.h"
#include "Snake.h"

namespace slither {
	class Factory {
	public:
		Factory();
		~Factory();
		static Factory* GetInstance() {
			static Factory instance;
			return &instance;
		}

		Snake* CreateSnake(Scene* pScene, uint32_t uSnakeId, const Vector2D& pos, uint32_t uBodySize, bool bRobot=false);
		void ReleaseSnake(Snake* pSnake);

		Food* CreateFood(uint32_t uFoodId, const Vector2D& pos, uint32_t uValue);
		void ReleaseFood(Food* pFood);
	};

#define gpFactory Factory::GetInstance()
}

#endif