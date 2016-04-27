#ifndef GAMESERVER_PLANESHOOTING_OBJECT_H
#define GAMESERVER_PLANESHOOTING_OBJECT_H

#include <stdint.h>

namespace plane_shooting {
	struct Vector2D {
		uint16_t x;
		uint16_t y;

		Vector2D() : x(0), y(0) {
		}
	};

	class Object
	{
	public:
		Object() {
		}
		Object(uint16_t uObjId, Vector2D& pos) : m_uObjId(uObjId), m_pos(pos) {
		}
		~Object() {
		}

	public:
		uint16_t GetObjId() {
			return m_uObjId;
		}

		Vector2D& GetPos() {
			return m_pos;
		}

	protected:
		uint16_t m_uObjId;						// ŒÔÃÂID
		Vector2D m_pos;
	};
}

#endif