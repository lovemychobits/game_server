#include "SceneMng2.h"
#include "Bullet.h"
#include "../../utils/Utility.h"
#include <vector>
#include <iostream>
#include <windows.h>
using namespace std;

namespace plane_shooting {
	SceneMng::SceneMng() {
		Rectangle rect(250, 250, 500, 500);
		m_pQuadTree = new QuadTree(rect);
	}

	SceneMng::~SceneMng() {

	}

	void SceneMng::OnTimer() {

		DWORD dwStart = ::GetTickCount();

		for (int i = 0; i < 100; i++) {
			// 模拟子弹飞行，及广播
			list<Bullet*>::iterator bulletIt = m_bulletList.begin();
			list<Bullet*>::iterator bulletItEnd = m_bulletList.end();
			list<Object*> objList;
			for (; bulletIt != bulletItEnd; bulletIt++) {
				Bullet* pBullet = *bulletIt;
				if (!pBullet) {
					continue;
				}
				m_pQuadTree->retrieve(pBullet->GetRect(), objList);
				objList.clear();
			}
		}

		DWORD dwEnd = ::GetTickCount();
		cout << "time cost=[" << dwEnd - dwStart << "]" << endl;
	}

	void SceneMng::TestQuadTree() {
		Rectangle rect;
		rect.uWidth = 3;
		rect.uHeight = 3;

		// test insert
		vector<Plane*> planeList;
		for (int i = 0; i < 1000; i++)
		{
			rect.x = cputil::GenRandom(1, 500);
			rect.y = cputil::GenRandom(1, 500);

			Plane* pPlane = new Plane(i, rect, 1);
			planeList.push_back(pPlane);
			m_pQuadTree->insert(pPlane);
		}

		for (int i = 0; i < 100; i++) {
			Plane* pPlane = planeList.at(i / 20);
			Rectangle& rect = pPlane->GetRect();
			rect.uWidth = 0;
			rect.uHeight = 0;
			rect.x = cputil::GenRandom(1, 500);
			rect.y = cputil::GenRandom(1, 500);

			Bullet* pBullet = new Bullet(1, pPlane, pPlane->GetRect(), pPlane->GetOrientation(), 3);
			m_bulletList.push_back(pBullet);
		}
	}

	void SceneMng::AddPlane(uint16_t uPlaneId, Vector2D pos, int8_t nOrientation) {
		Rectangle rect;
		rect.x = pos.x;
		rect.y = pos.y;
		rect.uWidth = 3;
		rect.uHeight = 3;

		Plane* pPlane = new Plane(uPlaneId, rect, nOrientation);
		m_planeList.push_back(pPlane);

		Broadcast(pPlane);
	}

	void SceneMng::PlaneMove(Plane* pPlane, uint32_t uAction) {
		if (!pPlane) {
			return;
		}

		pPlane->Up();
		Broadcast(pPlane);
		return;
	}

	void SceneMng::PlaneShoot(Plane* pPlane) {
		Bullet* pBullet = new Bullet(1, pPlane, pPlane->GetRect(), pPlane->GetOrientation(), 3);
		m_bulletList.push_back(pBullet);
	}

	void SceneMng::Broadcast(Object* pObject) {
		list<Plane*>::iterator planeIt = m_planeList.begin();
		list<Plane*>::iterator planeItEnd = m_planeList.end();
		for (; planeIt != planeItEnd; planeIt++) {
			Plane* pPlane = *planeIt;
			if (!pPlane || !pPlane->NeedNotify(pObject)) {
				continue;
			}

			// to do Notify
		}
	}
}