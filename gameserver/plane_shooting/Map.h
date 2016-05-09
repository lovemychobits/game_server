#ifndef GAMESERVER_PLANESHOOTING_MAP_H
#define GAMESERVER_PLANESHOOTING_MAP_H

#include <stdint.h>

namespace plane_shooting {
#define MAP_GRID_SIZE 200
	class Map
	{
	public:
		Map() {}
		~Map() {}

	private:
		uint8_t m_map[MAP_GRID_SIZE][MAP_GRID_SIZE];
	};
}

#endif