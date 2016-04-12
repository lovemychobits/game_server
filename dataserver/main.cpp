#include <iostream>
#include "header.h"
#include "DataServer.h"

using namespace std;

Logger g_dsLog = Logger::getInstance(LOG4CPLUS_TEXT("ALL_MSGS"));


int main(int argc, char* argv[])
{
	DataServer dataServer;
	const char* pInitConfPath = "./ds.xml";
	if (argc >= 2)
	{
		pInitConfPath = argv[1];		// 读取配置
	}

	if (!dataServer.Init(pInitConfPath))
	{
		ERRORLOG("dataServer init failed.");
		return 1;
	}
	TRACELOG("dataserver start.");
	cout << "server start success" << endl;
	dataServer.Start();
	TRACELOG("dataserver end.");

	return 0;
}