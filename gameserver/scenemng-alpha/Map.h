#pragma once

#include "../header.h"

namespace scene_alpha {
#define MAP_GRID_SIZE 100
	class Map
	{
	public:
		Map();
		~Map();

	private:
		uint8_t scene[MAP_GRID_SIZE][MAP_GRID_SIZE];
	};
}