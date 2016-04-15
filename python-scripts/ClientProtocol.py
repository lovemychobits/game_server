import socket
import struct
import login_pb2

def recv_full(sock):
	headLen = 0
	headBuf = sock.recv(12)
	eadLen = len(headBuf)
	    
	while headLen < 12: 
	    leftBuf = sock.recv(12)
	    headLen = headLen + len(leftBuf)
	    headBuf = headBuf + leftBuf
	    
	bodySize, cmdMsg, magicCode = struct.unpack("iiI", headBuf)
	if bodySize == headLen:
	    return headBuf
	    
	bodySize = bodySize - headLen
	bodyBuf = sock.recv(bodySize)
	bodyLen = len(bodyBuf)
	while bodyLen < bodySize:
	    leftBuf = sock.recv(bodySize - bodyLen)
	    bodyLen = bodyLen + len(leftBuf)
	    bodyBuf = bodyBuf + leftBuf
	    
	return headBuf + bodyBuf

########################################################################login########################################################################
def login_req(sock, username, passwd):
	loginReq = login_pb2.LoginReq()
	loginReq.userName = username
	loginReq.passwd = passwd
	body = loginReq.SerializeToString()
	header = struct.pack('iiI', 12+len(body), 1000, 0xA1B2C3D4)
	sock.send(header+body)
	    
	#pbstr = sock.recv(1024)
	recv_buf = recv_full(sock)
	loginAck = login_pb2.LoginAck()
	try:
		loginAck.ParseFromString(str(pbstr[12:]))           
	except Exception as e:
		print e
	return loginAck	