using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ProtoBuf;
using System.Threading;

public class MsgMng {
	private static MsgMng instance =  new MsgMng();
	public static GameObject obj;
	public static System.Collections.Generic.List<client.PBVector> path = null;
	public static List<GameObject> objList = new List<GameObject>();
	public static CreatePlane mainThread;
	public static MsgMng getInstace() {
		return instance;
	}

	public static void ProcMsg(int command, byte[] buf) {
		mainThread.PushCmd (command, buf);
	}
}