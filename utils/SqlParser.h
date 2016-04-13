#ifndef UTILS_SQLPARSER_H
#define UTILS_SQLPARSER_H

#include <string>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include "Mysql.h"
using namespace std;

class SqlParser
{
public:
	SqlParser(CMysqlConnection* pConn);
	~SqlParser();

public:
	bool ExecuteSqlFile(const char* pPath);							// 执行sql文件
private:
	bool ReadSqlFile(const char* pPath);							// 读取sql文件
	bool ProcSqls();												// 执行sql文件中的所有sql语句
	FILE* OpenFile(const char* pPath, const char* pOpenMode);		// 打开文件

private:
	vector<string> m_sqlVec;										// 待执行的sql语句
	CMysqlConnection* m_pSqlConnection;								// Mysql连接
};

#endif