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
	const char* pClientServerId = pClient->Attribute("server_id");
	if (pClientListenIp)
	{
		memcpy(m_szBindClientIp, pClientListenIp, strlen(pClientListenIp));
	}
	if (pClientListenPort)
	{
		m_nListenClientPort = boost::lexical_cast<int>(pClientListenPort);
	}
	if (pClientServerId)
	{
		client_serverId = pClientServerId;
	}
	
	TiXmlElement* pMysql = pRoot->FirstChildElement("Mysql");
	if (!pMysql) {
		return false;
	}
	TiXmlElement* pMysqlInfo = pMysql->FirstChildElement("info");
	if (!pMysqlInfo) {
		return false;
	}
	const char* pMysqlIp = pMysqlInfo->Attribute("ip");
	const char* pMysqlPort = pMysqlInfo->Attribute("port");
	const char* pMysqlUser = pMysqlInfo->Attribute("user");
	const char* pMysqlPwd = pMysqlInfo->Attribute("pwd");
	const char* pMysqlDB = pMysqlInfo->Attribute("db");
	if (!pMysqlIp || !pMysqlPort || !pMysqlUser || !pMysqlPwd || !pMysqlDB) {
		return false;
	}
	m_strMysqlIp = pMysqlIp;
	m_uMysqlPort = boost::lexical_cast<uint32_t>(pMysqlPort);
	m_strMysqlUser = pMysqlUser;
	m_strMysqlPwd = pMysqlPwd;
	m_strMysqlDB = pMysqlDB;


	//-------------------------SDK--------------------------//
	TiXmlElement* sdkNodeInfo = pRoot->FirstChildElement("SDK");
	if (!sdkNodeInfo) {
		return false;
	}
	TiXmlElement* sdkAppIdNode = sdkNodeInfo->FirstChildElement("appId");
	if (!sdkAppIdNode) {
		return false;
	}
	const char* appId = sdkAppIdNode->GetText();


	TiXmlElement* sdkAppSecretNode = sdkNodeInfo->FirstChildElement("appSecret");
	if (!sdkAppSecretNode) {
		return false;
	}

	const char* appSecret = sdkAppSecretNode->GetText();


	TiXmlElement* sdkIpNode = sdkNodeInfo->FirstChildElement("sdkIP");
	if (!sdkIpNode) {
		return false;
	}
	const char* sdkIp = sdkIpNode->GetText();

	TiXmlElement* sdkPortNode = sdkNodeInfo->FirstChildElement("sdkPort");
	if (!sdkPortNode) {
		return false;
	}
	const char* sdkPort = sdkPortNode->GetText();

	TiXmlElement* apiURLNode = sdkNodeInfo->FirstChildElement("apiURL");
	if (!apiURLNode) {
		return false;
	}
	const char* apiURL = apiURLNode->GetText();


	//TiXmlElement* verNode = sdkNodeInfo->FirstChildElement("testVer");
	//if (!verNode) {
	//	return false;
	//}
	//const char* ver = verNode->GetText();

	
	SDKInfo info;
	info.sdkAppId = appId;
	info.sdkAppSecret = appSecret;
	info.sdkIp = sdkIp;
	info.sdkPort = sdkPort;
	info.apiURL = apiURL;
	//info.ver = ver;

	sdkInfo = info;


	TiXmlElement* otherNodeInfo = pRoot->FirstChildElement("Other");
	if (!otherNodeInfo) {
		return false;
	}
	TiXmlElement* namePerFixNode = otherNodeInfo->FirstChildElement("playerNamePerfix");
	if (!namePerFixNode) {
		return false;
	}

	palyerNamePerfix = namePerFixNode->GetText();

	//cout << "appId: " << sdkInfo.sdkAppId << "  ,appSecret: " << sdkInfo.sdkAppSecret << " ,  SDKURL: " << sdkInfo.sdkURL << endl;


	return true;
}

