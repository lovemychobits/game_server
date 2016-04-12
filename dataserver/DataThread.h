#ifndef DATASERVER_WORKTHREAD_H
#define DATASERVER_WORKTHREAD_H

#include "../network/header.h"
#include "../utils/ProtoTemplate.hpp"
#include "PlayerDataMng.h"
using namespace cpnet;

class DataThread
{
public:
	DataThread();
	~DataThread();

	void Push(const char* pBuf, int nLen);
	bool Init();
	void Run();
	void Stop();

private:
	bool m_bRun;
	deque<string> m_msgQueue;
	boost::mutex m_mutex;
	boost::condition m_cond;
	PlayerDataMng m_playerDataMng;
};

#endif