#ifndef DATASERVER_SERVERCONFIG_H
#define DATASERVER_SERVERCONFIG_H

#include "../tinyxml/tinyxml.h"
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
	
	uint32_t GetAreaId()
	{
		return m_uAreaId;
	}
	
	int GetListenPort()
	{
		return m_uListenPort;
	}
	const char* GetBindIp()
	{
		return m_szBindIp;
	}

	int GetThreadNum()
	{
		return m_uThreadNum;
	}

	const string& GetMysqlIp()
	{
		return m_strMysqlIp;
	}

	int GetMysqlPort()
	{
		return m_uMysqlPort;
	}

	const string& GetMysqlUser()
	{
		return m_strMysqlUser;
	}

	const string& GetMysqlPwd()
	{
		return m_strMysqlPwd;
	}

	const string& GetMysqlDB()
	{
		return m_strMysqlDB;
	}

private:
	ServerConfig() : m_uAreaId(0), m_uListenPort(0)
	{
		memset(m_szBindIp, 0, sizeof(m_szBindIp));
	}

private:
	TiXmlDocument m_xmlDoc;
	uint32_t m_uAreaId;

	uint32_t m_uListenPort;
	char m_szBindIp[32];

	uint32_t m_uThreadNum;

	string m_strMysqlIp;
	uint32_t m_uMysqlPort;
	string m_strMysqlUser;
	string m_strMysqlPwd;
	string m_strMysqlDB;
};

#define gpServerConfig ServerConfig::GetInstance()


#endif