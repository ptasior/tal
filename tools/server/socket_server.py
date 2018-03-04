import asyncio
import websockets
from app_globals import logger
from app_globals import tree

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
        self.raw_send = []


    def initData(self):
        self.log('- init data')
        self.send('server\1me\1no\1'+str(self.no))
        self.send('server\1me\1addr\1'+str(self.addr))

        treeList = [i for i in tree.list()] # Make a copy to avoid mutex double locking
        for l in treeList:
            self.send(l)

        self.server.broadcast('server\1clients\1'+str(self.no+1))
        self.server.broadcast('server\1clients\1'+str(self.no)+'\1addr\1'+str(self.addr))


    def sendLockTransaction(self):
        self.send('server\1me\1transaction\1true\2')
        self.server.broadcast('server\1transaction\1lockedBy\1'+str(self.no))
        self.log('- transaction start <<<<<<<<<<<<'+str(self.server.transactionCount))


    def sendReleaseTransaction(self):
        self.send('server\1me\1transaction\1false\2')
        self.server.broadcast('server\1transaction\1lockedBy\1')
        self.log('- transaction stop >>>>>>>>>>>>'+str(self.server.transactionCount))


    def handleLine(self, line):
        self.log('> '+line)

        if self.server.processCommand(line, self):
            return

        self.server.broadcast(line)


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
        self.log('< '+line)
        self.ws_send.append(bytes(line+'\2', 'ascii'))


    def _send_raw(self, line):
        self.log('< '+line)
        self.raw_send.append(bytes(line+'\2', 'ascii'))

        # self.raw_socket.write(bytes(line+'\2', 'ascii'))
        # self.raw_socket.drain()

    def log(self, line):
        logger.client(line, self.no)



class Server(object):
    def __init__(self):
        self.clients = {}
        self.clientsNo = 0
        self.transaction = None
        self.transactionCount = 0;


    def broadcast(self, line):
        for c in self.clients:
            tree.write(line)
            self.clients[c].send(line)


    def add_client(self, client):
        client.no = self.clientsNo
        self.clients[client.no] = client
        self.clientsNo = self.clientsNo + 1

        client.log('Client ' + str(client.no)
                        + ': (' + str(client.prot)
                        + ') '+ str(client.addr))


    def remove_client(self, client):
        del self.clients[client.no]


    def lockTransaction(self, invoker):
        self.transactionCount = self.transactionCount +1

        self.transaction = invoker.no
        invoker.sendLockTransaction()

    def releaseTransaction(self, invoker):
        self.transactionCount = self.transactionCount -1
        if self.transactionCount == 0:
            self.transaction = None
        invoker.sendReleaseTransaction()


    def processCommand(self, line, invoker):
        if(line == "server\1transaction\1start"):
            self.lockTransaction(invoker)
            return True
        if(line == "server\1transaction\1stop"):
            self.releaseTransaction(invoker)
            return True
        if(line == "server\1hello"):
            # invoker.log('- hello received')
            invoker.send(line)
            return True

        return False



    async def loop_raw(self, reader, writer):
        addr = writer.get_extra_info('peername')
        logger.general('RAW conected ' + str(addr))
        c = Client(addr, 'RAW', self)
        c.raw_socket = writer
        self.add_client(c)
        c.initData()

        while(True):
            # Em, a spinlock but in asyncio it just yields
            while self.transaction != None and self.transaction != c.no:
                await asyncio.sleep(0)

            # Yield all messages to web sockets as yielding can be done only here
            for cl in self.clients:
                # if self.clients[cl].prot == 'WS': continue

                for l in self.clients[cl].raw_send:
                    self.clients[cl].raw_socket.write(l)
                    await self.clients[cl].raw_socket.drain()
                    self.clients[cl].log('sent raw'+str(cl))
                self.clients[cl].raw_send = []

            data = await reader.read(1024)
            message = data.decode()
            if len(message) == 0: break;
            c.receive(message)

        logger.general('RAW disconected')
        self.remove_client(c)
        writer.close()



    async def loop_ws(self, websocket, path):
        logger.general('WS connected ' + str(websocket)  +' '+ str(path))
        # print(dir(path))
        c = Client('ws:://'+str(path), 'WS', self)
        c.ws_socket = websocket
        self.add_client(c)

        # JS must talk first
        await websocket.recv()
        c.initData()

        while(True):
            # Em, a spinlock but in asyncio it just yields
            while self.transaction != None and self.transaction != c.no:
                await asyncio.sleep(0)

            # Yield all messages to web sockets as yielding can be done only here
            for cl in self.clients:
                if self.clients[cl].prot == 'RAW': continue

                for l in self.clients[cl].ws_send:
                    await self.clients[cl].ws_socket.send(l)
                    self.clients[cl].log('sent ws'+str(cl))
                self.clients[cl].ws_send = []

            data = await websocket.recv()
            message = data.decode()
            c.receive(message)

        logger.general('WS disconected')
        self.remove_client(c)



    def start(self):
        loop = asyncio.get_event_loop()

        srv_c = asyncio.start_server(self.loop_raw, '0.0.0.0', 1234, loop=loop)
        raw_srv = loop.run_until_complete(srv_c)
        logger.general('Serving on {}'.format(raw_srv.sockets[0].getsockname()))

        ws_c = websockets.serve(self.loop_ws, '0.0.0.0', 1235, subprotocols=['binary'])
        ws_srv = loop.run_until_complete(ws_c)
        logger.general('Serving on {}'.format(ws_srv.sockets[0].getsockname()))

        try: loop.run_forever()
        except KeyboardInterrupt: pass

        # Close the server
        raw_srv.close()
        loop.run_until_complete(raw_srv.wait_closed())
        loop.close()


# s = Server()
# s.start()

