#ifndef GAMESERVER_PLANESHOOTING_BULLET_H
#define GAMESERVER_PLANESHOOTING_BULLET_H

#include "Plane.h"
#include "Object.h"

namespace plane_shooting {

// 子弹最大飞行距离
#define MAX_DISTANCE 30	

	class Bullet : public Object
	{
	public:
		Bullet();
		Bullet(uint16_t uBulletId, Plane* pOwner, const Rectangle& rect, uint16_t uAngle, float fSpeed);
		~Bullet();

	public:
		void fly();								// 子弹飞行，完全由服务器来模拟
		bool CanDestroy() {
			return m_bDestroy;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
			if (status == DEAD)
			{
				m_bDestroy = true;
			}
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

	private:
		uint16_t GetSinAngle(uint16_t uAngle);	// 根据unity坐标系中的角度，计算出对于的直角坐标系中斜边sin对应的角度
		float GetXLen(float vectLen, uint16_t uSinAngle);
		float GetYLen(float vectLen, uint16_t uSinAngle);

	private:
		Plane* m_pOwner;						// 属于哪个飞机
		uint16_t m_uAngle;						// 角度（0~360）（在unity坐标系中，是逆时针方向的转角）
		float m_fSpeed;							// 速度
		int8_t m_orientation;					// 朝向
		float m_fDistance;						// 已经飞行的距离
		bool m_bDestroy;						// 销毁
		ObjectStatus m_status;					// 状态
	};

}

#endif