#ifndef GAMESERVER_PLANESHOOTING_SCENEMNG_H
#define GAMESERVER_PLANESHOOTING_SCENEMNG_H

#include "Plane.h"
#include <list>
#include <map>
using namespace std;
#include "../../network/header.h"
#include "QuadTree.h"
#include "FixedQuadTree.h"

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

	public:
		void PlayerEnter(IConnection* pConn);
		void PlaneMove(Plane* pPlane, Vector2D newPos, uint32_t uFireAngle, uint32_t uFlyAngle);
		void PlaneShoot(Plane* pPlane);
		Plane* GetPlaneByConn(IConnection* pConn);
		void PlaneDisconnect(IConnection* pConn); 

	private:
		Plane* AddPlane(IConnection* pConn, uint16_t uPlaneId, Vector2D pos, int8_t nOrientation);
		void Broadcast(Object* pObject);											// �㲥
		void NotifyOthers(Plane* pSelf);
		bool CheckCollision(Vector2D pos, Object* pObject);
		bool CheckIntersect(Vector2D oldPos, Vector2D newPos, Rectangle rect);		// �ж��߶��Ƿ�������ཻ
		bool CheckIntersect(Rectangle rect1, Rectangle rect2);						// �ж����������Ƿ��ཻ
		void BroadcastCollision(Object* pObject, Object* pTarget);					// �㲥��������ײ
		void TestCheckIntersect();

	private:
		list<Plane*> m_planeList;
		list<Bullet*> m_bulletList;
		map<IConnection*, Plane*> m_connToPlaneMap;
		FixedQuadTree* m_pFixedQuadTree;
		uint32_t m_uPlaneId;
	};
}
#endif