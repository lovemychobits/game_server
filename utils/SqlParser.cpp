#include "SqlParser.h"

SqlParser::SqlParser(CMysqlConnection* pConn) : m_pSqlConnection(pConn)
{
}

SqlParser::~SqlParser()
{
}

bool SqlParser::ExecuteSqlFile(const char* pPath) {
	if (!ReadSqlFile(pPath)) {
		return false;
	}
	if (!ProcSqls()) {
		return false;
	}

	return true;
}

bool SqlParser::ReadSqlFile(const char* pPath)
{
	FILE* file = OpenFile(pPath, "rb");
	if (!file) {
		return false;
	}

	// 获取文件总长度
	long length = 0;
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* pFileBuf = new char[length];
	if (!pFileBuf) {
		return false;
	}

	fread(pFileBuf, length, 1, file);

	int start = 0;
	int end = 0;
	while (end < length) {
		if (pFileBuf[end++] == ';') {		// find sql end
			m_sqlVec.push_back(string(pFileBuf + start, end - start));
			start = end;
		}
	}

	fclose(file);
	return true;
}

bool SqlParser::ProcSqls()
{
	if (!m_pSqlConnection) {
		return false;
	}
	vector<string>::const_iterator sqlIt = m_sqlVec.begin();
	vector<string>::const_iterator sqlItEnd = m_sqlVec.end();
	for (; sqlIt != sqlItEnd; sqlIt++) {
		m_pSqlConnection->ExecuteQuery(sqlIt->c_str());
	}
	return true;
}

FILE* SqlParser::OpenFile(const char* pPath, const char* pOpenMode)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
	FILE* fp = 0;
	errno_t err = fopen_s(&fp, pPath, pOpenMode);
	if (!err && fp)
		return fp;
	return 0;
#else
	return fopen(filename, mode);
#endif
}
