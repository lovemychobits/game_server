#ifndef GAMESERVER_PLANESHOOTING_PLANE_H
#define GAMESERVER_PLANESHOOTING_PLANE_H

#include <stdint.h>
#include "Object.h"
#include "../../network/IConnection.h"
using namespace cpnet;

namespace plane_shooting {

#define MOVE_STEP 1

	// 飞机视野范围，也就是如果有其他飞机，或者子弹进入这个范围了，就需要通知飞机
	struct PlaneViewRange
	{
		float fWidth;			// 长度
		float fHeight;			// 宽度
	};

	class Plane : public Object
	{
	public:
		Plane();
		Plane(uint16_t uPlaneId, const Rectangle& rect, int8_t orientation, IConnection* pConn);
		~Plane();

	public:
		// 发送消息
		void SendMsg(const char* pData, uint32_t uLen);

	public:
		// 设置属性
		void SetSpeed(float fSpeed) {
			m_curSpeed = fSpeed;
		}

		float GetSpeed() {
			return m_curSpeed;
		}

		int8_t GetOrientation() {
			return m_orientation;
		}

		void SetFireAngle(uint16_t uAngle) {
			m_uFireAngle = uAngle;
		}

		uint16_t GetFireAngle() {
			return m_uFireAngle;
		}

		void SetFlyAngle(uint16_t uAngle) {
			m_uFlyAngle = uAngle;
		}

		uint16_t GetFlyAngle() {
			return m_uFlyAngle;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

		Rectangle GetViewRect() {
			Rectangle viewRect(m_rect.x, m_rect.y, m_viewRange.fWidth, m_viewRange.fHeight);
			return viewRect;
		}

		bool NeedNotify(Object* pObj);					// 是否需要通知

	private:
		bool IsInViewRange(const Rectangle& rect);		// 是否在视野内

	private:
		IConnection* m_pConn;
		int8_t m_orientation;							// 朝向，1为朝上，-1为朝下
		float m_curSpeed;								// 当前速度
		uint16_t m_uFireAngle;							// 射击角度
		uint16_t m_uFlyAngle;							// 飞行角度
		ObjectStatus m_status;							// 飞机当前状态
		PlaneViewRange m_viewRange;						// 视野范围
	};
}

#endif