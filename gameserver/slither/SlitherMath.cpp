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

	Vector2D SlitherMath::MoveToAngle(const Vector2D& oldPos, float fAngle, float fSpeed) {
		float x = fSpeed * ::cos(fAngle);
		float y = fSpeed * ::sin(fAngle);

		Vector2D newPos = oldPos;

		newPos.x += x;
		newPos.y += y;

		if (newPos.x < 0.0f) {
			newPos.x = 0.0f;
		}

		if (newPos.y < 0.0f) {
			newPos.y = 0.0f;
		}

		if (newPos.x > slither::gpSlitherConf->m_uMapLength) {
			newPos.x = (float)slither::gpSlitherConf->m_uMapLength;
		}

		if (newPos.y > slither::gpSlitherConf->m_uMapLength) {
			newPos.y = (float)slither::gpSlitherConf->m_uMapLength;
		}

		return newPos;
	}
}