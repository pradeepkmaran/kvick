import socket

s = socket.socket()
s.connect(('localhost', 8080)) 
s.sendall(b'SET password new-password\n')
print(s.recv(1024).decode())
s.close()

s = socket.socket()
s.connect(('localhost', 8080)) 
s.sendall(b'GET username \n')
print(s.recv(1024).decode())
s.close()

s = socket.socket()
s.connect(('localhost', 8080)) 
s.sendall(b'GET password \n')
print(s.recv(1024).decode())
s.close()
