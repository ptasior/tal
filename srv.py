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
    print("accept")
    current_connection, address = connection.accept()
    while True:
        data = current_connection.recv(2048)
        print("recv", data, len(data))
        if len(data) == 0:
            current_connection.shutdown(1)
            current_connection.close()
            print("data = 0, disconnect !!!!")
            break

        current_connection.send(data)


