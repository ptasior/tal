#!/usr/bin/env python3

# No security whatsoever.
# You are playing with friends, if they cheat, the only reasonable solution is
#  to change friends. It will solve a bunch of other problems as well and
#  will make you a happier person in long run.

from bottle import *
import socket
import threading
from pprint import pprint


connections = {}
treeData = {'nodes':{}, 'value':''}
threadLocal = threading.local()
lock = threading.Lock()
transaction = threading.RLock()


# TODO Protect connections and treeData with mutex!!!


def init(conn, addr):
    lock.acquire()
    connections[addr] = conn

    myNo = len(connections)-1
    threadLocal.myNo = myNo
    threadLocal.myAddr = addr

    send = []
    for l in tree_list('', treeData):
        send.append(bytes(l, 'ascii'))

    lock.release()

    conn.send(bytes('server\1me\1no\1'+str(myNo)+'\2', 'ascii'))
    conn.send(bytes('server\1me\1addr\1'+str(addr)+'\2', 'ascii'))

    for l in send:
        conn.send(l)

    broadcast('server\1clients\1'+str(myNo+1))
    broadcast('server\1clients\1'+str(myNo)+'\1addr\1'+str(addr))



def tree_write(l, v, ptr):
    # TODOAssert if mutex locked
    if l:
        h = l[0]
        if h not in ptr['nodes'].keys():
            ptr['nodes'][h] = {'nodes':{}, 'value':''}
        tree_write(l[1:], v, ptr['nodes'][h])
    else: ptr['value'] = v



def tree_list(prefix, ptr):
    # TODO Assert if mutex locked
    if prefix: p = prefix + '\1' # Skips empty root
    else: p = ''

    if ptr['value'] != '':
        yield p+ptr['value']+'\2'

    for n in ptr['nodes'].keys():
        yield from tree_list(p+n, ptr['nodes'][n])



def processCommand(line):
    if(line == "server\1transaction\1start"):
        transaction.acquire()
        print('Transaction start>>>>>>')
        connections[threadLocal.myAddr].send(bytes('server\1me\1transaction\1true\2', 'ascii'))
        broadcast('server\1transaction\1lockedBy\1'+str(threadLocal.myNo))
        return True
    if(line == "server\1transaction\1stop"):
        connections[threadLocal.myAddr].send(bytes('server\1me\1transaction\1false\2', 'ascii'))
        broadcast('server\1transaction\1lockedBy\1') # Clean up
        transaction.release()
        print('Transaction stop<<<<<<')
        # If not locked, a runtime error is raised. Let it be so for now
        return True

    return False



def broadcast(text):
    print("Broadcast: ", text)
    l = text.split('\1')
    val = l.pop()

    lock.acquire()
    tree_write(l, val, treeData)

    for c in connections:
        if connections[c]:
            connections[c].send(bytes(text+'\2', 'ascii'))
            # TODO Check if send should be moved out of lock
    lock.release()



def handleLine(line):
    if(processCommand(line)):
        return

    # If some other thread started a transaction, wait
    transaction.acquire()
    transaction.release()

    broadcast(line)



def handleData(data):
    data = threadLocal.reminder + data
    end = data.find("\2")
    while(end != -1):
        line = data[:end]
        print('+'+line)
        handleLine(line)

        data = data[end+1:]
        end = data.find("\2")

    threadLocal.reminder = data



def client_loop(conn, addr):
    threadLocal.reminder = ''

    init(conn, addr)

    while True:
        datar = conn.recv(2048)
        if len(datar) == 0: break # Disconnected

        print("Recv from:", addr, ", data: ", datar)

        handleData(datar.decode())

    conn.shutdown(1)
    conn.close()

    lock.acquire()
    connections[addr] = None
    lock.release()

    broadcast('server\1clients\1'+str(threadLocal.myNo)+'\1addr\1') # Clean up
    print("Disconnect", addr)



def main():
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    connection.bind(('0.0.0.0', 1234))
    connection.listen(10)
    print("Listening")
    while True:
        conn, addr = connection.accept()
        print("Connected", addr)
        threading.Thread(target=client_loop, args=(conn, addr)).start()



main()

