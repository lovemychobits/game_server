#include "QuadTree.h"

QuadTree::QuadTree(plane_shooting::Rectangle bounds) {
	m_bounds = bounds;
	for (int i = 0; i < NODE_SIZE; ++i) {
		nodes[i] = NULL;
	}
}

QuadTree::~QuadTree() {
	// to do clear
}

void QuadTree::Split() {
	float fSubWidth = m_bounds.fWidth / 2;
	float fSubHeight = m_bounds.fHeight / 2;
	float x = m_bounds.x;
	float y = m_bounds.y;

	/*
		  width
	-----------------
	|	0	|	1	|
	|-----(x,y)-----| height
	|	2  	|	3	|
	-----------------
	*/

	nodes[0] = new QuadTree(plane_shooting::Rectangle(x - fSubWidth / 2, y + fSubHeight / 2, fSubWidth, fSubHeight));
	nodes[1] = new QuadTree(plane_shooting::Rectangle(x + fSubWidth / 2, y + fSubHeight / 2, fSubWidth, fSubHeight));
	nodes[2] = new QuadTree(plane_shooting::Rectangle(x - fSubWidth / 2, y - fSubHeight / 2, fSubWidth, fSubHeight));
	nodes[3] = new QuadTree(plane_shooting::Rectangle(x + fSubWidth / 2, y - fSubHeight / 2, fSubWidth, fSubHeight));

	return;
}

// 获取一个矩形所在的四叉树
int16_t QuadTree::GetIndex(const Rectangle& rect) {
	int16_t index = -1;

	bool topQuadrant = (rect.y + rect.fHeight / 2 < m_bounds.y + m_bounds.fHeight / 2) && (rect.y - rect.fHeight / 2 > m_bounds.y);
	bool bottomQuadrant = (rect.y + rect.fHeight/2 < m_bounds.y) && (rect.y - rect.fHeight / 2 > m_bounds.y - m_bounds.fHeight / 2);

	// left quadrant
	if ((rect.x - rect.fWidth/2 > m_bounds.x - m_bounds.fWidth/2) && (rect.x + rect.fWidth/2 < m_bounds.x)) {
		if (topQuadrant) {
			index = 0;
		}
		if (bottomQuadrant) {
			index = 2;
		}
	}

	// right quadrant
	if ((rect.x - rect.fWidth/2 > m_bounds.x) && (rect.x + rect.fWidth/2 < m_bounds.x + m_bounds.fWidth/2)) {
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
		if (nodes[0] == NULL) {						// 分裂
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

// 从死叉树中删除物体
void QuadTree::remove(Object* pObj) {
	if (!pObj) {
		return;
	}

	const plane_shooting::Rectangle& rect = pObj->GetRect();
	if (nodes[0] != NULL) {					// 去查找子树
		int16_t index = GetIndex(rect);
		
		if (index != -1) {
			nodes[index]->remove(pObj);
			return;
		}
	}

	// 删除物体
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
