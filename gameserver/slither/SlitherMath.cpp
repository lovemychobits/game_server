#include "SlitherMath.h"
#include "../config/SlitherConfig.h"

namespace slither {
#define MAX_ROTATION (PI * 1.5f)

	float SlitherMath::PosToAnglePI(const Vector2D& pos) {
		float fAnglePI = fmod((2 * PI) + atan2(pos.y, pos.x), 2 * PI);
		return fAnglePI;
	}

	int32_t SlitherMath::Sign(float fValue) {
		if (fValue > 0.00001f) {
			return 1;
		}
		else if (fValue < 0.00001f) {
			return -1;
		}
		else {
			return 0;
		}
	}

	float SlitherMath::GetLegalAngle(float fTurnAngle, float fFaceAngle) {
		float fAngle = fTurnAngle - fFaceAngle;

		if (fabs(fAngle) >= PI) {
			if (fAngle > 0)
				fAngle = fAngle - 2 * PI;
			else if (fAngle < 0)
				fAngle = 2 * PI + fAngle;
		}

		float fDetalAngel = MAX_ROTATION * 0.1f;

		if (fabs(fAngle) > fDetalAngel) {
			return fmod((fFaceAngle + Sign(fAngle) * fDetalAngel), (2 * PI));
		}

		return fmod(fTurnAngle, 2 * PI);
	}

	Vector2D SlitherMath::MoveTo(Vector2D& pos, float fLength) {
		return MoveTo(PosToAnglePI(pos), fLength);
	}

	Vector2D SlitherMath::MoveTo(float fAngle, float fLength) {
		Vector2D move(fLength * ::cos(fAngle), fLength * ::sin(fAngle));
		return move;
	}

	Vector2D SlitherMath::MoveToAngle(Vector2D& pos, float fAngle, float fSpeed) {
		float x = fSpeed * ::cos(fAngle);
		float y = fSpeed * ::sin(fAngle);

		pos.x += x;
		pos.y += y;

		if (pos.x < 0.0f) {
			pos.x = 0.0f;
		}

		if (pos.y < 0.0f) {
			pos.y = 0.0f;
		}

		if (pos.x > slither::gpSlitherConf->m_uMapLength) {
			pos.x = (float)slither::gpSlitherConf->m_uMapLength;
		}

		if (pos.y > slither::gpSlitherConf->m_uMapLength) {
			pos.y = (float)slither::gpSlitherConf->m_uMapLength;
		}

		return pos;
	}
}