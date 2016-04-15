import socket
import time
import struct
import ClientProtocol

class ClientSimulator :
	def __init__(self, username, passwd):
		self.username = username
		self.passwd = passwd

	def connect_login_server(self, ip, port):
		self.login_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
		self.login_sock.connect((ip, port))
		self.login_ip = ip
		self.login_port = port
		return

	def login(self):
		loginAck = ClientProtocol.login_req(self.login_sock, self.username, self.passwd)
		if loginAck.errorCode != 0:
			print "login failed, errorCode=" + loginAck.errorCode
			return
		else:
			self.token = loginAck.token


if __name__ == "__main__":
    robot = ClientSimulator("www", "111111")
    robot.connect_login_server("127.0.0.1", 8700)
    robot.login()
    robot.do_loop()