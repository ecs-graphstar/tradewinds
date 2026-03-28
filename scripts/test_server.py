import time
import zmq

context = zmq.Context()
socket = context.socket(zmq.REP)
# socket.bind("ipc:///tmp/feeds.ipc")
socket.bind("ipc:///tmp/feeds.ipc")

while True:
    message = socket.recv()
    print(f"Received req: {message}")
    time.sleep(0.01)
    socket.send_string("World")
