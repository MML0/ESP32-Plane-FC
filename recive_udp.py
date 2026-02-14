import socket

UDP_IP = "0.0.0.0"
UDP_PORT = 50000
BUFFER_SIZE = 1024

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening on {UDP_PORT}...")

while True:
    data, addr = sock.recvfrom(BUFFER_SIZE)
    
    try:
        msg = data.decode().strip()
        
        if len(msg) == 16:  # 4 channels × 4 digits
            ch1 = int(msg[0:4])
            ch2 = int(msg[4:8])
            ch3 = int(msg[8:12])
            ch4 = int(msg[12:16])

            print(f""" Left X  : {ch1} Left Y  : {ch2} Right X : {ch3} Right Y : {ch4}""")
        else:
            print("Unexpected packet:", msg)

    except Exception as e:
        print("Error:", e)
