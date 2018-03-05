# Asyncio debugging
# import os
# os.environ['PYTHONASYNCIODEBUG'] = '1'
# import logging, warnings
# logging.basicConfig(level=logging.DEBUG)
# warnings.resetwarnings()

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
        self.has_data = asyncio.Event()


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
        data = self.reminder + data

        end = data.find("\2")
        while(end != -1):
            line = data[:end]

            if len(line) > 0:
                self.handleLine(line)

            data = data[end+1:]
            end = data.find("\2")

        self.reminder = data


    def send(self, line):
        data = bytes(line+'\2', 'ascii')
        if self.prot == 'WS':
            self.ws_send.append(data)
        else:
            self.raw_send.append(data)

        self.log('< '+line)
        self.has_data.set()


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

            data = await cli.raw_reader.read(1024)
            message = data.decode()
            if len(message) == 0: break
            cli.receive(message)


    async def _raw_write(self, cli):
        print('writing raw')
        while(True):
            for l in cli.raw_send:
                cli.raw_writer.write(l)
                await cli.raw_writer.drain()
            cli.raw_send = []

            cli.has_data.clear()
            await cli.has_data.wait()


    async def loop_raw(self, reader, writer):
        addr = writer.get_extra_info('peername')
        logger.general('RAW conected ' + str(addr))
        cli = Client(addr, 'RAW', self)
        cli.raw_reader = reader
        cli.raw_writer = writer
        self.add_client(cli)
        cli.initData()

        read_t = asyncio.ensure_future(self._raw_read(cli))
        write_t = asyncio.ensure_future(self._raw_write(cli))

        await asyncio.gather(read_t)
        write_t.cancel()

        logger.general('RAW disconected')
        self.forceReleaseTransaction(cli)
        self.remove_client(cli)

        writer.close()


    async def _ws_read(self, cli):
        print('reading ws')
        while(True):
            # Em, a spinlock but in asyncio it just yields
            while self.transaction != None and self.transaction != cli.no:
                await asyncio.sleep(0)

            try:
                data = await cli.ws_socket.recv()
            except:
                break
            message = data.decode()
            cli.receive(message)


    async def _ws_write(self, cli):
        print('writing ws')
        while(True):
            for l in cli.ws_send:
                await cli.ws_socket.send(l)
                cli.log('< sent ws')
            cli.ws_send = []

            cli.has_data.clear()
            await cli.has_data.wait()


    async def loop_ws(self, websocket, path):
        logger.general('WS connected ' + str(websocket)  +' '+ str(path))
        # print(dir(path))
        cli = Client('ws:://'+str(path), 'WS', self)
        cli.ws_socket = websocket
        self.add_client(cli)
        cli.initData()

        read_t = asyncio.ensure_future(self._ws_read(cli))
        write_t = asyncio.ensure_future(self._ws_write(cli))

        await asyncio.gather(read_t)
        write_t.cancel()

        logger.general('WS disconected')
        self.forceReleaseTransaction(cli)
        self.remove_client(cli)


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


