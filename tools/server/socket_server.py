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
        self.raw_reader = None
        self.raw_writer = None
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
        self.log('< staged '+line)
        self.ws_send.append(bytes(line+'\2', 'ascii'))


    def _send_raw(self, line):
        self.log('< staged '+line)
        self.raw_send.append(bytes(line+'\2', 'ascii'))


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

    def forceReleaseTransaction(self, invoker):
        # Used when client disconnects
        if self.transaction == invoker.no:
            self.transactionCount = 0
            self.transaction = None


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


    async def _raw_read(self, cli):
        print('reading raw')
        while(True):
            # Em, a spinlock but in asyncio it just yields
            while self.transaction != None and self.transaction != cli.no:
                await asyncio.sleep(0)

            print('await reading raw')
            data = await cli.raw_reader.read(1024)
            message = data.decode()
            print('rcd: '+message)
            if len(message) == 0: break
            cli.receive(message)


    async def _raw_write(self, cli):
        print('writing raw')
        while(True):
            for l in cli.raw_send:
                cli.raw_writer.write(l)
                await cli.raw_writer.drain()
                cli.log('< sent raw')
            cli.raw_send = []


    async def loop_raw(self, reader, writer):
        addr = writer.get_extra_info('peername')
        logger.general('RAW conected ' + str(addr))
        c = Client(addr, 'RAW', self)
        c.raw_reader = reader
        c.raw_writer = writer
        self.add_client(c)
        c.initData()

        read_t = asyncio.ensure_future(self._raw_read(c))
        write_t = asyncio.ensure_future(self._raw_write(c))

        await asyncio.gather(read_t, write_t)

        logger.general('RAW disconected')
        self.forceReleaseTransaction(c)
        self.remove_client(c)

        writer.close()
        reader.close()


    async def _ws_read(self, cli):
        print('reading ws')
        while(True):
            # Em, a spinlock but in asyncio it just yields
            while self.transaction != None and self.transaction != cli.no:
                await asyncio.sleep(0)

            data = await cli.ws_socket.recv()
            message = data.decode()
            if len(message) == 0: break
            cli.receive(message)


    async def _ws_write(self, cli):
        print('writing ws')
        while(True):
            for l in cli.ws_send:
                await cli.ws_socket.send(l)
                cli.log('< sent ws')
            cli.ws_send = []
            await asyncio.sleep(0);


    async def loop_ws(self, websocket, path):
        logger.general('WS connected ' + str(websocket)  +' '+ str(path))
        # print(dir(path))
        c = Client('ws:://'+str(path), 'WS', self)
        c.ws_socket = websocket
        self.add_client(c)
        c.initData()

        read_t = asyncio.ensure_future(self._ws_read(c))
        write_t = asyncio.ensure_future(self._ws_write(c))

        await asyncio.gather(read_t, write_t)

        logger.general('WS disconected')
        self.forceReleaseTransaction(c)
        self.remove_client(c)

        websocket.close()


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

