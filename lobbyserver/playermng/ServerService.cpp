#include "ServerService.h"
#include "../gsgroup/ClientConnMgr.h"
#include "../../utils/ProtoTemplate.hpp"
#include "../../utils/Utility.h"
#include <google/protobuf/text_format.h>

void notifyOnLineForBulletin(slither::BulletinInfo& bulletinInfo);


ServerService :: ~ServerService()
{

}



void ServerService::receiveHttpServerBulletin(IConnection* pConn, slither::BulletinInfo& bulletinInfo) 
{
	string strPrint;
	google::protobuf::TextFormat::PrintToString(bulletinInfo, &strPrint);

	cout << "receiveHttpServerBulletin REQ =\n" << strPrint << endl;
	TRACELOG("receiveHttpServerBulletin REQ :[\n" << strPrint.c_str() << "\n");

	uint64_t nowTime = cputil::GetSysTime();
	uint64_t endTime = bulletinInfo.endtime();

	if (endTime >  nowTime)
	{
		//allBulletinMap.insert(pair<string, slither::BulletinInfo>(bulletinInfo.id(), bulletinInfo));
		notifyOnLineForBulletin(bulletinInfo);
		allBulletinMap[bulletinInfo.id()] = bulletinInfo;
	}
	

	slither::BulletinInfoAck bulletinInfoAck;
	bulletinInfoAck.set_errorcode(slither::SERVER_SUCCESS);

	google::protobuf::TextFormat::PrintToString(bulletinInfoAck, &strPrint);

	cout << "receiveHttpServerBulletin RESP proto:" << endl << strPrint << endl;
	TRACELOG("receiveHttpServerBulletin RESP proto:[\n" << strPrint.c_str() << "\n");

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(bulletinInfoAck, strResponse, slither::BULLETIN_FROM_HTTP_SERVER);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}


void ServerService::delHttpServerBulletin(IConnection* pConn, slither::DelBulletinReq& delReqInfo)
{
	string strPrint;
	google::protobuf::TextFormat::PrintToString(delReqInfo, &strPrint);

	cout << "delHttpServerBulletin REQ =\n" << strPrint << endl;
	TRACELOG("delHttpServerBulletin REQ :[\n" << strPrint.c_str() << "\n");

	
	reomveBulletin(delReqInfo.id());

	slither::DelBulletinAck delAck;
	delAck.set_errorcode(slither::SERVER_SUCCESS);

	google::protobuf::TextFormat::PrintToString(delAck, &strPrint);

	cout << "delHttpServerBulletin RESP proto:" << endl << strPrint << endl;
	TRACELOG("delHttpServerBulletin RESP proto:[\n" << strPrint.c_str() << "\n");

	if (pConn) {
		string strResponse;
		cputil::BuildResponseProto(delAck, strResponse, slither::DEL_BULLETIN_FROM_HTTP_SERVER);
		pConn->SendMsg(strResponse.c_str(), strResponse.size());
	}
}






vector<slither::BulletinInfo> ServerService::getNowAllBulletin()
{
	reomveOutTimeBulletin();

	map<string, slither::BulletinInfo> ::iterator it;
	vector<slither::BulletinInfo> nowBulletin;

	for (it = allBulletinMap.begin(); it != allBulletinMap.end(); ++it)
	{
		string id = it->first;
		slither::BulletinInfo bulletin = it->second;
		uint64_t nowTime = cputil::GetSysTime();
		uint64_t endTime = bulletin.endtime();

		if (nowTime < endTime){
			nowBulletin.push_back(bulletin);
		}
	}
	return nowBulletin;
}


void ServerService::reomveBulletin(string delBulletinId)
{
	map<string, slither::BulletinInfo> ::iterator it = allBulletinMap.find(delBulletinId);
	if (it != allBulletinMap.end())
	{
		allBulletinMap.erase(delBulletinId);
	}
}


void ServerService::reomveOutTimeBulletin()
{
	map<string, slither::BulletinInfo> ::iterator it;
	vector<string> needDelIds;
	
	for (it = allBulletinMap.begin(); it != allBulletinMap.end(); ++it)
	{
		string id = it->first;
		slither::BulletinInfo bulletin = it->second;
		uint64_t nowTime = cputil::GetSysTime();
		uint64_t endTime = bulletin.endtime();

		if (nowTime >= endTime){
			needDelIds.push_back(id);
		}
	}

	if (needDelIds.size() > 0)
	{
		int needDelNum = needDelIds.size();
		for (int i = 0; i < needDelNum; i++)
		{
			string needDelId = needDelIds.at(i);
			map<string, slither::BulletinInfo> ::iterator it = allBulletinMap.find(needDelId);
			if (it != allBulletinMap.end())
			{
				allBulletinMap.erase(needDelId);
			}
		}
	}
}


void notifyOnLineForBulletin(slither::BulletinInfo& bulletinInfo)
{
	vector<IConnection*> allOnLineConn = clientConnMgr->getAllOnLineConn();
	int allOnLineNum = allOnLineConn.size();
	if (allOnLineNum > 0){
		for (int i = 0; i < allOnLineNum; i++)
		{
			slither::NTYBestNewBulletin notifyInfo;

			slither::BulletinInfo* bullTemp = notifyInfo.mutable_bulletins();
			bullTemp->CopyFrom(bulletinInfo);

			string strResponse;
			cputil::BuildResponseProto(notifyInfo, strResponse, slither::NTY_BEST_NEW_BULLETIN);

			string strPrint;
			google::protobuf::TextFormat::PrintToString(notifyInfo, &strPrint);

			cout << "notifyOnLineForBulletin RESP =\n" << strPrint << endl;
			TRACELOG("notifyOnLineForBulletin RESP :[\n" << strPrint.c_str() << "\n");

			IConnection* onLineConn = allOnLineConn.at(i);
			if (onLineConn){ 
				cout << "-------NTY_BEST_NEW_BULLETIN---------" << endl;
				onLineConn->SendMsg(strResponse.c_str(), strResponse.size());
			}
		}
	}
}