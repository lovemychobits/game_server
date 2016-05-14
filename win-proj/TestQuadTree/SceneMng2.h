#ifndef GAMESERVER_PLANESHOOTING_SCENEMNG_H
#define GAMESERVER_PLANESHOOTING_SCENEMNG_H

#include "Plane.h"
#include <list>
using namespace std;
#include "../../network/header.h"
#include "QuadTree.h"

namespace plane_shooting {
	class Bullet;
	class SceneMng
	{
	public:
		SceneMng();
		~SceneMng();
		static SceneMng* GetInstance() {
			static SceneMng instance;
			return &instance;
		}

		void OnTimer();

		void TestQuadTree();

	public:
		void AddPlane(uint16_t uPlaneId, Vector2D pos, int8_t nOrientation);
		void PlaneMove(Plane* pPlane, uint32_t uAction);
		void PlaneShoot(Plane* pPlane);

	private:
		void Broadcast(Object* pObject);											// ¹ã²¥

	private:
		list<Plane*> m_planeList;
		list<Bullet*> m_bulletList;
		QuadTree* m_pQuadTree;
	};
}
#endif