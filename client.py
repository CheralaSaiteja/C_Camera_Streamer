import cv2
import numpy as np
import socket
import struct


SERVER_IP = "127.0.0.1"
SERVER_PORT = 9999
BUF_SIZE = 65507

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    try:
        # Send "data" string to server
        sock.sendto(b"data", (SERVER_IP, SERVER_PORT))


        sock.settimeout(1)
        # Receive frame size from server
        frameSize, addr = sock.recvfrom(4)  # Assuming frame size is sent as a 4-byte integer
        frameSize = struct.unpack('!I', frameSize)[0]

        # Receive compressed frame data from server
        data, addr = sock.recvfrom(int(frameSize))
        sock.settimeout(None)

        # Convert bytes to numpy array
        nparr = np.frombuffer(data, np.uint8)

        # Decode image
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)

        # Display image
        cv2.imshow("Stream", img)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    except Exception as e:
        print(e)
# Close socket
sock.close()
cv2.destroyAllWindows()
