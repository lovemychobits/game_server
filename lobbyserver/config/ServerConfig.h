#ifndef LOBBYSERVER_SERVERCONFIG_H
#define LOBBYSERVER_SERVERCONFIG_H

#include "../../tinyxml/tinyxml.h"
#include <boost/lexical_cast.hpp>

class ServerConfig
{
public:
	static ServerConfig* GetInstance()
	{
		static ServerConfig instance;
		return &instance;
	}

	bool LoadServerConf(const char* pFilePath);
	int GetListenPort()
	{
		return m_nListenPort;
	}
	const char* GetBindIp()
	{
		return m_szBindIp;
	}

	int GetClientListenPort()
	{
		return m_nListenClientPort;
	}

	const char* GetClientBindIp()
	{
		return m_szBindClientIp;
	}

private:
	ServerConfig() : m_nListenPort(0)
	{
		memset(m_szBindIp, 0, sizeof(m_szBindIp));
	}

private:
	TiXmlDocument m_xmlDoc;

	int m_nListenPort;
	char m_szBindIp[32];

	int m_nListenClientPort;
	char m_szBindClientIp[32];
};

#define gpServerConfig ServerConfig::GetInstance()


#endif