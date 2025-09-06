import socket

s = socket.socket()
s.connect(('192.168.122.1', 8080)) 
s.sendall(b'SET username Pradeep\n')
print(s.recv(1024).decode())
s.close()

s = socket.socket()
s.connect(('192.168.122.1', 8080)) 
s.sendall(b'GET username\n')
print(s.recv(1024).decode())
s.close()
