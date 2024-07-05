#!/usr/bin/python

import socket
from termcolor import colored

#Socket info IPv4, TCP and TimeOut
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.setdefaulttimeout(1)

#user inputs
host = input("{*}  Enter the host to scan:")

#if there is an error
def portscanner(port):
    if sock.connect_ex((host, port)):
        print(colored("[!!] port %d is closed" % (port), 'red'))
    else:
        print(colored("[!!] port %d is open" % (port), 'green'))


#Call the function for ea. individual port
for port in range(1,1000):
    portscanner(port)