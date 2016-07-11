#ifndef GAMESERVER_SLITHER_FOOD_H
#define GAMESERVER_SLITHER_FOOD_H

#include "Object.h"
#include "../../protocol/slither_battle.pb.h"
#include "Buff.h"

namespace slither {
	class Food : public Object {
	public:
		Food(uint32_t uFoodId, const Vector2D& pos, float fRadius, uint32_t uValue, BuffType m_buffType = INVALID_BUFF) : 
			Object(pos, fRadius, Food_Type), m_uFoodId(uFoodId), m_uMass(uValue), m_buffType(INVALID_BUFF) {

		}
		~Food() {

		}

		void SerializeToPB(slither::PBFood& pbFood) {
			pbFood.set_foodid(m_uFoodId);
			pbFood.mutable_pos()->set_x(GetPos().x);
			pbFood.mutable_pos()->set_y(GetPos().y);
			pbFood.set_mass(m_uMass);
		}

		uint32_t GetFoodId() {
			return m_uFoodId;
		}

		uint32_t GetMass() {
			return m_uMass;
		}

		BuffType GetBuffType() {
			return m_buffType;
		}

	private:
		uint32_t m_uFoodId;					// 食物ID
		uint32_t m_uMass;					// food对应的值
		BuffType m_buffType;
	};
}

#endif