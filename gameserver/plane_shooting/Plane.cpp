#include "Plane.h"
#include "Map.h"
#include <string.h>

namespace plane_shooting {
	/*  Y
		^
		|
		|
		|
		|__________> X
	*/

	Plane::Plane() : m_orientation(1), m_curSpeed(1), m_pConn(NULL) {

	}

	Plane::Plane(uint16_t uPlaneId, Vector2D& pos, int8_t orientation, IConnection* pConn) : 
		Object(uPlaneId, pos), m_pConn(pConn), m_orientation(orientation), m_curSpeed(1) {
		m_viewRange.uWidth = 10;
		m_viewRange.uHeight = 15;
	}

	void Plane::Left(uint32_t uLeftTimes) {
		if (m_pos.x > 0) {
			m_pos.x -= MOVE_STEP * m_curSpeed;
		}
	}

	void Plane::Right(uint32_t uRightTimes) {
		if (m_pos.x < MAP_GRID_SIZE) {
			m_pos.x += MOVE_STEP * m_curSpeed;
		}
	}

	void Plane::Up(uint32_t uUpTimes) {
		if (m_pos.y < MAP_GRID_SIZE) {
			m_pos.y += MOVE_STEP * m_curSpeed;
		}
	}

	void Plane::Down(uint32_t uDownTimes) {
		if (m_pos.y > 0) {
			m_pos.y -= MOVE_STEP * m_curSpeed;
		}
	}

	void Plane::TurnAround() {
		m_orientation = -m_orientation;
	}

	bool Plane::NeedNotify(Object* pObj) {
		if (pObj == NULL || pObj == this) {				// 空对象和自己本身不用通知
			return false;
		}

		if (IsInViewRange(pObj->GetPos())) {
			return true;
		}

		return false;
	}

	bool Plane::IsInViewRange(Vector2D& pos) {
		/*
		x1y1-----------x2y2
		|				|
		|		O		|
		|	  (x,y)		|
		x3y3-----------x4y4
		*/

		uint16_t x1, y1, x2, y2, x3, y3, x4, y4;
		x3 = x1 = m_pos.x - m_viewRange.uWidth / 2;
		x4 = x2 = m_pos.x + m_viewRange.uWidth / 2;

		y2 = y1 = m_pos.y + m_viewRange.uHeight / 2;
		y4 = y3 = m_pos.y - m_viewRange.uHeight / 2;

		if (pos.x < x1 || pos.x > x2) {
			return false;
		}
		if (pos.y > y1 || pos.y < y3) {
			return false;
		}

		return true;
	}
}

