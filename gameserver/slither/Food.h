#ifndef GAMESERVER_SLITHER_FOOD_H
#define GAMESERVER_SLITHER_FOOD_H

#include "Object.h"
#include "../../protocol/slither_client.pb.h"

namespace slither {
	class Food : public Object {
	public:
		Food(uint32_t uFoodId, const Vector2D& pos, float fRadius, uint32_t uValue) : 
			Object(pos, fRadius, Food_Type), m_uFoodId(uFoodId), m_uMass(uValue) {

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

	private:
		uint32_t m_uFoodId;					// 食物ID
		uint32_t m_uMass;					// food对应的值
	};
}

#endif