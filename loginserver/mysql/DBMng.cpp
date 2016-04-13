#include "../header.h"
#include "DBMng.h"
#include "../config/ServerConfig.h"

DBMng::DBMng() : m_pSqlConnection(NULL)
{
	m_pSqlConnection = new CMysqlConnection();
}

DBMng::~DBMng()
{
}

bool DBMng::init() 
{
	if (!m_pSqlConnection) {
		return false;
	}

	if (!m_pSqlConnection->Init(gpServerConfig->GetMysqlIp().c_str(), gpServerConfig->GetMysqlPort(), gpServerConfig->GetMysqlUser().c_str(),
		gpServerConfig->GetMysqlPwd().c_str(), gpServerConfig->GetMysqlDB().c_str()))
	{
		ERRORLOG("init mysql connection failed");
		return false;
	}
	return true;
}

bool DBMng::CheckLogin(const char* pUserName, const char* pPasswd) 
{
	if (!m_pSqlConnection) {
		return false;
	}

	string strLoginSql = string("select passwd from account where username='") + string(pUserName) + string("'");
	CRecordSet rs;
	if (!m_pSqlConnection->Execute(strLoginSql.c_str(), rs)) {
		return false;
	}

	MYSQL_ROW row;
	rs.FetchRow(row);
	if (!row)
	{
		return false;
	}

	// ±»Ωœ√‹¬Î
	if (strcmp(pPasswd, row[0]) != 0) {
		return false;
	}

	return true;
}