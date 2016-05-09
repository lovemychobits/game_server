#include "SceneMng2.h"
#include "Bullet.h"
#include "../../utils/Utility.h"
#include "../../utils/ProtoTemplate.hpp"
#include "../../protocol/client.pb.h"
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

namespace plane_shooting {
	SceneMng::SceneMng() : m_uPlaneId(1) {
		Rectangle rect(250, 250, 500, 500);
		m_pFixedQuadTree = new FixedQuadTree(rect, 5);
		ObjectInNodeMng::GetInstance()->SetHeadNode(m_pFixedQuadTree);

		// 测试
		TestCheckIntersect();
	}

	SceneMng::~SceneMng() {

	}

	void SceneMng::OnTimer(const boost::system::error_code& errCode) {
		/*
			遍历子弹去跟每个飞机是否发生碰撞，效率比较低，因为子弹数量通常会非常多。

			新策略：
			根据地图的大小，将整个地图用四叉树划固定分成4级（256个小模块），或者5级（1024个小模块），子弹和飞机都加入到四叉树中，
			每次检测碰撞的时候，不去遍历子弹，而去遍历飞机，然后根据飞机所在的四叉树来判断是否发生碰撞。
			由于飞机的数量通常远小于子弹，所以遍历的速度就可以提升很多了，不过暂时还没有用新的算法测试，有待验证
		*/
		
		DWORD dwStart = ::GetTickCount();
		// 模拟子弹飞行，及广播
		list<Bullet*>::iterator bulletIt = m_bulletList.begin();
		list<Bullet*>::iterator bulletItEnd = m_bulletList.end();
		list<Object*> objList;
		for (; bulletIt != bulletItEnd;) {
			Bullet* pBullet = *bulletIt;
			if (!pBullet) {
				continue;
			}

			Vector2D oldPos = pBullet->GetPos();
			pBullet->Fly();
			ObjectInNodeMng::GetInstance()->CheckUpdate(pBullet, oldPos);

			// 和飞机的碰撞检测 
			m_pFixedQuadTree->Retrieve(pBullet->GetRect(), objList);
			list<Object*>::iterator objIt = objList.begin();
			for (; objIt != objList.end(); objIt++) {
				if (CheckIntersect(oldPos, pBullet->GetPos(), (*objIt)->GetRect())) {
					pBullet->SetStatus(DEAD);
					BroadcastCollision(pBullet, *objIt);					// 通知发生碰撞了
				}
			}

			objList.clear();
			// 通知飞机
			Broadcast(pBullet);

			// 检测是否需要删除子弹
			if (pBullet->CanDestroy())
			{
				m_bulletList.erase(bulletIt++);
				continue;
			}
			bulletIt++;
		}
		DWORD dwEnd = ::GetTickCount();
		//cout << "time cost=[" << dwEnd - dwStart << "]" << endl;
	}

	void SceneMng::PlayerEnter(IConnection* pConn) {
		if (!pConn) {
			return;
		}

		// 随机初始化起始点
		Vector2D pos;
		pos.x = (float)cputil::GenFloatRandom(1.0, 5.0);
		pos.y = (float)cputil::GenFloatRandom(1.0, 5.0);
		Plane* pPlane = AddPlane(pConn, m_uPlaneId++, pos, 0);
		if (!pPlane) {
			return;
		}

		// 返回消息给客户端
		client::EnterGameAck enterGameAck;
		enterGameAck.mutable_pos()->set_x(pos.x);
		enterGameAck.mutable_pos()->set_y(pos.y);

		string strResponse;
		cputil::BuildResponseProto(enterGameAck, strResponse, client::ClientProtocol::REQ_ENTER_GAME);
		pPlane->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	Plane* SceneMng::AddPlane(IConnection* pConn, uint16_t uPlaneId, Vector2D pos, int8_t nOrientation) {
		// 初始化飞机碰撞矩形信息
		Rectangle rect;
		rect.x = pos.x;
		rect.y = pos.y;
		rect.fWidth = 1;
		rect.fHeight = 1;

		Plane* pPlane = new Plane(uPlaneId, rect, nOrientation, pConn);
		if (!pPlane) {
			return NULL;
		}

		m_planeList.push_back(pPlane);
		m_connToPlaneMap.insert(make_pair(pConn, pPlane));
		m_pFixedQuadTree->Insert(pPlane);							// 加入到四叉树中

		// 将新加入的飞机通知其他玩家
		Broadcast(pPlane);

		// 通知新加入的飞机其他玩家信息
		NotifyOthers(pPlane);
		return pPlane;
	}

	void SceneMng::PlaneMove(Plane* pPlane, Vector2D newPos, uint32_t uFireAngle, uint32_t uFlyAngle) {
		if (!pPlane) {
			return;
		}

		Vector2D oldPos = pPlane->GetPos();
		pPlane->SetPos(newPos);
		pPlane->SetFireAngle(uFireAngle);
		pPlane->SetFlyAngle(uFlyAngle);

		ObjectInNodeMng::GetInstance()->CheckUpdate(pPlane, oldPos);
		
		Broadcast(pPlane);
		return;
	}

	void SceneMng::PlaneShoot(Plane* pPlane) {
		Bullet* pBullet = new Bullet(1, pPlane, pPlane->GetRect(), pPlane->GetFireAngle(), 0.5);
		if (!pBullet) {
			return;
		}
		m_bulletList.push_back(pBullet);
		m_pFixedQuadTree->Insert(pBullet);								// 将子弹加入四叉树
	}

	// 将物体信息通知所有其他玩家
	void SceneMng::Broadcast(Object* pObject) {
		string strResponse;

		list<Plane*>::iterator planeIt = m_planeList.begin();
		list<Plane*>::iterator planeItEnd = m_planeList.end();
		for (; planeIt != planeItEnd; planeIt++) {
			Plane* pPlane = *planeIt;
			if (!pPlane || !pPlane->NeedNotify(pObject)) {
				continue;
			}

			// 通知其他玩家pObject的信息
			client::NotifyObjs objsNty;
			client::PBObject* pPBObject = objsNty.add_objlist();
			pPBObject->set_objid(pObject->GetObjId());
			pPBObject->set_type((client::ObjectType)pObject->GetType());
			pPBObject->mutable_pos()->set_x(pObject->GetRect().x);
			pPBObject->mutable_pos()->set_y(pObject->GetRect().y);

			if (pObject->GetType() == Plane_Type) {
				Plane* pTmpPlane = (Plane*)pObject;
				pPBObject->set_speed(pTmpPlane->GetSpeed());
				pPBObject->set_fireangle(pTmpPlane->GetFireAngle());
				pPBObject->set_flyangle(pTmpPlane->GetFlyAngle());
			}
			else if (pObject->GetType() == Bullet_Type) {
				Bullet* pTmpBullet = (Bullet*)pObject;
				pPBObject->set_speed(pTmpBullet->GetSpeed());
				pPBObject->set_flyangle(pTmpBullet->GetAngle());
			}

			cputil::BuildResponseProto(objsNty, strResponse, client::ClientProtocol::NTY_OBJS);
			pPlane->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
	}

	// 将其他玩家及子弹信息同步给自己
	void SceneMng::NotifyOthers(Plane* pSelf) {
		if (!pSelf)
		{
			return;
		}

		// 获取飞机所在视野内的所有物体
		list<Object*> objList;
		m_pFixedQuadTree->RetrieveArea(pSelf->GetViewRect(), objList);

		// 通知所有物体信息
		string strResponse;
		list<Object*>::iterator objIt = objList.begin();
		list<Object*>::iterator objItEnd = objList.end();
		for (; objIt != objItEnd; objIt++) {
			Object* pObject = *objIt;
			if (!pObject || !pSelf->NeedNotify(pObject)) {
				continue;
			}

			// Notify other player
			client::NotifyObjs objsNty;
			client::PBObject* pPBObject = objsNty.add_objlist();
			pPBObject->set_objid(pObject->GetObjId());
			pPBObject->set_type((client::ObjectType)pObject->GetType());
			pPBObject->mutable_pos()->set_x(pObject->GetRect().x);
			pPBObject->mutable_pos()->set_y(pObject->GetRect().y);
			if (pObject->GetType() == Plane_Type)
			{
				Plane* pTmpPlane = (Plane*)pObject;
				pPBObject->set_speed(pTmpPlane->GetSpeed());
				pPBObject->set_fireangle(pTmpPlane->GetFireAngle());
				pPBObject->set_flyangle(pTmpPlane->GetFlyAngle());
			}
			else if (pObject->GetType() == Bullet_Type)
			{
				Bullet* pTmpBullet = (Bullet*)pObject;
				pPBObject->set_speed(pTmpBullet->GetSpeed());
				pPBObject->set_flyangle(pTmpBullet->GetAngle());
			}

			cputil::BuildResponseProto(objsNty, strResponse, client::ClientProtocol::NTY_OBJS);
			pSelf->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
	}

	// 先做点碰撞检测
	bool SceneMng::CheckCollision(Vector2D pos, Object* pObject) {
		Rectangle rect = pObject->GetRect();
		if ((pos.x < (rect.x - rect.fWidth / 2)) || (pos.x > (rect.x + rect.fWidth / 2))) {
			return false;
		}
		if ((pos.y < (rect.y - rect.fHeight / 2)) || (pos.y > (rect.y + rect.fHeight / 2))) {
			return false;
		}

		return true;
	}

	// 判断线段与矩形是否相交
	// 将线段矩形化，也就是说给线段人为的加上一个宽度，不过这个宽度非常小
	bool SceneMng::CheckIntersect(Vector2D oldPos, Vector2D newPos, Rectangle rect) {
		float fLength = sqrt(((oldPos.x - newPos.x) * (oldPos.x - newPos.x) + (oldPos.y - newPos.y) * (oldPos.y - newPos.y)));
		Rectangle lineRect((oldPos.x + newPos.x) / 2, (newPos.y + newPos.y) / 2, fLength, 0.01f);
		return CheckIntersect(lineRect, rect);
	}

	// 判断两个矩形是否相交
	bool SceneMng::CheckIntersect(Rectangle rect1, Rectangle rect2) {
		if (   min(rect1.x - rect1.fWidth / 2, rect1.x + rect1.fWidth / 2) <= max(rect2.x - rect2.fWidth / 2, rect2.x + rect2.fWidth / 2)
			&& min(rect2.x - rect2.fWidth / 2, rect2.x + rect2.fWidth / 2) <= max(rect1.x - rect1.fWidth / 2, rect1.x + rect1.fWidth / 2)
			&& min(rect1.y - rect1.fHeight / 2, rect1.y + rect1.fHeight / 2) <= max(rect2.y - rect2.fHeight / 2, rect2.y + rect2.fHeight / 2)
			&& min(rect2.y - rect2.fHeight / 2, rect2.y + rect2.fHeight / 2) <= max(rect1.y - rect1.fHeight / 2, rect1.y + rect1.fHeight / 2)) {
			return true;
		}

		return false;
	}

	// 获取飞机对于的网络连接
	Plane* SceneMng::GetPlaneByConn(IConnection* pConn) {
		if (!pConn) {
			return NULL;
		}

		map<IConnection*, Plane*>::iterator planeIt = m_connToPlaneMap.find(pConn);
		if (planeIt != m_connToPlaneMap.end()) {
			return planeIt->second;
		}

		return NULL;
	}

	// 玩家断开连接
	void SceneMng::PlaneDisconnect(IConnection* pConn) {
		map<IConnection*, Plane*>::iterator mapIt = m_connToPlaneMap.find(pConn);
		if (mapIt != m_connToPlaneMap.end()) {
			list<Plane*>::iterator planeIt = m_planeList.begin();
			list<Plane*>::iterator planeItEnd = m_planeList.end();
			for (; planeIt != planeItEnd; planeIt++) {
				if (mapIt->second == *planeIt) {
					m_planeList.erase(planeIt);
					return;
				}
			}
		}
	}

	// 广播发生了碰撞
	void SceneMng::BroadcastCollision(Object* pObject, Object* pTarget) {
		if (!pObject || !pTarget) {
			return;
		}

		// to do
		client::NotifyCollision collisionNty;

	}

	// 测试矩形相交功能
	void SceneMng::TestCheckIntersect() {
		Rectangle rect1(30, 20, 30, 20);
		Rectangle rect2(60, 15, 10, 10);

		bool bRet = CheckIntersect(rect1, rect2);
		bRet = CheckIntersect(Vector2D(10, 10), Vector2D(25, 15), rect1);
		bRet = CheckIntersect(Vector2D(10, 10), Vector2D(25, 15), rect2);
		return;
	}
}