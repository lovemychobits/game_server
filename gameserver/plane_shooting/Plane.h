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
		// 行为，操作
		void Left(uint32_t uLeftTimes = 1);					// 左移
		void Right(uint32_t uRightTimes = 1);				// 右移
		void Up(uint32_t uUpTimes = 1);						// 上移
		void Down(uint32_t uDownTimes = 1);					// 下移
		void TurnAround();									// 掉头
		void SendMsg(const char* pData, uint32_t uLen);

	public:
		void SetSpeed(float fSpeed) {
			m_curSpeed = fSpeed;
		}

		float GetSpeed() {
			return m_curSpeed;
		}

		int8_t GetOrientation() {
			return m_orientation;
		}

		void SetAngle(uint16_t uAngle) {
			m_uAngle = uAngle;
		}

		uint16_t GetAngle() {
			return m_uAngle;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

		bool NeedNotify(Object* pObj);					// 是否需要通知

	private:
		bool IsInViewRange(const Rectangle& rect);		// 是否在视野内

	private:
		IConnection* m_pConn;
		int8_t m_orientation;							// 朝向，1为朝上，-1为朝下
		float m_curSpeed;								// 当前速度
		uint16_t m_uAngle;								// 角度
		ObjectStatus m_status;							// 飞机当前状态
		PlaneViewRange m_viewRange;						// 视野范围
	};
}

#endif