#ifndef GAMESERVER_PLANESHOOTING_SCENEMNG_H
#define GAMESERVER_PLANESHOOTING_SCENEMNG_H

#include "Plane.h"
#include <list>
#include <map>
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

		void OnTimer(const boost::system::error_code&);

		void TestQuadTree();

	public:
		void PlayerEnter(IConnection* pConn);
		void PlaneMove(Plane* pPlane, uint32_t uAction);
		void PlaneShoot(Plane* pPlane);
		Plane* GetPlaneByConn(IConnection* pConn);

	private:
		void AddPlane(IConnection* pConn, uint16_t uPlaneId, Vector2D pos, int8_t nOrientation);
		void Broadcast(Object* pObject);											// ¹ã²¥

	private:
		list<Plane*> m_planeList;
		list<Bullet*> m_bulletList;
		map<IConnection*, Plane*> m_connToPlaneMap;
		QuadTree* m_pQuadTree;
		uint32_t m_uPlaneId;
	};
}
#endif