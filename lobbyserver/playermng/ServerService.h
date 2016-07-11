#ifndef LOBBYSERVER_SERVERSERVICE_H
#define LOBBYSERVER_SERVERSERVICE_H

#include "../header.h"
#include "../../protocol/slither_server.pb.h"
#include "../../protocol/slither_lobby.pb.h"

class ServerService
{
public:
	~ServerService();
	static ServerService* GetInstance() {
		static ServerService instance;
		return &instance;
	}

	void receiveHttpServerBulletin(IConnection* pConn, slither::BulletinInfo& bulletinInfo);
	void delHttpServerBulletin(IConnection* pConn, slither::DelBulletinReq& delReqInfo);
	void getBulletin(IConnection* pConn, slither::GetBulletinReq& getBulletionReq);

	vector<slither::BulletinInfo> getNowAllBulletin();

private:
	void reomveBulletin(string bulletinId);
	void reomveOutTimeBulletin();
	map < string, slither::BulletinInfo > allBulletinMap;

};

#define  serverService ServerService::GetInstance()

#endif