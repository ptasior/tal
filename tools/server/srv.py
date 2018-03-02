#!/usr/bin/env python3

from pprint import pprint
from threading import Thread


import app_globals
from web_server import WebServer
from socket_server import Server

# app_globals.init()

web = WebServer()
web_server = Thread(target = web.run)
web_server.setDaemon(True)
web_server.start()

socket_server = Server()
socket_server.start()

