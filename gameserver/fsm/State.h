#ifndef GAMESERVER_STATE_H
#define GAMESERVER_STATE_H

#define override 

class State
{
public:
	virtual void OnEnter();							// ����״̬
	virtual void OnExit();							// �˳�״̬
	virtual void Update();							// �ڴ�״̬�µ���Ϊ
	virtual void NextState(State* pNext);			// ������һ��״̬
};

#endif