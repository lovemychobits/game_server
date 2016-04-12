#ifndef DATASERVER_DATASERVER_H
#define DATASERVER_DATASERVER_H

#include "ServerConfig.h"
#include "../network/INetCluster.h"
#include "../network/header.h"
#include "DataThread.h"
#include "CmdHandler.h"

class GsHandler;
class GsMsgParser;
class DataServer
{
public:
	DataServer() : m_pServerSession(NULL), m_pNetCluster(NULL), m_pGsHandler(NULL), m_pDataThread(NULL), m_pThread(NULL)
	{

	}

	~DataServer()
	{
		int nThreadNum = gpServerConfig->GetThreadNum();
		for (int i=0; i<nThreadNum; i++)
		{
			if (m_pThread[i])
			{
				m_pDataThread[i]->Stop();
				m_pThread[i]->join();
				delete m_pThread[i];
			}

			if (m_pDataThread[i])
			{
				delete m_pDataThread[i];
			}			
		}
		if (m_pDataThread)
		{
			delete[] m_pDataThread;
		}
		if (m_pThread)
		{
			delete[] m_pThread;
		}
	}

	bool Init(const char* pConfPath);

	void Start()
	{
		m_pNetCluster->Run();
	}

	void Stop()
	{
		m_pNetCluster->Stop();
	}

	DataThread* GetDataThread(int nIndex)
	{
		return m_pDataThread[nIndex];
	}

private:
	bool InitLog4cpp();
	bool InitServerConf(const char* pConfPath);
	bool InitServerApp();
	bool InitListenCmd();
	bool InitDataThread();

private:
	IServerSession* m_pServerSession;
	IServerSession* m_pCmdSession;
	INetCluster* m_pNetCluster;
	DataThread** m_pDataThread;
	boost::thread** m_pThread;
	GsHandler* m_pGsHandler;
	CmdHandler* m_pCmdHandler;
};

#endif