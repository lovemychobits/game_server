#include "../../network/header.h"
#include "ServerConfig.h"

bool ServerConfig::LoadServerConf(const char* pFilePath)
{
	if (!m_xmlDoc.LoadFile(pFilePath))
	{
		return false;
	}

	TiXmlElement* pRoot = m_xmlDoc.RootElement();
	if (!pRoot)
	{
		return false;
	}

	TiXmlElement* pHostServer = pRoot->FirstChildElement("HostServer");
	if (!pHostServer)
	{
		cout << "cannot find host server" << endl;
		return false;
	}
	m_uAreaId = boost::lexical_cast<uint32_t>(pHostServer->Attribute("id"));

	TiXmlElement* pServer = pHostServer->FirstChildElement("bind");
	if (!pServer)
	{
		return false;
	}
	const char* pServerIp = pServer->Attribute("ip");
	const char* pServerPort = pServer->Attribute("port");
	if (pServerIp)
	{
		memcpy(m_szBindIp, pServerIp, strlen(pServerIp));
	}
	if (pServerPort)
	{
		m_uListenPort = boost::lexical_cast<uint32_t>(pServerPort);
	}

	TiXmlElement* pWorkThread = pRoot->FirstChildElement("WorkThread");
	if (!pWorkThread)
	{
		return false;
	}
	TiXmlElement* pThread = pWorkThread->FirstChildElement("thread");
	if (!pThread)
	{
		return false;
	}
	m_uThreadNum = boost::lexical_cast<uint32_t>(pThread->Attribute("num"));

	TiXmlElement* pMysql = pRoot->FirstChildElement("Mysql");
	if (!pMysql)
	{
		return false;
	}
	TiXmlElement* pMysqlInfo = pMysql->FirstChildElement("info");
	if (!pMysqlInfo)
	{
		return false;
	}
	const char* pMysqlIp = pMysqlInfo->Attribute("ip");
	const char* pMysqlPort = pMysqlInfo->Attribute("port");
	const char* pMysqlUser = pMysqlInfo->Attribute("user");
	const char* pMysqlPwd = pMysqlInfo->Attribute("pwd");
	const char* pMysqlDB = pMysqlInfo->Attribute("db");
	if (!pMysqlIp || !pMysqlPort || !pMysqlUser || !pMysqlPwd || !pMysqlDB)
	{
		return false;
	}
	m_strMysqlIp = pMysqlIp;
	m_uMysqlPort = boost::lexical_cast<uint32_t>(pMysqlPort);
	m_strMysqlUser = pMysqlUser;
	m_strMysqlPwd = pMysqlPwd;
	m_strMysqlDB = pMysqlDB;

	return true;
}