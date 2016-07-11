#include "ClientConnMgr.h"
#include "TeamMng.h"


ClientConnMgr :: ClientConnMgr()
{

}


ClientConnMgr :: ~ClientConnMgr()
{

}

IConnection* ClientConnMgr::getConnByPlayerId(uint64_t playerId)
{
	map<uint64_t, IConnection*> ::iterator it = playerIdConnMap.find(playerId);
	if (it != playerIdConnMap.end())
	{
		return it->second;
	}
	return NULL;
}


uint64_t ClientConnMgr::getPlayerIdByConn(IConnection* conn)
{
	map<IConnection*, uint64_t> ::iterator it = connPlayerMap.find(conn);
	if (it != connPlayerMap.end())
	{
		return it->second;
	}
	return 0;
}


void ClientConnMgr::addPlayerIdAndConn(uint64_t playerId, IConnection* conn)
{
	playerIdConnMap[playerId] = conn;
	connPlayerMap[conn] = playerId;
	
	// ²âÊÔÊ¹ÓÃ
	gpTeamMng->AddCanInvitePlayer(playerId);
}


void ClientConnMgr::removeConn(IConnection* conn)
{
	uint64_t palyerId = getPlayerIdByConn(conn);
	playerIdConnMap.erase(palyerId);
	connPlayerMap.erase(conn);
}


vector<IConnection*> ClientConnMgr::getAllOnLineConn()
{
	vector<IConnection*> allOnLineConn;
	map<IConnection*, uint64_t>::iterator it;
	for (it = connPlayerMap.begin(); it != connPlayerMap.end(); ++it)
	{
		allOnLineConn.push_back(it->first);
	}
	return allOnLineConn;
}