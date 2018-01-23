import socket
import threading
from app_globals import logger
from app_globals import tree


class SocketServer(object):
    def __init__(self):
        self.clients = {}
        self.clientsNo = 0
        self.threadLocal = threading.local()
        self.lock = threading.Lock()

        self.transaction = threading.RLock()



    def run(self):
        self.connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connection.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.connection.bind(('0.0.0.0', 1234))
        self.connection.listen(10)
        logger.general("SocketServer listening")

        while True:
            conn, addr = self.connection.accept()
            logger.general("SocketServer connected " + str(addr))

            self.lock.acquire()
            no = self.clientsNo
            self.clientsNo = self.clientsNo +1

            t = threading.Thread(target=self.client_loop, args=(no,))
            self.clients[no] = {'thread':t,
                                 'addr': addr,
                                 'conn':conn,
                                 'no':no}
            self.lock.release()

            t.start()



    def myNo(self):
        return self.threadLocal.myNo



    def processCommand(self, line):
        if(line == "server\1transaction\1start"):
            self.transaction.acquire()
            logger.client('Transaction start>>>>>>')
            self._sendSelf('server\1me\1transaction\1true\2')
            self.broadcast('server\1transaction\1lockedBy\1'+str(self.threadLocal.myNo))
            return True
        if(line == "server\1transaction\1stop"):
            self._sendSelf('server\1me\1transaction\1false\2')
            self.broadcast('server\1transaction\1lockedBy\1') # Clean up
            self.transaction.release()
            logger.client('Transaction stop<<<<<<')
            # If not locked, a runtime error is raised. Let it be so for now
            return True

        return False



    def send(self, idx, text):
        self.lock.acquire()
        self._send(idx, text)
        lock.release()



    def broadcast(self, text):
        self.lock.acquire()
        self._broadcast(text)
        self.lock.release()



    def _sendSelf(self, text):
        self._send(self.threadLocal.myNo, text)



    def _send(self, idx, text):
        logger.client('< ' + text, idx)
        self.clients[idx]['conn'].send(bytes(text+'\2', 'ascii'))



    def _broadcast(self, text):
        print("Broadcast: ", text)

        tree.write(text)
        for c in self.clients.keys():
            self._send(c, text)


    def _initClient(self):
        myNo = self.threadLocal.myNo
        addr = self.clients[myNo]['addr']

        self._sendSelf('server\1me\1no\1'+str(myNo))
        self._sendSelf('server\1me\1addr\1'+str(addr))

        for l in tree.list():
            self._sendSelf(l)

        self._broadcast('server\1clients\1'+str(myNo+1))
        self._broadcast('server\1clients\1'+str(myNo)+'\1addr\1'+str(addr))



    def _handleLine(self, line):
        logger.client('> ' + line)

        if(self.processCommand(line)):
            return

        # If some other thread started a transaction, wait
        self.transaction.acquire()
        self.transaction.release()

        self.broadcast(line)



    def _handleData(self, data):
        data = self.threadLocal.reminder + data
        end = data.find("\2")
        while(end != -1):
            line = data[:end]
            print('+'+line)
            self._handleLine(line)

            data = data[end+1:]
            end = data.find("\2")

        self.threadLocal.reminder = data



    def client_loop(self, no):
        self.threadLocal.myNo = no
        self.threadLocal.reminder = ''

        self._initClient()

        conn = self.clients[no]['conn']
        addr = self.clients[no]['addr']
        while True:
            datar = conn.recv(2048)
            if len(datar) == 0: break # Disconnected

            print("Recv from:", addr, ", data: ", datar)

            self._handleData(datar.decode())

        self.lock.acquire()
        self._closeSelf()
        del self.clients[no]

        self._broadcast('server\1clients\1'+str(self.threadLocal.myNo)+'\1addr\1') # Clean up
        self.lock.release()

        logger.general("SocketServer disconnect " + str(addr))



    def _closeSelf(self):
        self._closeConn(self.threadLocal.myNo)



    def _closeConn(self, no):
        conn = self.clients[no]['conn']

        conn.shutdown(1)
        conn.close()



    def closeAll(self):
        self.lock.acquire()
        for c in self.clients:
            t = self.clients[c]['thread']
            self._closeConn(c)
            self.lock.release()

            t.join()

            self.lock.acquire()
        self.lock.release()


