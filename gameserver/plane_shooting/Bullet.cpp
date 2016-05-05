#include "Bullet.h"
#include <math.h>

namespace plane_shooting {
	Bullet::Bullet() {

	}

	Bullet::Bullet(uint16_t uBulletId, Plane* pOwner, const Rectangle& rect, uint16_t uAngle, float fSpeed) :
		Object(uBulletId, rect, Bullet_Type), m_pOwner(pOwner), m_uAngle(uAngle), m_fSpeed(fSpeed), m_bDestroy(false) {

		m_orientation = pOwner->GetOrientation();
	}

	Bullet::~Bullet() {

	}

	void Bullet::fly() {
		if (m_fDistance >= MAX_DISTANCE) {
			m_bDestroy = true;
			return;
		}
		
		uint16_t uAngle = GetSinAngle(m_uAngle);
		float x_len = GetXLen(m_fSpeed, uAngle);
		float y_len = GetYLen(m_fSpeed, uAngle);

		if (m_uAngle <= 90) {
			x_len = -x_len;
		}
		else if (m_uAngle <= 180) {
			x_len = -x_len;
			y_len = -y_len;
		}
		else if (m_uAngle <= 270) {
			y_len = -y_len;
		}
		
		m_rect.x += x_len;
		m_rect.y += y_len;

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

#define PI 3.14159

	float Bullet::GetXLen(float vectLen, uint16_t uSinAngle) {
		float fAngle = float(uSinAngle) * PI / 180;
		float sin_value = (float)::sin(fAngle);
		return vectLen * sin_value;
	}

	float Bullet::GetYLen(float vectLen, uint16_t uSinAngle) {
		float fAngle = float(uSinAngle) * PI / 180;
		float cos_value = (float)::cos(fAngle);
		return vectLen * cos_value;
	}
}