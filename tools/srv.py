#!/usr/bin/env python3

# No security whatsoever.
# You are playing with friends, if they cheat, the only reasonable solution is
#  to change friends. It will solve a bunch of other problems as well and
#  will make you a happier person in long run.

import socket
import threading
from pprint import pprint


connections = {}
data = {'nodes':{}, 'value':''}


# TODO Protect connections and data with mutex!!!


def init(conn, addr):
    connections[addr] = conn

    myNo = len(connections)-1
    conn.send(bytes('server\1me\1no\1'+str(myNo)+'\2', 'ascii'))
    conn.send(bytes('server\1me\1addr\1'+str(addr)+'\2', 'ascii'))

    for l in tree_list('', data):
        conn.send(bytes(l, 'ascii'))

    broadcast('server\1clients\1'+str(myNo+1))
    broadcast('server\1clients\1'+str(myNo)+'\1addr\1'+str(addr))

    return myNo



def tree_write(l, v, ptr):
    if l:
        h = l[0]
        if h not in ptr['nodes'].keys():
            ptr['nodes'][h] = {'nodes':{}, 'value':''}
        tree_write(l[1:], v, ptr['nodes'][h])
    else: ptr['value'] = v



def tree_list(prefix, ptr):
    if prefix: p = prefix + '\1' # Skips empty root
    else: p = ''

    if ptr['value'] != '':
        yield p+ptr['value']+'\2'

    for n in ptr['nodes'].keys():
        yield from tree_list(p+n, ptr['nodes'][n])



def broadcast(text):
    l = text.split('\1')
    val = l.pop()
    tree_write(l, val, data)

    for c in connections:
        if connections[c]:
            connections[c].send(bytes(text+'\2', 'ascii'))



def client_loop(conn, addr):
    myNo = init(conn, addr)

    while True:
        datar = conn.recv(2048)
        if len(datar) == 0: break # Disconnected

        print("Recv from:", addr, ", data: ", datar, " len:", len(datar))

        broadcast(datar.decode())

    conn.shutdown(1)
    conn.close()
    connections[addr] = None
    broadcast('server\1clients\1'+str(myNo)+'\1addr\1')
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

