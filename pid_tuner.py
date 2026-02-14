# simple pid tune sender

import socket
import struct

UDP_IP = "192.168.43.66"   # your ESP IP
UDP_PORT = 50000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

CMD_PID_TUNE = 1

def send_pid(axis, kp, ki, kd):
    # pack like your RadioPacket struct
    # B = uint8
    # h = int16
    # H = uint16
    # f = float

    packet = struct.pack(
        "<BhhhHBfff",
        CMD_PID_TUNE,
        0,      # roll
        0,      # pitch
        0,      # yaw
        1000,   # throttle
        axis,
        kp,
        ki,
        kd
    )

    sock.sendto(packet, (UDP_IP, UDP_PORT))
    print("Sent PID:", axis, kp, ki, kd)


# example usage
send_pid(axis=0, kp=1.5, ki=0.01, kd=0.02)
