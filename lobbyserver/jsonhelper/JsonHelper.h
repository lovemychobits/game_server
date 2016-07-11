#ifndef LOBBYSERVER_JSON_H
#define LOBBYSERVER_JSON_H

//#include "../header.h"
#include <map>
#include <string>
using namespace std;

namespace jsonHelper{
	string asiToUtf(string str);
	map<string,string> parseSDKGetInfoResp(const string &jsonStr);
}
















#endif