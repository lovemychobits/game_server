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

	string strLoginSql = string("select password from account where username='") + string(pUserName) + string("'");
	CRecordSet rs;
	if (!m_pSqlConnection->Execute(strLoginSql.c_str(), rs)) {
		ERRORLOG("execute sql=[" << strLoginSql.c_str() << "] failel, error=[" << m_pSqlConnection->GetErrInfo() << "]");
		return false;
	}

	MYSQL_ROW row;
	rs.FetchRow(row);
	if (!row)
	{
		return false;
	}

	// �Ƚ�����
	if (strcmp(pPasswd, row[0]) != 0) {
		return false;
	}

	return true;
}

bool DBMng::Register(const char* pUserName, const char* pPasswd)
{
	if (!m_pSqlConnection) {
		return false;
	}

	// �ж��˺��Ƿ��Ѿ�����
	if (IsUserExist(pUserName)) {
		return false;
	}

	string strRegisterSql = string("insert into account(username, passwor) values('") + string(pUserName) + string("', '") + string(pPasswd) + string("'");
	if (!m_pSqlConnection->ExecuteQuery(strRegisterSql.c_str())) {
		ERRORLOG("execute sql=[" << strRegisterSql.c_str() << "] failel, error=[" << m_pSqlConnection->GetErrInfo() << "]");
		return false;
	}

	return true;
}

bool DBMng::IsUserExist(const char* pUserName)
{
	if (!m_pSqlConnection) {
		return false;
	}

	string strQuerySql = string("select * from account where username='") + string(pUserName) + string("'");
	CRecordSet rs;
	if (!m_pSqlConnection->Execute(strQuerySql.c_str(), rs)) {
		ERRORLOG("execute sql=[" << strQuerySql.c_str() << "] failel, error=[" << m_pSqlConnection->GetErrInfo() << "]");
		return false;
	}

	MYSQL_ROW row;
	rs.FetchRow(row);
	if (!row)					// ������
	{
		return false;
	}
	return true;
}