#ifndef LOBBYSERVER_SERVERCONFIG_H
#define LOBBYSERVER_SERVERCONFIG_H

#include "../../tinyxml/tinyxml.h"
#include <boost/lexical_cast.hpp>


struct SDKInfo
{
	string sdkAppId;
	string sdkAppSecret;
	string sdkIp;
	string sdkPort;
	string apiURL;
	//string ver;
};



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

	const string& GetClientServerId() {
		return client_serverId;
	}

	const char* GetClientBindIp()
	{
		return m_szBindClientIp;
	}

	const string& GetMysqlIp() {
		return m_strMysqlIp;
	}

	int GetMysqlPort() {
		return m_uMysqlPort;
	}

	const string& GetMysqlUser() {
		return m_strMysqlUser;
	}

	const string& GetMysqlPwd() {
		return m_strMysqlPwd;
	}

	const string& GetMysqlDB() {
		return m_strMysqlDB;
	}

	const string& GetPlayerNamePerFix() {
		return palyerNamePerfix;
	}



	const SDKInfo& GetSDKInfo(){
		return sdkInfo;
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
	string client_serverId;
	char m_szBindClientIp[32];

	string m_strMysqlIp;
	uint32_t m_uMysqlPort;
	string m_strMysqlUser;
	string m_strMysqlPwd;
	string m_strMysqlDB;

	string palyerNamePerfix;

	SDKInfo sdkInfo;

};

#define gpServerConfig ServerConfig::GetInstance()


#endif