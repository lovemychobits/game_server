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
		m_pQuadTree = new QuadTree(rect);
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

		for (int i = 0; i < 100; i++) {
			// ģ���ӵ����У����㲥
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
		for (int i = 0; i < 100; i++)
		{
			rect.x = cputil::GenRandom(1, 500);
			rect.y = cputil::GenRandom(1, 500);

			Plane* pPlane = new Plane(i, rect, 1, NULL);
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

	void SceneMng::PlayerEnter(IConnection* pConn) {
		if (!pConn) {
			return;
		}

		Vector2D pos;
		pos.x = cputil::GenRandom(20, 50);
		pos.y = cputil::GenRandom(20, 50);
		AddPlane(pConn, m_uPlaneId++, pos, 1);
	}

	void SceneMng::AddPlane(IConnection* pConn, uint16_t uPlaneId, Vector2D pos, int8_t nOrientation) {
		Rectangle rect;
		rect.x = pos.x;
		rect.y = pos.y;
		rect.uWidth = 3;
		rect.uHeight = 3;

		Plane* pPlane = new Plane(uPlaneId, rect, nOrientation, pConn);
		m_planeList.push_back(pPlane);

		m_connToPlaneMap.insert(make_pair(pConn, pPlane));

		Broadcast(pPlane);
	}

	void SceneMng::PlaneMove(Plane* pPlane, uint32_t uAction) {
		if (!pPlane) {
			return;
		}
		switch (uAction) {
		case client::MoveType::MOVE_UP:
			{
				pPlane->Up();
			}
			break;
		case client::MoveType::MOVE_DOWN:
			{
				pPlane->Down();
			}
			break;
		case client::MoveType::MOVE_LEFT:
			{
				pPlane->Left();
			}
			break;
		case client::MoveType::MOVE_RIGHT:
			{
				pPlane->Right();
			}
			break;
		case client::MoveType::TURN_AROUND:
			{
				pPlane->TurnAround();
			}
			break;
		}
		
		Broadcast(pPlane);
		return;
	}

	void SceneMng::PlaneShoot(Plane* pPlane) {
		Bullet* pBullet = new Bullet(1, pPlane, pPlane->GetRect(), pPlane->GetOrientation(), 3);
		m_bulletList.push_back(pBullet);
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
			pPBObject->set_objid(pPlane->GetObjId());
			pPBObject->mutable_pos()->set_x(pPlane->GetRect().x);
			pPBObject->mutable_pos()->set_y(pPlane->GetRect().y);
			pPBObject->set_speed(pPlane->GetSpeed());
			pPBObject->set_orientation(pPlane->GetOrientation());
			
			cputil::BuildResponseProto(objsNty, strResponse, client::ClientProtocol::NTY_OBJS);
			pPlane->SendMsg(strResponse.c_str(), strResponse.size());
		}

		return;
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
}