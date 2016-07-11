#include "../header.h"
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include "JsonHelper.h"

namespace jsonHelper{
	map<string, string> parseSDKGetInfoResp(const string &jsonStr)
	{
		//std::string str = "{\"code\":0,\"images\":[{\"url\":\"fmn057/20111221/1130/head_kJoO_05d9000251de125c.jpg\"},{\"url\":\"fmn057/20111221/1130/original_kJoO_05d9000251de125c.jpg\"}]}";
		//{"state":0, "message" : "请求成功", "remark" : null, "data" : {"appid":"tid", "passport" : "huwe****.com", "account" : "8", "bindaccount" : "7", "mode" : "1", "point" : "0", "level" : "0"}}
		using namespace boost::property_tree;

		stringstream ss(jsonStr);
		map<string, string> jsonMap;
		if (!jsonStr.empty() && jsonStr.length() > 0)
		{
			ptree pt;
			try{
				read_json(ss, pt);
			}
			catch (ptree_error & e) {
				string errorInfo = "JsonHelper.cpp line 21 read json error json str:[" + jsonStr + "]";
				ERRORLOG(errorInfo.c_str());
			}

			try
			{
				if (!pt.empty() && pt.size() > 0)
				{
					int state = pt.get<int>("state");							// 得到"code"的value 
					jsonMap["state"] = boost::lexical_cast<string>(state);
					if (state == 0){
						ptree dataObj = pt.get_child("data");
						if (!dataObj.empty() && dataObj.size() > 0)
						{
							string accountId = dataObj.get<string>("account");
							jsonMap["account"] = accountId;
						}
					}
				}
			}
			catch (ptree_error & e)
			{
				string errorInfo = "JsonHelper.cpp line 21 read json error json str:[" + jsonStr + "]";
				ERRORLOG(errorInfo.c_str());
			}



			///
		}

		return jsonMap;
	}

























	/////////////////////////////////
	//string asiToUtf(string str)
	//{
	//	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	//	if (len == 0) return str;


	//	vector<wchar_t> unicode(len);
	//	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &unicode[0], len);

	//	wstring wstr = &unicode[0];

	//	int len2 = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	//	if (len2 == 0) return str;

	//	vector<char> utf8(len2);
	//	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8[0], len2, NULL, NULL);

	//	return  &utf8[0];
	//}


}
