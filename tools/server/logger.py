import threading

class Logger(object):
    def __init__(self):
        self._lock = threading.Lock()
        self.globalLog = []
        self.clientsLog = {}


    def general(self, message):
        self._lock.acquire()
        self.globalLog.append(message)
        self._lock.release()

        print('*', message)


    def client(self, message, idx=None):
        self._lock.acquire()

        if idx == None:
            cid = self.server.myNo()
            print('idx myno = ', cid)
        else:
            cid = idx
            print('idx = ', cid)
        if(cid not in self.clientsLog):
            self.clientsLog[cid] = []

        self.clientsLog[cid].append(message)
        self._lock.release()
        print(cid, message)

    def setServer(self, server):
        self.server = server

