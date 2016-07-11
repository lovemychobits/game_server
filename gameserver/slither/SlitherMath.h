﻿#ifndef GAMESERVER_SLITHER_SLITHERMATH_H
#define GAMESERVER_SLITHER_SLITHERMATH_H

#include <math.h>
#include "Object.h"

namespace slither {
#define PI (3.14159f)

	class SlitherMath {
	public:
		static float PosToAnglePI(const Vector2D& pos);
		static float GetLegalAngle(float fTurnAngle, float fFaceAngle);
		static int32_t Sign(float fValue);
		static Vector2D MoveTo(Vector2D& pos, float fLength);
		static Vector2D MoveTo(float fAngle, float fLength);
		static Vector2D MoveToAngle(const Vector2D& oldPos, float fAngle, float fSpeed);
	};

	#define slither_max(a,b)    (((a) > (b)) ? (a) : (b))
	#define slither_min(a,b)    (((a) < (b)) ? (a) : (b))
}


#endif