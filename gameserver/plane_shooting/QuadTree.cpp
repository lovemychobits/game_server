#include "QuadTree.h"

QuadTree::QuadTree(plane_shooting::Rectangle bounds) {
	m_bounds = bounds;
	for (int i = 0; i < NODE_SIZE; ++i) {
		nodes[i] = NULL;
	}
}

QuadTree::~QuadTree() {
	m_objects.clear();
	for (int i = 0; i < NODE_SIZE; ++i) {
		nodes[i] = NULL;
	}
}

void QuadTree::Split() {
	int16_t uSubWidth = m_bounds.uWidth / 2;
	int16_t uSubHeight = m_bounds.uHeight / 2;
	uint16_t x = m_bounds.x;
	uint16_t y = m_bounds.y;

	/*
		  width
	-----------------
	|	0	|	1	|
	|-----(x,y)-----| height
	|	2  	|	3	|
	-----------------
	*/

	nodes[0] = new QuadTree(plane_shooting::Rectangle(x - uSubWidth / 2, y + uSubHeight / 2, uSubWidth, uSubHeight));
	nodes[1] = new QuadTree(plane_shooting::Rectangle(x + uSubWidth / 2, y + uSubHeight / 2, uSubWidth, uSubHeight));
	nodes[2] = new QuadTree(plane_shooting::Rectangle(x - uSubWidth / 2, y - uSubHeight / 2, uSubWidth, uSubHeight));
	nodes[3] = new QuadTree(plane_shooting::Rectangle(x + uSubWidth / 2, y - uSubHeight / 2, uSubWidth, uSubHeight));

	return;
}

// ��ȡһ���������ڵ��Ĳ���
int16_t QuadTree::GetIndex(const Rectangle& rect) {
	int16_t index = -1;

	//bool topQuadrant = (rect.y < m_bounds.y) && (rect.y - rect.uHeight > horizontal_mid);
	bool topQuadrant = (rect.y + rect.uHeight / 2 < m_bounds.y + m_bounds.uHeight / 2) && (rect.y - rect.uHeight / 2 > m_bounds.y);
	//bool bottomQuadrant = (rect.y < horizontal_mid) && (rect.y - rect.uHeight > m_bounds.y - m_bounds.uHeight);
	bool bottomQuadrant = (rect.y + rect.uHeight/2 < m_bounds.y) && (rect.y - rect.uHeight / 2 > m_bounds.y - m_bounds.uHeight / 2);

	// left quadrant
	if ((rect.x - rect.uWidth/2 > m_bounds.x - m_bounds.uWidth/2) && (rect.x + rect.uWidth/2 < m_bounds.x)) {
		if (topQuadrant) {
			index = 0;
		}
		if (bottomQuadrant) {
			index = 2;
		}
	}

	// right quadrant
	if ((rect.x - rect.uWidth/2 > m_bounds.x) && (rect.x + rect.uWidth/2 < m_bounds.x + m_bounds.uWidth/2)) {
		if (topQuadrant) {
			index = 1;
		}
		if (bottomQuadrant) {
			index = 3;
		}
	}

	return index;
}

void QuadTree::insert(Object* pObj) {
	if (!pObj) {
		return;
	}
	const plane_shooting::Rectangle& rect = pObj->GetRect();
	if (nodes[0] != NULL) {
		int16_t index = GetIndex(rect);

		if (index != -1) {
			nodes[index]->insert(pObj);
			return;
		}
	}

	m_objects.push_back(pObj);

	if (m_objects.size() > NODE_MAX_BOJS) {
		if (nodes[0] == NULL) {						// ����
			Split();
		}

		list<Object*>::iterator objIt = m_objects.begin();
		list<Object*>::iterator objItEnd = m_objects.end();
		for (; objIt != objItEnd; ) {
			int index = 0;
			Object* obj = *objIt;
			index = GetIndex(obj->GetRect());
			if (index != -1) {
				nodes[index]->insert(obj);
				m_objects.erase(objIt++);
			}
			else {
				objIt++;
			}
		}
	}
}

// ����������ɾ������
void QuadTree::remove(Object* pObj) {
	if (!pObj) {
		return;
	}

	const plane_shooting::Rectangle& rect = pObj->GetRect();
	if (nodes[0] != NULL) {					// ȥ��������
		int16_t index = GetIndex(rect);
		
		if (index != -1) {
			nodes[index]->remove(pObj);
			return;
		}
	}

	// ɾ������
	list<Object*>::iterator objIt = m_objects.begin();
	list<Object*>::iterator objItEnd = m_objects.end();
	for (; objIt != objItEnd; objIt++) {
		if (pObj == *objIt) {
			m_objects.erase(objIt);
			return;
		}
	}

	return;
}


void QuadTree::retrieve(const plane_shooting::Rectangle& rect, list<Object*>& objList) {
	int16_t index = GetIndex(rect);
	if (index != -1 && nodes[0] != NULL) {
		nodes[index]->retrieve(rect, objList);
	}

	list<Object*>::iterator objIt = m_objects.begin();
	list<Object*>::iterator objItEnd = m_objects.end();
	for (; objIt != objItEnd; objIt++) {
		objList.push_back(*objIt);
	}

	return;
}