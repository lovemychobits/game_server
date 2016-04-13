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
	bool ExecuteSqlFile(const char* pPath);							// ִ��sql�ļ�
private:
	bool ReadSqlFile(const char* pPath);							// ��ȡsql�ļ�
	bool ProcSqls();												// ִ��sql�ļ��е�����sql���
	FILE* OpenFile(const char* pPath, const char* pOpenMode);		// ���ļ�

private:
	vector<string> m_sqlVec;										// ��ִ�е�sql���
	CMysqlConnection* m_pSqlConnection;								// Mysql����
};

#endif