#pragma once

#include "../header.h"

namespace scene_alpha {
	class Map
	{
	public:
		Map();
		~Map();

	private:
		uint8_t scene[20][20];						// 20 * 20
	};
}