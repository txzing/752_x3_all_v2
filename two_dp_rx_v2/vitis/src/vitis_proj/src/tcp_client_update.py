#!/usr/bin/env python3

# usage:
# python3 tcp_client_update.py 192.168.1.10 6789 path_to/BOOT.bin

import socket                   # Import socket module
import time
import sys

s = socket.socket()             # Create a socket object
#host = socket.gethostname()     # Get local machine name
host = sys.argv[1]
#print (sys.argv[1])
#port = 60000                    # Reserve a port for your service.
port=int(sys.argv[2])
buf = 1024
addr = (host,port)
file_name=sys.argv[3]

s.connect((host, port))

#s.send("Hello server!") # for python2
#s.send(b"Hello server!") # for python3
#trigger = 'Hello server!'
#s.send(trigger.encode()) # for python3
#s.send('Hello server!'.encode())

'''
with open('received_file', 'wb') as f:
    print ('file opened')
    while True:
        print('receiving data...')
        data = s.recv(1024)
        print('data=%s', (data))
        if not data:
            break
        # write data to a file
        f.write(data)
'''
'''
    f = open(filename,'rb')
    l = f.read(1024)
    while (l):
       conn.send(l)
       print('Sent ',repr(l))
       l = f.read(1024)
    f.close()
'''
#s.send('clear'.encode())
#s.sendto(b"clear", addr)
s.sendto('clear'.encode(), addr)
f=open(file_name,"rb")
data = f.read(buf)
while (data):
    if(s.sendto(data,addr)):
        print("sending ...")
        time.sleep(0.001) # 不行就降低比如0.005
        data = f.read(buf)

print('Successfully send the file')

time.sleep(0.1)        
#s.sendto(b"update", addr)
s.sendto('update'.encode(), addr)
#s.send('update'.encode())

s.close()
print('Connection closed')

f.close()



