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
		m_nListenPort = boost::lexical_cast<int>(pServerPort);
	}

	// listen client info
	TiXmlElement* pClient = pHostServer->FirstChildElement("client");
	if (!pClient)
	{
		return false;
	}
	const char* pClientListenIp = pClient->Attribute("ip");
	const char* pClientListenPort = pClient->Attribute("port");
	if (pClientListenIp)
	{
		memcpy(m_szBindClientIp, pClientListenIp, strlen(pClientListenIp));
	}
	if (pClientListenPort)
	{
		m_nListenClientPort = boost::lexical_cast<int>(pClientListenPort);
	}
	
	return true;
}