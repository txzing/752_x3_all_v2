#!/usr/bin/env python3
# usage:
# python3 udp_client_update.py 192.168.1.10 6789 path_to/BOOT.bin

from socket import *
import time
#import datetime
import sys


s = socket(AF_INET,SOCK_DGRAM)
host =sys.argv[1]
#port = 9999
port=int(sys.argv[2])
buf = 1024
addr = (host,port)

file_name=sys.argv[3]

#s.sendto(file_name,addr)

s.sendto(b"clear",addr)

f=open(file_name,"rb")
data = f.read(buf)
while (data):
    if(s.sendto(data,addr)):
        print("sending ...")
        time.sleep(0.001) # 不行就降低比如0.005
        data = f.read(buf)

print('Successfully send the file')

time.sleep(0.01)        
s.sendto(b"update",addr)

s.close()
print('Connection closed')

f.close()

