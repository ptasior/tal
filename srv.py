#!/usr/bin/env python3
import socket

resp = "HTTP/1.1 101 Switching Protocols\r\n" + \
"Upgrade: websocket\r\n" + \
"Connection: Upgrade\r\n" + \
"Sec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=\r\n" + \
"Sec-WebSocket-Protocol: chat\r\n\r\n"

connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
connection.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
connection.bind(('0.0.0.0', 1234))
connection.listen(10)
while True:
    current_connection, address = connection.accept()
    while True:
        data = current_connection.recv(2048)

        # print "recv:"
        # print data
        # current_connection.send(resp)

        # data = current_connection.recv(2048)
        # print "recv:"
        # print data

        if data == 'quit\r\n':
            current_connection.shutdown(1)
            current_connection.close()
            break

        elif data == 'stop\r\n':
            current_connection.shutdown(1)
            current_connection.close()
            exit()

        elif data:
            current_connection.send(data)
            print(data)



