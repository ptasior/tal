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


    def client(self, message):
        self._lock.acquire()

        tid = self.server.threadId()
        if(tid not in self.clientsLog):
            self.clientsLog[tid] = []

        self.clientsLog[tid].append(message)
        self._lock.release()
        print(tid, message)

    def setServer(self, server):
        self.server = server

