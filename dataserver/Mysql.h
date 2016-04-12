#ifndef DATASTORE_MYSQL_H
#define DATASTORE_MYSQL_H

#include "header.h"

#ifdef _MSC_VER
	#include<windows.h>
#endif

#include <mysql.h>
#include <string>
#include <errmsg.h>
using namespace std;

class CRecordSet
{
public:
	CRecordSet() : m_pRes(NULL)
	{

	}
	explicit CRecordSet(MYSQL_RES* pRes)
	{
		m_pRes = pRes;
	}
	MYSQL_RES* MysqlRes()
	{
		return m_pRes;
	}
	~CRecordSet()
	{
		mysql_free_result(m_pRes);
	}
	inline void SetResult(MYSQL_RES* pRes)
	{
		BOOST_ASSERT(m_pRes == NULL);		// 如果此时已经保存了结果集了，那么应该让程序报错，防止内存泄露
		if (m_pRes)					
		{
			WARNLOG("the MYSQL_RES has already stored result, maybe will cause memory leak");
		}
		m_pRes = pRes;
	}
	inline MYSQL_RES* GetResult()
	{
		return m_pRes;
	}
	void FetchRow(MYSQL_ROW& row)
	{
		row = mysql_fetch_row(m_pRes);
	}
private:
	MYSQL_RES* m_pRes;
};

class CMysqlConnection
{
public:
	CMysqlConnection();
	~CMysqlConnection();
	MYSQL* Mysql()
	{
		return m_pMysql;
	}
	bool Init(const char* szHost, int nPort, const char* szUser, const char* szPasswd, const char* szDb);
	bool Execute(const char* szSql);
	bool Execute(const char* szSql, CRecordSet& recordSet);
	int EscapeString(const char* pSrc, int nSrcLen, char* pDest);
	int EscapeString(const string& strSrc, string& strDest);
	void Close();
	const char* GetErrInfo();
	void Reconnect();
private:
	MYSQL* m_pMysql;
};

#endif