#pragma once

#include <list>
#include <stdint.h>
#include "Object.h"
using namespace plane_shooting;
using namespace std;

/*
	�Ĳ������������еķɻ����ϰ�����������ײ��⣬�Լ�����ҵķɻ��Ĺ㲥
*/
class QuadTree
{
#define NODE_SIZE 4
#define NODE_MAX_BOJS 5

public:
	QuadTree(plane_shooting::Rectangle bounds);
	~QuadTree();

public:
	void insert(Object* pObj);
	void remove(Object* pObj);
	void retrieve(const plane_shooting::Rectangle& rect, list<Object*>& objList);

private:
	void Split();
	int16_t GetIndex(const plane_shooting::Rectangle& rect);
	

private:
	list<Object*> m_objects;
	QuadTree* nodes[NODE_SIZE];
	plane_shooting::Rectangle m_bounds;
};