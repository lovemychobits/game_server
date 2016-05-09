#include "Bullet.h"
#include <math.h>

namespace plane_shooting {
#define PI 3.14159

	Bullet::Bullet() {

	}

	Bullet::Bullet(uint16_t uBulletId, Plane* pOwner, const Rectangle& rect, uint16_t uAngle, float fSpeed) :
		Object(uBulletId, rect, Bullet_Type), m_pOwner(pOwner), m_uAngle(uAngle), m_fSpeed(fSpeed), m_bDestroy(false) {
	}

	Bullet::~Bullet() {
	}

	void Bullet::Fly() {
		if (m_fDistance >= MAX_DISTANCE) {
			m_bDestroy = true;						// 已经超出最大距离了，不需要再模拟飞行，直接设置为可销毁
			return;
		}
		
		// 获取射线与Y轴的夹角
		uint16_t uAngle = GetSinAngle(m_uAngle);
		float x_len = GetXLen(m_fSpeed, uAngle);
		float y_len = GetYLen(m_fSpeed, uAngle);

		// 根据飞行方向，重新计算子弹坐标点，处理四个象限的不同情况，第一象限不需要特殊处理
		if (m_uAngle <= 90) {				// 第二象限
			x_len = -x_len;
		}
		else if (m_uAngle <= 180) {			// 第三象限
			x_len = -x_len;
			y_len = -y_len;
		}
		else if (m_uAngle <= 270) {			// 第四象限
			y_len = -y_len;
		}
		
		m_rect.x += x_len;
		m_rect.y += y_len;

		// 如果超出我们规定的坐标范围
		if (m_rect.x <= 0) {
			m_rect.x = 0;
			m_bDestroy = true;
		}

		if (m_rect.y <= 0) {
			m_rect.y = 0;
			m_bDestroy = true;
		}

		return;
	}

	uint16_t Bullet::GetSinAngle(uint16_t uAngle) {
		// 为了算出所在象限中飞行射线与Y轴的夹角
		if (uAngle <= 90) {
			return uAngle;
		}
		else if (uAngle <= 180) {
			return 180 - uAngle;
		}
		else if (uAngle <= 270) {
			return uAngle - 180;
		}
		else if (uAngle <= 360) {
			return 360 - uAngle;
		}

		// 其他情况理论上都是错误的
		return 0;
	}

	float Bullet::GetXLen(float vectLen, uint16_t uSinAngle) {
		float fAngle = float(uSinAngle * PI / 180);
		float sin_value = (float)::sin(fAngle);
		return vectLen * sin_value;
	}

	float Bullet::GetYLen(float vectLen, uint16_t uSinAngle) {
		float fAngle = float(uSinAngle * PI / 180);
		float cos_value = (float)::cos(fAngle);
		return vectLen * cos_value;
	}
}