#!/usr/bin/env python3

# No security whatsoever.
# You are playing with friends, if they cheat, the only reasonable solution is
#  to change friends. It will solve a bunch of other problems as well and
#  will make you a happier person in long run.

from pprint import pprint
# import threading

import app_globals

app_globals.init()

from tree import Tree
tree = Tree()


import asyncio
import websockets


# app_globals.server.run()
# threading.Thread(target=app_globals.server.run).start()
# app_globals.web.run()


class Client(object):
    def __init__(self, address, prot, server):
        self.addr = address
        self.prot = prot
        self.server = server
        self.no = None
        self.raw_socket = None
        self.ws_socket = None
        self.reminder = ''
        self.ws_send = []
        Client.invoker = None


    def initData(self):
        print('init data', self.no)
        self.send('server\1me\1no\1'+str(self.no))
        self.send('server\1me\1addr\1'+str(self.addr))

        for l in tree.list():
            self.send(l)

        self.server.broadcast('server\1clients\1'+str(self.no+1))
        self.server.broadcast('server\1clients\1'+str(self.no)+'\1addr\1'+str(self.addr))


    def handleLine(self, line):
        print("Line:", line, self.no)
        Client.invoker = self

        if self.server.processCommand(line):
            return

        self.server.broadcast(line)

        Client.invoker = None


    def receive(self, data):
        # print("Received %r" % (data))

        data = self.reminder + data

        end = data.find("\2")
        while(end != -1):
            line = data[:end]
            # print('+'+line)

            if len(line) > 0:
                self.handleLine(line)

            data = data[end+1:]
            end = data.find("\2")

        self.reminder = data



    def send(self, line):
        if self.prot == 'WS': self._send_ws(line)
        else:                 self._send_raw(line)


    def _send_ws(self, line):
        print("Send WS: %r" % line, self.no)
        self.ws_send.append(bytes(line+'\2', 'ascii'))
        # self.ws_socket.send(bytes(line+'\2', 'ascii'))


    def _send_raw(self, line):
        print("Send RAW: %r" % line, self.no)

        self.raw_socket.write(bytes(line+'\2', 'ascii'))
        # self.raw_socket.drain()




class Server(object):
    def __init__(self):
        self.clients = {}
        self.clientsNo = 0
        self.transaction = None


    def broadcast(self, line):
        for c in self.clients:
            self.clients[c].send(line)


    def add_client(self, client):
        client.no = self.clientsNo
        self.clients[client.no] = client
        self.clientsNo = self.clientsNo + 1


    def remove_client(self, client):
        del self.clients[client.no]


    def lockTransaction(self):
        # logger.client('Transaction start>>>>>>')
        self.transaction = Client.invoker.no

        Client.invoker.send('server\1me\1transaction\1true\2')
        self.broadcast('server\1transaction\1lockedBy\1'+str(Client.invoker.no))
        print('transaction start <<<<<<<<<<<<', str(Client.invoker.no))


    def releaseTransaction(self):
        self.transaction = None

        Client.invoker.send('server\1me\1transaction\1false\2')
        self.broadcast('server\1transaction\1lockedBy\1') # Clean up
        print('transaction stop >>>>>>>>>>>>', str(Client.invoker.no))
        # logger.client('Transaction stop<<<<<<', self.no)


    def processCommand(self, line):
        if(line == "server\1transaction\1start"):
            self.lockTransaction()
            return True
        if(line == "server\1transaction\1stop"):
            self.releaseTransaction()
            return True

        return False



    async def loop_raw(self, reader, writer):
        addr = writer.get_extra_info('peername')
        print('RAW conected', addr)
        c = Client(addr, 'RAW', self)
        c.raw_socket = writer
        self.add_client(c)
        c.initData()

        while(True):
            # Em, a spinlock. Yeah, I know. Whatever...
            while self.transaction != None and self.transaction != c.no:
                await asyncio.sleep(0)
                print('awa tra:', self.transaction, c.no)

            print('tra:', self.transaction, c.no)

            data = await reader.read(1024)
            message = data.decode()
            if len(message) == 0: break;
            c.receive(message)

            # Yield all messages to web sockets as it can be done only here
            for cl in self.clients:
                for l in self.clients[cl].ws_send:
                    await self.clients[cl].ws_socket.send(l)

        print('RAW disconected')
        self.remove_client(c)
        writer.close()



    async def loop_ws(self, websocket, path):
        print('WS connected', path)
        c = Client(path, 'WS', self)
        c.ws_socket = websocket
        self.add_client(c)
        c.initData()

        while(True):
            data = await websocket.recv()
            message = data.decode()
            c.receive(message)

            # Yield all messages to web sockets as it can be done only here
            for c in self.clients:
                for l in self.clients[c].ws_send:
                    await self.clients[c].ws_socket.send(l)

        print('WS disconected')
        self.remove_client(c)



    def start(self):
        loop = asyncio.get_event_loop()

        srv_c = asyncio.start_server(self.loop_raw, '0.0.0.0', 1234, loop=loop)
        raw_srv = loop.run_until_complete(srv_c)
        print('Serving on {}'.format(raw_srv.sockets[0].getsockname()))

        ws_c = websockets.serve(self.loop_ws, '0.0.0.0', 1235, subprotocols=['binary'])
        ws_srv = loop.run_until_complete(ws_c)
        print('Serving on {}'.format(ws_srv.sockets[0].getsockname()))

        try: loop.run_forever()
        except KeyboardInterrupt: pass

        # Close the server
        raw_srv.close()
        loop.run_until_complete(raw_srv.wait_closed())
        loop.close()


s = Server()
s.start()

