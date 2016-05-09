using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using client;

class Msg {
	public int command;
	public byte[] data;
}

public class CreatePlane : MonoBehaviour {

	public GameObject obj = null;
	public GameObject bulletPrefab = null;
	public GameObject planePrefab = null;
	public float step = 0.2f;
	private List<GameObject> objList = new List<GameObject>();
	Login log = new Login();
	private List<Msg> msgList = new List<Msg>();
	private List<GameObject> bulletList = new List<GameObject>();
	private List<GameObject> planeList = new List<GameObject>();
	private Dictionary<uint, GameObject> objTable = new Dictionary<uint, GameObject>();

	// Use this for initialization
	void Start () {
		log.connect ();
		MsgMng.obj = obj;
		MsgMng.mainThread = this;

		log.enter_game ();
	}
	
	// Update is called once per frame
	void Update () {
		lock (msgList) {
			if (msgList.Count > 0) {
				var tmpList = new List<Msg>(msgList);
				foreach (Msg msg in tmpList) {
					switch (msg.command) {
					case (int)client.ClientProtocol.REQ_ENTER_GAME: {
						Debug.Log("REQ_ENTER_GAME");

						System.IO.MemoryStream stream = new System.IO.MemoryStream(msg.data);
						client.EnterGameAck enterGameAck = ProtoBuf.Serializer.Deserialize<client.EnterGameAck>(stream);

						Vector2 enterPos = new Vector2(enterGameAck.pos.x, enterGameAck.pos.y);
						obj.transform.position = enterPos;
						msgList.Remove(msg);

					} break;

					case (int)client.ClientProtocol.NTY_OBJS: {
						Debug.Log("NTY_OBJS");

						System.IO.MemoryStream stream = new System.IO.MemoryStream(msg.data);
						client.NotifyObjs objsNty = ProtoBuf.Serializer.Deserialize<client.NotifyObjs>(stream);

						foreach (client.PBObject pbObj in objsNty.objList) {
							GameObject tmp = null;

							if (objTable.ContainsKey(pbObj.objId)) {
								tmp = objTable[pbObj.objId];
							}
							else {
								if (pbObj.type == client.ObjectType.PLANE) {
									tmp = Instantiate(planePrefab) as GameObject;
								}
								else if (pbObj.type == client.ObjectType.BULLET) {
									tmp = Instantiate(bulletPrefab) as GameObject;
								}
								objTable.Add(pbObj.objId, tmp);
								planeList.Add(tmp);
							}

							Vector2 planePos = new Vector2(pbObj.pos.x, pbObj.pos.y);
							Debug.Log ("new plane x=" + pbObj.pos.x + ", y=" + pbObj.pos.y);
							tmp.transform.position = planePos;
						}
						msgList.Remove(msg);
					} break;
					}
				}
			}
		}

		/*foreach (GameObject bullet in bulletList) {
			Vector2 pos = bullet.transform.position;
			pos.y += 1;
			bullet.transform.position = pos;
		}*/


		if (Input.GetKeyDown (KeyCode.Space)) {
			/*GameObject new_obj = Instantiate(obj) as GameObject;
			new_obj.transform.position = obj.transform.position;

			Vector2 pos = obj.transform.position;
			pos.x += 1;
			pos.y += 2;

			new_obj.transform.position = pos;

			objList.Add(new_obj);*/
			Vector3 startPos = new Vector3(1, 1, 1);
			Vector3 endPos = new Vector3(1, 1, 1);
		}

		bool bMoved = false;
		if (Input.GetKey (KeyCode.A)) {
			Vector2 pos = obj.transform.position;
			pos.x -= step;
			obj.transform.position = pos;
			bMoved = true;
		} else if (Input.GetKey (KeyCode.D)) {
			Vector2 pos = obj.transform.position;
			pos.x += step;
			obj.transform.position = pos;
			bMoved = true;
		} else if (Input.GetKey (KeyCode.W)) {
			Vector2 pos = obj.transform.position;
			pos.y += step;
			obj.transform.position = pos;
			bMoved = true;
		} else if (Input.GetKey (KeyCode.S)) {
			Vector2 pos = obj.transform.position;
			pos.y -= step;
			obj.transform.position = pos;
			bMoved = true;
		}

		if (bMoved) {
			log.move (obj.transform.position, 0);
		}

		if (Input.GetKeyDown (KeyCode.J)) {
			GameObject bullet = Instantiate(bulletPrefab) as GameObject;
			Vector2 pos = obj.transform.position;
			bullet.transform.position = pos;
			bullet.transform.eulerAngles = obj.transform.eulerAngles;
			bulletList.Add (bullet);

			log.shoot();
		}
		if (Input.GetKeyDown (KeyCode.K)) {
			// 每次旋转10度
			float zRotation = 10.0f;
			Vector3 eulerAngles = obj.transform.eulerAngles;
			eulerAngles.z += zRotation;
			obj.transform.eulerAngles = eulerAngles;

			log.move (obj.transform.position, (uint)eulerAngles.z);
		}
	}

	public void PushCmd(int cmd, byte[] data) {
		Msg msg = new Msg ();
		msg.command = cmd;
		msg.data = data;
		lock (msgList) {
			Debug.Log("add msg command=" + cmd);
			msgList.Add(msg);
		}
	}
}
