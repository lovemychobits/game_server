#include "State.h"

void State::OnEnter()
{
	return;
}

void State::OnExit()
{
	return;
}

void State::Update()
{
	return;
}

void State::NextState(State* pNext) 
{
	if (!pNext) {
		return;
	}
	OnExit();						// ִ�е�ǰ״̬�˳�

	pNext->OnEnter();				// ��һ��״̬����
	pNext->Update();				// ��һ��״ִ̬��
	return;
}