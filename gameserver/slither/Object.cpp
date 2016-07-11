#include "Object.h"
#include "Snake.h"
#include "Food.h"

namespace slither {
	// ���������������
	void Grid::AddObj(Object* pObj) {
		if (!pObj) {
			return;
		}

		if (pObj->GetType() == Food_Type) {
			foodList.push_back(pObj);
		}
		else if (pObj->GetType() == Snake_Head_Type || pObj->GetType() == Snake_Body_Type) {

			SnakeBodyNode* pNode = (SnakeBodyNode*)pObj;
			Snake* pSnake = pNode->GetOwner();
			if (!pSnake) {
				return;
			}

			map<Snake*, set<Object*> >::iterator snakeIt = m_snakeMap.find(pSnake);
			if (snakeIt == m_snakeMap.end()) {						// ���߻�������
				set<Object*> nodeList;
				nodeList.insert(pObj);
				m_snakeMap.insert(make_pair(pSnake, nodeList));
			}
			else {
				(snakeIt->second).insert(pObj);
			}
		}
	}

	// �Ӹ�����ɾ������
	void Grid::DelObj(Object* pObj) {
		if (!pObj) {
			return;
		}

		if (pObj->GetType() == Food_Type) {															// ʳ��
			list<Object*>::iterator objIt = foodList.begin();
			for (; objIt != foodList.end();) {
				if (*objIt == pObj) {
					foodList.erase(objIt++);
					break;
				}
				objIt++;
			}
		}
		else if (pObj->GetType() == Snake_Head_Type || pObj->GetType() == Snake_Body_Type) {		// ��
			SnakeBodyNode* pNode = (SnakeBodyNode*)pObj;
			if (!pNode) {
				return;
			}
			Snake* pSnake = pNode->GetOwner();
			if (!pSnake) {
				return;
			}

			map<Snake*, set<Object*> >::iterator snakeIt = m_snakeMap.find(pSnake);
			if (snakeIt == m_snakeMap.end()) {														// ���߻�������, �����ϲ������������
				ERRORLOG("grid =[" << x << "," << y << "] snake id=[" << pSnake->GetSnakeId() << "], just body in grid.");
				return;
			}

			(snakeIt->second).erase(pObj);
			if ((snakeIt->second).empty()) {														// ����ߵĽڵ㶼�����ˣ���ô���߾ʹ����������ɾ��
				m_snakeMap.erase(pSnake);
			}
		}
	}

	bool Grid::IsExist(Snake* pSnake) {
		map<Snake*, set<Object*> >::iterator snakeIt = m_snakeMap.find(pSnake);
		if (snakeIt != m_snakeMap.end()) {
			return true;
		}

		return false;
	}

}