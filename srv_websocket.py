#!/usr/bin/env python3

import asyncio
import websockets

# @asyncio.coroutine
# def hello():
#     websocket = yield from websockets.connect('ws://localhost:8765/')
# 
#     try:
#         name = 'hohoho'
#         yield from websocket.send(name)
#         print("> {}".format(name))
# 
#         greeting = yield from websocket.recv()
#         print("< {}".format(greeting))
# 
#     finally:
#         yield from websocket.close()
# 
# asyncio.get_event_loop().run_until_complete(hello())

@asyncio.coroutine
def echo(websocket, path):
    text = yield from websocket.recv()
    print("< {}".format(text))
    yield from websocket.send(text)

asyncio.get_event_loop().run_until_complete(websockets.serve(echo, '0.0.0.0', 1234))
asyncio.get_event_loop().run_forever()


