#include "header.h"
#include "Mysql.h"
#include <string.h>

//////////////////////////////////MySqlConnection/////////////////////////////

CMysqlConnection::CMysqlConnection() : m_pMysql(NULL)
{

}

CMysqlConnection::~CMysqlConnection()
{
	if (m_pMysql)
	{
		mysql_close(m_pMysql);	
	}
	return;
}

bool CMysqlConnection::Init(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb)
{
	if ((m_pMysql = mysql_init(m_pMysql)) == NULL)
	{
		return false;
	}
	
	// 设置自动重连
	char cAuto = 1;
	if (!mysql_options(m_pMysql, MYSQL_OPT_RECONNECT, &cAuto))
	{
		TRACELOG("mysql_options MYSQL_OPT_RECONNECT failed.");
	}

	// 设置超时时间
	unsigned int uTimeout = 3;
	if (!mysql_options(m_pMysql, MYSQL_OPT_CONNECT_TIMEOUT, &uTimeout))
	{
		TRACELOG("mysql_options MYSQL_OPT_CONNECT_TIMEOUT failed.");
	}

	if (mysql_real_connect(m_pMysql, szHost, szUser, szPasswd, szDb, nPort, NULL, 0) == NULL)
	{
		return false;
	}
	return true;
}

bool CMysqlConnection::Execute(const char* szSql)
{
	if (mysql_real_query(m_pMysql, szSql, strlen(szSql)) != 0)
	{
		if (mysql_errno(m_pMysql) == CR_SERVER_GONE_ERROR)
		{
			Reconnect();
		}
		return false;
	}
	return true;
}

bool CMysqlConnection::Execute(const char* szSql, CRecordSet& recordSet)
{
	if (mysql_real_query(m_pMysql, szSql, strlen(szSql)) != 0)
	{
		if (mysql_errno(m_pMysql) == CR_SERVER_GONE_ERROR)
		{
			Reconnect();
		}
		return false;
	}
	MYSQL_RES* pRes = mysql_store_result(m_pMysql);
	if (!pRes)
	{
		return NULL;
	}
	recordSet.SetResult(pRes);
	return true;
}

int CMysqlConnection::EscapeString(const char* pSrc, int nSrcLen, char* pDest)
{
	if (!m_pMysql)
	{
		return 0;
	}
	return mysql_real_escape_string(m_pMysql, pDest, pSrc, nSrcLen);
}

int CMysqlConnection::EscapeString(const string& strSrc, string& strDest)
{
	if (!m_pMysql)
	{
		return 0;
	}
	char szDest[2048] = {0};
	int nRet = mysql_real_escape_string(m_pMysql, szDest, strSrc.c_str(), strSrc.size());
	if (nRet <= 0)
	{
		return nRet;
	}
	strDest = string(szDest);
	return nRet;
}

void CMysqlConnection::Close()
{
	mysql_close(m_pMysql);
}

const char* CMysqlConnection::GetErrInfo()
{
	return mysql_error(m_pMysql);
}

void CMysqlConnection::Reconnect()
{
	mysql_ping(m_pMysql);
}

//////////////////////////////////MySqlConnection END/////////////////////////////