using UnityEngine;
using System.Collections;
using ProtoBuf;
using System.IO;

public class Login {
	private GameClient player = new GameClient();

	public bool connect() {
		return player.Connect ();
	}

	public void do_login(string user_name, string passwd) {
		login.LoginReq login_req = new login.LoginReq ();
		login_req.userName = user_name;
		login_req.passwd = passwd;

		Protocol protocol = new Protocol ();
		protocol.MessageObj2Bytes (login_req, 1000);

		player.Request (1000, protocol.Buffer, protocol.Buffer.Length);
	}

	public void enter_game() {
		client.EnterGameReq enterGameReq = new client.EnterGameReq ();
		Protocol protocol = new Protocol ();
		int bytes_length = protocol.MessageObj2Bytes (enterGameReq, (int)client.ClientProtocol.REQ_ENTER_GAME);
		player.Request ((int)client.ClientProtocol.REQ_ENTER_GAME, protocol.Buffer, bytes_length);
	}

	public void shoot() {
		client.PlaneShootReq shootReq = new client.PlaneShootReq ();
		Protocol protocol = new Protocol ();
		int bytes_length = protocol.MessageObj2Bytes (shootReq, (int)client.ClientProtocol.REQ_PLANE_SHOOT);
		player.Request ((int)client.ClientProtocol.REQ_PLANE_SHOOT, protocol.Buffer, bytes_length);
	}

	public void move(Vector2 newPos, uint newAngle) {
		client.PlaneMoveReq moveReq = new client.PlaneMoveReq ();
		moveReq.newPos = new client.PBVector2D ();
		moveReq.newPos.x = newPos.x;
		moveReq.newPos.y = newPos.y;
		moveReq.angle = (int)newAngle;

		Debug.Log ("move x=" + newPos.x + ", y=" + newPos.y);

		Protocol protocol = new Protocol ();
		int bytes_length = protocol.MessageObj2Bytes (moveReq, (int)client.ClientProtocol.REQ_PLANE_MOVE);
		player.Request ((int)client.ClientProtocol.REQ_PLANE_MOVE, protocol.Buffer, bytes_length);
	}
}
