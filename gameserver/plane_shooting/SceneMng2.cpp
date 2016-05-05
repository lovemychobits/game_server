#include "SceneMng2.h"
#include "Bullet.h"
#include "../../utils/Utility.h"
#include "../../utils/ProtoTemplate.hpp"
#include "../../protocol/client.pb.h"
#include <vector>
#include <iostream>
using namespace std;

namespace plane_shooting {
	SceneMng::SceneMng() : m_uPlaneId(1) {
		Rectangle rect(250, 250, 500, 500);
		m_pFixedQuadTree = new FixedQuadTree(rect, 5);
		ObjectInNodeMng::GetInstance()->SetHeadNode(m_pFixedQuadTree);
	}

	SceneMng::~SceneMng() {

	}

	void SceneMng::OnTimer(const boost::system::error_code& errCode) {
		/*
			�����ӵ�ȥ��ÿ���ɻ��Ƿ�����ײ��Ч�ʱȽϵͣ���Ϊ�ӵ�����ͨ����ǳ��ࡣ

			�²��ԣ�
			���ݵ�ͼ�Ĵ�С����������ͼ���Ĳ������̶��ֳ�4����256��Сģ�飩������5����1024��Сģ�飩���ӵ��ͷɻ������뵽�Ĳ����У�
			ÿ�μ����ײ��ʱ�򣬲�ȥ�����ӵ�����ȥ�����ɻ���Ȼ����ݷɻ����ڵ��Ĳ������ж��Ƿ�����ײ��
			���ڷɻ�������ͨ��ԶС���ӵ������Ա������ٶȾͿ��������ܶ��ˣ�������ʱ��û�����µ��㷨���ԣ��д���֤
		*/
		
		DWORD dwStart = ::GetTickCount();
		// ģ���ӵ����У����㲥
		list<Bullet*>::iterator bulletIt = m_bulletList.begin();
		list<Bullet*>::iterator bulletItEnd = m_bulletList.end();
		list<Object*> objList;
		for (; bulletIt != bulletItEnd;) {
			Bullet* pBullet = *bulletIt;
			if (!pBullet) {
				continue;
			}

			Vector2D oldPos = pBullet->GetPos();
			pBullet->fly();
			ObjectInNodeMng::GetInstance()->CheckUpdate(pBullet, oldPos);

			// �ͷɻ�����ײ��� 
			m_pFixedQuadTree->retrieve(pBullet->GetRect(), objList);
			list<Object*>::iterator objIt = objList.begin();
			for (; objIt != objList.end(); objIt++) {
				if (CheckCollision(pBullet->GetPos(), *objIt)) {
					pBullet->SetStatus(DEAD);
				}
			}

			objList.clear();
			// ֪ͨ�ɻ�
			Broadcast(pBullet);

			// ����Ƿ���Ҫɾ���ӵ�
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

	void SceneMng::TestQuadTree() {
		Rectangle rect;
		rect.fWidth = 3;
		rect.fHeight = 3;

		// test insert
		vector<Plane*> planeList;
		for (int i = 0; i < 100; i++)
		{
			rect.x = (float)cputil::GenRandom(1, 500);
			rect.y = (float)cputil::GenRandom(1, 500);

			Plane* pPlane = new Plane(i, rect, 1, NULL);
			planeList.push_back(pPlane);
			m_pFixedQuadTree->insert(pPlane);
		}

		for (int i = 0; i < 100; i++) {
			Plane* pPlane = planeList.at(i / 20);
			Rectangle& rect = pPlane->GetRect();
			rect.fWidth = 0;
			rect.fHeight = 0;
			rect.x = (float)cputil::GenRandom(1, 500);
			rect.y = (float)cputil::GenRandom(1, 500);

			Bullet* pBullet = new Bullet(1, pPlane, pPlane->GetRect(), pPlane->GetOrientation(), 3);
			m_bulletList.push_back(pBullet);
		}

		return;
	}

	void SceneMng::PlayerEnter(IConnection* pConn) {
		if (!pConn) {
			return;
		}

		Vector2D pos;
		pos.x = (float)cputil::GenFloatRandom(1.0, 5.0);
		pos.y = (float)cputil::GenFloatRandom(1.0, 5.0);
		Plane* pPlane = AddPlane(pConn, m_uPlaneId++, pos, 0);
		if (!pPlane) {
			return;
		}

		// ������Ϣ���ͻ���
		client::EnterGameAck enterGameAck;
		enterGameAck.mutable_pos()->set_x(pos.x);
		enterGameAck.mutable_pos()->set_y(pos.y);

		string strResponse;
		cputil::BuildResponseProto(enterGameAck, strResponse, client::ClientProtocol::REQ_ENTER_GAME);
		pPlane->SendMsg(strResponse.c_str(), strResponse.size());
		return;
	}

	Plane* SceneMng::AddPlane(IConnection* pConn, uint16_t uPlaneId, Vector2D pos, int8_t nOrientation) {
		Rectangle rect;
		rect.x = pos.x;
		rect.y = pos.y;
		rect.fWidth = 1;
		rect.fHeight = 1;

		Plane* pPlane = new Plane(uPlaneId, rect, nOrientation, pConn);
		m_planeList.push_back(pPlane);
		m_connToPlaneMap.insert(make_pair(pConn, pPlane));
		m_pFixedQuadTree->insert(pPlane);

		// ���¼���ķɻ�֪ͨ�������
		Broadcast(pPlane);

		// ֪ͨ�¼���ķɻ����������Ϣ
		NotifyOthers(pPlane);
		return pPlane;
	}

	void SceneMng::PlaneMove(Plane* pPlane, Vector2D newPos, uint32_t uNewAngle) {
		if (!pPlane) {
			return;
		}

		Vector2D oldPos = pPlane->GetPos();
		pPlane->SetPos(newPos);
		pPlane->SetAngle(uNewAngle);

		ObjectInNodeMng::GetInstance()->CheckUpdate(pPlane, oldPos);
		
		Broadcast(pPlane);
		return;
	}

	void SceneMng::PlaneShoot(Plane* pPlane) {
		Bullet* pBullet = new Bullet(1, pPlane, pPlane->GetRect(), pPlane->GetAngle(), 0.5);
		m_bulletList.push_back(pBullet);
		m_pFixedQuadTree->insert(pBullet);
	}

	void SceneMng::Broadcast(Object* pObject) {
		string strResponse;

		list<Plane*>::iterator planeIt = m_planeList.begin();
		list<Plane*>::iterator planeItEnd = m_planeList.end();
		for (; planeIt != planeItEnd; planeIt++) {
			Plane* pPlane = *planeIt;
			if (!pPlane || !pPlane->NeedNotify(pObject)) {
				continue;
			}

			// Notify other player
			client::NotifyObjs objsNty;
			client::PBObject* pPBObject = objsNty.add_objlist();
			pPBObject->set_objid(pObject->GetObjId());
			pPBObject->set_type((client::ObjectType)pObject->GetType());
			pPBObject->mutable_pos()->set_x(pObject->GetRect().x);
			pPBObject->mutable_pos()->set_y(pObject->GetRect().y);
			pPBObject->set_speed(pPlane->GetSpeed());
			
			cputil::BuildResponseProto(objsNty, strResponse, client::ClientProtocol::NTY_OBJS);
			pPlane->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
	}

	void SceneMng::NotifyOthers(Plane* pSelf) {
		if (!pSelf)
		{
			return;
		}

		string strResponse;

		list<Plane*>::iterator planeIt = m_planeList.begin();
		list<Plane*>::iterator planeItEnd = m_planeList.end();
		for (; planeIt != planeItEnd; planeIt++) {
			Plane* pPlane = *planeIt;
			if (!pPlane || !pSelf->NeedNotify(pPlane)) {
				continue;
			}

			// Notify other player
			client::NotifyObjs objsNty;
			client::PBObject* pPBObject = objsNty.add_objlist();
			pPBObject->set_objid(pPlane->GetObjId());
			pPBObject->mutable_pos()->set_x(pPlane->GetRect().x);
			pPBObject->mutable_pos()->set_y(pPlane->GetRect().y);
			pPBObject->set_speed(pPlane->GetSpeed());

			cputil::BuildResponseProto(objsNty, strResponse, client::ClientProtocol::NTY_OBJS);
			pSelf->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
	}

	// ��������ײ���
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
}