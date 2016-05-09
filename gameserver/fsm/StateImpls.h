#ifndef GAMESERVER_STATEIMPLS_H
#define GAMESERVER_STATEIMPLS_H

#include "State.h"

// 子弹被创建的起始状态
class BulletCreatedState : public State
{
public:
	virtual void OnEnter() override;						// 进入状态
	virtual void OnExit() override;							// 退出状态
	virtual void Update() override;							// 在此状态下的行为
};

// 子弹飞行状态
class BulletFlyState : public State
{
public:
	virtual void OnEnter() override;						// 进入状态
	virtual void OnExit() override;							// 退出状态
	virtual void Update() override;							// 在此状态下的行为
};

// 子弹销毁状态
class BulletDestroyState : public State
{
public:
	virtual void OnEnter() override;						// 进入状态
	virtual void OnExit() override;							// 退出状态
	virtual void Update() override;							// 在此状态下的行为
};

#endif