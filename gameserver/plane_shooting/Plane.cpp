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

	Plane::Plane(uint16_t uPlaneId, const Rectangle& rect, int8_t orientation, IConnection* pConn) :
		Object(uPlaneId, rect, Plane_Type), m_pConn(pConn), m_orientation(orientation), m_curSpeed(1) {

		// ����Ĭ�ϵ���Ұ��Χ���Էɻ���������ĵ�
		m_viewRange.fWidth = 10;
		m_viewRange.fHeight = 15;
	}

	bool Plane::NeedNotify(Object* pObj) {
		if (pObj == NULL || pObj == this) {				// �ն�����Լ�������֪ͨ
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

		float x1, y1, x2, y2, x3, y3, x4, y4;
		x3 = x1 = m_rect.x - m_viewRange.fWidth / 2;
		x4 = x2 = m_rect.x + m_viewRange.fWidth / 2;

		y2 = y1 = m_rect.y + m_viewRange.fHeight / 2;
		y4 = y3 = m_rect.y - m_viewRange.fHeight / 2;

		if (rect.x < x1 || rect.x > x2) {
			return false;
		}
		if (rect.y > y1 || rect.y < y3) {
			return false;
		}

		return true;
	}

	void Plane::SendMsg(const char* pData, uint32_t uLen) {
		if (m_pConn && m_pConn->IsConnected()) {
			m_pConn->SendMsg(pData, uLen);
		}
	}
}
