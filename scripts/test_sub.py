import zmq

context = zmq.Context()
# 1. Change socket type to SUB
socket = context.socket(zmq.SUB)

# 2. Connect to the publisher's address
# Ensure this matches the port your Flecs ZMQServer is binding to
socket.connect("tcp://localhost:5555")

# 3. Sets the subscription filter.
# "" means subscribe to ALL topics.
# "weather" would only receive messages starting with "weather".
topic_filter = ""
socket.setsockopt_string(zmq.SUBSCRIBE, topic_filter)

print(f"Collecting updates from server on topic: '{topic_filter}'...")

while True:
    # 4. Subscribers usually receive in a loop.
    # Note: Pub/Sub is often multipart (Topic + Message)
    try:
        # If your C++ server sends multipart (topic then data):
        topic = socket.recv_string()
        messagedata = socket.recv_string()
        print(f"[{topic}] {messagedata}")
    except KeyboardInterrupt:
        break
