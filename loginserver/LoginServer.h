#ifndef LOGINSERVER_LOGINSERVER_H
#define LOGINSERVER_LOGINSERVER_H

#include "../network/INetCluster.h"
#include "../network/header.h"

class ClientHandler;
class LoginServer
{
public:
	LoginServer() : m_pServerSession(NULL), m_pNetCluster(NULL)
	{
	}

	~LoginServer()
	{
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

private:
	bool InitLog4cplus();
	bool InitServerConf(const char* pConfPath);
	bool InitServerApp();
	bool InitDataThread();

private:
	IServerSession		*m_pServerSession;
	INetCluster			*m_pNetCluster;
	ClientHandler		*m_pClientHandler;
};

#endif