#!/usr/bin/python
import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#change ip here

host = "192.168.198.130" 
port = 443

def portscanner(port):
    #if there is an error
    if sock.connect_ex((host,port)):
        print(" port %d is closed" % (port))
    else:
        print(" port %d is open" % (port))

portscanner(port)
