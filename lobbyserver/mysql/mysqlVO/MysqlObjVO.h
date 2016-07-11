#ifndef LOBBYSERVER_MYSQL_VO_H
#define LOBBYSERVER_MYSQL_VO_H

#include "../../header.h"
#include "../../../utils/Mysql.h"
#include "../../../utils/Utility.h"
#include "../../../protocol/slither_player.pb.h"
#include "../../../protocol/slither_server.pb.h"
#include "../../../protocol/slither_lobby.pb.h"


class PlayerAttentions
{
public:
	static const int STATUS_NORMAL = 0;
	static const int STATUS_DEL = 1;

	PlayerAttentions(){

	}

	PlayerAttentions(uint64_t recocId,uint64_t playerId,uint64_t fansId,int stat){
		id = recocId;
		player_id = playerId;
		fans_id = fansId;
		status = stat;
	}


	PlayerAttentions(uint64_t playerId, uint64_t fansId, int stat){
		player_id = playerId;
		fans_id = fansId;
		status = stat;
	}


	~PlayerAttentions(){

	}


	void setId(uint64_t recordId)
	{
		id = recordId;
	}
	uint64_t getId()
	{
		return id;
	}

	void setPlayerId(uint64_t playerId)
	{
		player_id = playerId;
	}
	uint64_t getPlayerId(){
		return player_id;
	}

	void setFansId(uint64_t fansId)
	{
		fans_id = fansId;
	}
	uint64_t getFansId(){
		return fans_id;
	}


	void setStatus(int stat)
	{
		status = stat;
	}
	int getStatus()
	{
		return status;
	}



private:
	uint64_t id;
	uint64_t player_id;
	uint64_t fans_id;
	int status;

};





















#endif