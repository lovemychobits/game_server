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

	Plane::Plane(uint16_t uPlaneId, const Rectangle& rect, int8_t orientation) :
		Object(uPlaneId, rect), m_orientation(orientation), m_curSpeed(1) {

		// 设置默认的视野范围，以飞机自身的中心点
		m_viewRange.uWidth = 10;
		m_viewRange.uHeight = 15;
	}

	void Plane::Left(uint32_t uLeftTimes) {
		if (m_rect.x > 0) {
			m_rect.x -= MOVE_STEP * m_curSpeed;
		}
	}

	void Plane::Right(uint32_t uRightTimes) {
		if (m_rect.x < MAP_GRID_SIZE) {
			m_rect.x += MOVE_STEP * m_curSpeed;
		}
	}

	void Plane::Up(uint32_t uUpTimes) {
		if (m_rect.y < MAP_GRID_SIZE) {
			m_rect.y += MOVE_STEP * m_curSpeed;
		}
	}

	void Plane::Down(uint32_t uDownTimes) {
		if (m_rect.y > 0) {
			m_rect.y -= MOVE_STEP * m_curSpeed;
		}
	}

	void Plane::TurnAround() {
		m_orientation = -m_orientation;
	}

	bool Plane::NeedNotify(Object* pObj) {
		if (pObj == NULL || pObj == this) {				// 空对象和自己本身不用通知
			return false;
		}

		if (IsInViewRange(pObj->GetRect())) {
			return true;
		}

		return false;
	}

	bool Plane::IsInViewRange(const Rectangle& rect) {
		/*
		x1y1-----------x2y2
		|				|
		|		O		|
		|	  (x,y)		|
		x3y3-----------x4y4
		*/

		uint16_t x1, y1, x2, y2, x3, y3, x4, y4;
		x3 = x1 = m_rect.x - m_viewRange.uWidth / 2;
		x4 = x2 = m_rect.x + m_viewRange.uWidth / 2;

		y2 = y1 = m_rect.y + m_viewRange.uHeight / 2;
		y4 = y3 = m_rect.y - m_viewRange.uHeight / 2;

		if (rect.x < x1 || rect.x > x2) {
			return false;
		}
		if (rect.y > y1 || rect.y < y3) {
			return false;
		}

		return true;
	}
}

