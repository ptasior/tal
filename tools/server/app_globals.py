logger = None
tree = None
server = None
web = None

def init():
    global logger
    global tree
    global server
    global web

    from logger import Logger
    logger = Logger()

    from tree import Tree
    tree = Tree()

    from socket_server import SocketServer
    server = SocketServer()

    logger.setServer(server)

    from web_server import WebServer
    web = WebServer()

