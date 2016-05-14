#ifndef GAMESERVER_SLITHER_SLITHERMATH_H
#define GAMESERVER_SLITHER_SLITHERMATH_H

#include <math.h>
#include "Object.h"

namespace slither {
	class SlitherMath {
	public:
		static float PosToAnglePI(const Vector2D& pos);
		static float GetLegalAngle(float fTurnAngle, float fFaceAngle);
		static int32_t Sign(float fValue);
		static Vector2D MoveTo(Vector2D& pos, float fLength);
		static Vector2D MoveTo(float fAngle, float fLength);
		static Vector2D MoveToAngle(Vector2D& pos, float fAngle, float fSpeed);
	};
}


#endif