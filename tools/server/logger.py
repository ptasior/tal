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


    def client(self, message, idx):
        self._lock.acquire()

        if(idx not in self.clientsLog):
            self.clientsLog[idx] = []

        self.clientsLog[idx].append(message)
        self._lock.release()
        print(idx, message)


