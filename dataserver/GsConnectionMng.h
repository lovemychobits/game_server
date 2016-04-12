#ifndef DATASERVER_GSCONNECTIONMNG_H
#define DATASERVER_GSCONNECTIONMNG_H

#include "header.h"

/*
	管理DataServer和GameServer之间的连接，如果以后需要多GameServer架构，这里也方便扩展
*/
class GsConnectionMng
{
public:
	static GsConnectionMng* GetInstance()
	{
		static GsConnectionMng instance;
		return &instance;
	}

	void SetGsConnection(IConnection* pGsConnection, uint32_t uGsIndex=0);
	void SendMsgToGS(const char* pBuf, uint32_t uSize, uint32_t uGsIndex=0);

private:
	boost::mutex m_mutex;
	vector<IConnection*> m_gsConnectionList;
};

#define gpGsConnMng GsConnectionMng::GetInstance()

#endif