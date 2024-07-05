#!/usr/bin/python
import socket

#Socket info IPv4, TCP and TimeOut
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.setdefaulttimeout(2)

#user inputs
host = raw_input("{*} Enter the host to scan:")
port = int(raw_input("{*} Enter the port to scan: "))

#if there is an error
def portscanner(port):
    if sock.connect_ex((host,port)):
        print(" port %d is closed" % (port))
    else:
        print(" port %d is open" % (port))

#Call the function
portscanner(port)

