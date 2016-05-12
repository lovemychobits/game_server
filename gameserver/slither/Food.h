#ifndef GAMESERVER_SLITHER_FOOD_H
#define GAMESERVER_SLITHER_FOOD_H

#include "Object.h"
#include "../../protocol/slither_client.pb.h"

namespace slither {
	class Food : public Object {
	public:
		Food(uint32_t uFoodId, const Vector2D& pos, float fRadius, uint32_t uValue) : 
			Object(pos, fRadius, Food_Type), m_uFoodId(uFoodId), m_uValue(uValue), m_status(ObjectStatus::OBJ_EXIST), m_uEatenBySnakeId(0) {

		}
		~Food() {

		}

		void SerializeToPB(slither::PBFood& pbFood) {
			pbFood.set_foodid(m_uFoodId);
			pbFood.mutable_pos()->set_x(GetPos().x);
			pbFood.mutable_pos()->set_y(GetPos().y);
			pbFood.set_value(m_uValue);

			if (m_status != OBJ_EXIST) {
				pbFood.set_status((slither::PBObjectStatus)m_status);
			}
			if (m_uEatenBySnakeId != 0) {
				pbFood.set_eatenbysnakeid(m_uEatenBySnakeId);
			}
		}

		uint32_t GetValue() {
			return m_uValue;
		}

		void SetStatus(ObjectStatus status) {
			m_status = status;
		}

		ObjectStatus GetStatus() {
			return m_status;
		}

		void SetEatenSnakeId(uint32_t uSnakeId) {
			m_uEatenBySnakeId = uSnakeId;
		}

		uint32_t GetEatenSnakeId() {
			return m_uEatenBySnakeId;
		}

	private:
		uint32_t m_uFoodId;					// 食物ID
		uint32_t m_uValue;					// food对应的值
		ObjectStatus m_status;				// 食物状态
		uint32_t m_uEatenBySnakeId;			// 被哪条蛇吃掉了
	};
}

#endif