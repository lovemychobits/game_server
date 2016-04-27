#ifndef GAMESERVER_STATE_H
#define GAMESERVER_STATE_H

#define override 

class State
{
public:
	virtual void OnEnter();							// 进入状态
	virtual void OnExit();							// 退出状态
	virtual void Update();							// 在此状态下的行为
	virtual void NextState(State* pNext);			// 设置下一个状态
};

#endif