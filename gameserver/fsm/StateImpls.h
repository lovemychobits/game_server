#ifndef GAMESERVER_STATEIMPLS_H
#define GAMESERVER_STATEIMPLS_H

#include "State.h"

// �ӵ�����������ʼ״̬
class BulletCreatedState : public State
{
public:
	virtual void OnEnter() override;						// ����״̬
	virtual void OnExit() override;							// �˳�״̬
	virtual void Update() override;							// �ڴ�״̬�µ���Ϊ
};

// �ӵ�����״̬
class BulletFlyState : public State
{
public:
	virtual void OnEnter() override;						// ����״̬
	virtual void OnExit() override;							// �˳�״̬
	virtual void Update() override;							// �ڴ�״̬�µ���Ϊ
};

// �ӵ�����״̬
class BulletDestroyState : public State
{
public:
	virtual void OnEnter() override;						// ����״̬
	virtual void OnExit() override;							// �˳�״̬
	virtual void Update() override;							// �ڴ�״̬�µ���Ϊ
};

#endif