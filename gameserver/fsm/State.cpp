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
	OnExit();						// 执行当前状态退出

	pNext->OnEnter();				// 下一个状态进入
	pNext->Update();				// 下一个状态执行
	return;
}