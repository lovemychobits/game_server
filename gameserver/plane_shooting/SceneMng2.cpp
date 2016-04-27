#include "SceneMng2.h"
#include "Bullet.h"

namespace plane_shooting {
	SceneMng::SceneMng() {

	}

	SceneMng::~SceneMng() {

	}

	void SceneMng::OnTimer(const boost::system::error_code& errCode) {
		// 模拟子弹飞行，及广播
		list<Bullet*>::iterator bulletIt = m_bulletList.begin();
		list<Bullet*>::iterator bulletItEnd = m_bulletList.end();
		for (; bulletIt != bulletItEnd; bulletIt++) {
			Bullet* pBullet = *bulletIt;
			if (!pBullet) {
				continue;
			}

			pBullet->fly();
			Broadcast(pBullet);
		}

		// 广播当前飞机，因为可能有刚加入场景的新飞机
		list<Plane*>::iterator planeIt = m_planeList.begin();
		list<Plane*>::iterator planeItEnd = m_planeList.end();
		for (; planeIt != planeItEnd; planeIt++) {
			Plane* pPlane = *planeIt;
			if (!pPlane) {
				continue;
			}

			Broadcast(pPlane);
		}
	}

	void SceneMng::AddPlane(uint16_t uPlaneId, Vector2D pos, int8_t nOrientation) {
		Plane* pPlane = new Plane(uPlaneId, pos, nOrientation);
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
		Bullet* pBullet = new Bullet(1, pPlane, pPlane->GetPos(), pPlane->GetOrientation(), 3);
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