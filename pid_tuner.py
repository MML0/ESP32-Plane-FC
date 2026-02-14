import socket, struct, time

UDP_IP = "192.168.4.1"
UDP_PORT = 50000
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send_pid(axis, level, kp, ki, kd):
    CMD_PID_TUNE = 1
    packet = struct.pack('<BBBfff', CMD_PID_TUNE, axis, level, kp, ki, kd)
    sock.sendto(packet, (UDP_IP, UDP_PORT))
    print(f"Sent PID: axis={axis}, level={level}, Kp={kp}, Ki={ki}, Kd={kd}")
    time.sleep(0.05)

# send_pid(0, 0, 4.0, 0.0, 0.0)
# sock.sendto(b"Hello ESP32", (UDP_IP, UDP_PORT))
send_pid(1, 1, 0.35, 0.0, 0.004)    