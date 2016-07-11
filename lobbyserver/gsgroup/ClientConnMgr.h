#ifndef LOBBYSERVER_CLIENTCONNMGR_H
#define LOBBYSERVER_CLIENTCONNMGR_H

#include "../header.h"



class ClientConnMgr
{
public:
	ClientConnMgr();
	~ClientConnMgr();

	static ClientConnMgr* GetInstance() {
		static ClientConnMgr instance;
		return &instance;
	}

	IConnection* getConnByPlayerId(uint64_t playerId);
	uint64_t getPlayerIdByConn(IConnection* conn);
	void addPlayerIdAndConn(uint64_t playerId, IConnection* conn);
	void removeConn(IConnection* conn);
	vector<IConnection*> getAllOnLineConn();

private:
	map<uint64_t, IConnection*> playerIdConnMap;
	map<IConnection*, uint64_t> connPlayerMap;
};



#define clientConnMgr ClientConnMgr::GetInstance()

#endif

