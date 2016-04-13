#include "PlayerDataMng.h"
#include "../header.h"
#include "../gsmng/GsConnectionMng.h"
#include "../config/ServerConfig.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

void PlayerDataMng::SetMysqlInfo(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb)
{
	m_strHostIp = szHost;
	m_uPort = nPort;
	m_strUser= szUser;
	m_strPasswd = szPasswd;
	m_strDB = szDb;
}

bool PlayerDataMng::Init()
{
	if (!m_sqlConn.Init(m_strHostIp.c_str(), m_uPort, m_strUser.c_str(), m_strPasswd.c_str(), m_strDB.c_str()))
	{
		ERRORLOG("init mysql connection failed, mysql ip=[" << m_strHostIp.c_str() << "], port=[" << m_uPort << "], user=[" << m_strUser.c_str() << "], pwd=[" << m_strPasswd.c_str() << "], db=[" << m_strDB.c_str() << "]");
		return false;
	}

	if (!InitTables())
	{
		ERRORLOG("init create tables failed.");
		return false;
	}
	m_strAreaId = boost::lexical_cast<string>(gpServerConfig->GetAreaId());
	return true;
}

bool PlayerDataMng::InitTables()
{
	if (!m_sqlTables.CreateTables("sql/create_global_table.sql"))
	{
		ERRORLOG("create table playerinfo failed.");
		return false;
	}
	return true;
}
