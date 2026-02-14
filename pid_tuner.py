# pid_tuner_gui_sliders.py
import socket
import struct
import json
import tkinter as tk
from tkinter import ttk

# ================= CONFIG =================
UDP_IP = "192.168.4.1"
UDP_PORT = 50000
SAVE_FILE = "pid_values.json"
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# ================= DEFAULTS =================
# From your PIDManager constructor
pid_defaults = {
    "roll_angle": {"kp": 4.0, "ki": 0.0, "kd": 0.0},
    "roll_rate":  {"kp": 0.15, "ki": 0.0, "kd": 0.002},
    "pitch_angle": {"kp": 4.0, "ki": 0.0, "kd": 0.0},
    "pitch_rate":  {"kp": 0.15, "ki": 0.0, "kd": 0.002}
}

# Load saved values or use defaults
try:
    with open(SAVE_FILE, "r") as f:
        pid_values = json.load(f)
except:
    pid_values = pid_defaults.copy()

# ================= SEND FUNCTION =================
def send_pid(axis, level, kp, ki, kd):
    CMD_PID_TUNE = 1
    packet = struct.pack('<BBBfff', CMD_PID_TUNE, axis, level, kp, ki, kd)
    sock.sendto(packet, (UDP_IP, UDP_PORT))
    print(f"Sent PID -> axis={axis}, level={level}, Kp={kp}, Ki={ki}, Kd={kd}")
    # Save locally
    key = f"{'roll' if axis==0 else 'pitch'}_{'angle' if level==0 else 'rate'}"
    pid_values[key] = {"kp": kp, "ki": ki, "kd": kd}
    with open(SAVE_FILE, "w") as f:
        json.dump(pid_values, f, indent=2)

# ================= GUI =================
root = tk.Tk()
root.title("ESP32 PID Tuner")
root.geometry("600x500")

sliders = {}

def create_slider(parent, label_text, var, from_, to_, resolution=0.001):
    ttk.Label(parent, text=label_text).pack(anchor='w')
    slider = tk.Scale(parent, variable=var, from_=from_, to=to_,
                      orient='horizontal', resolution=resolution, length=400)
    slider.pack(anchor='w', padx=5, pady=2)
    return slider

row_frame = tk.Frame(root)
row_frame.pack(pady=5)

# Create sliders for all 4 PIDs and 3 terms each
for axis_name, level_name, default in [
    ("roll", "angle", pid_values["roll_angle"]),
    ("roll", "rate",  pid_values["roll_rate"]),
    ("pitch", "angle", pid_values["pitch_angle"]),
    ("pitch", "rate",  pid_values["pitch_rate"])
]:
    frame = tk.LabelFrame(root, text=f"{axis_name.capitalize()} {level_name.capitalize()} PID")
    frame.pack(padx=5, pady=2, fill="x")

    kp_var = tk.DoubleVar(value=default["kp"])
    ki_var = tk.DoubleVar(value=default["ki"])
    kd_var = tk.DoubleVar(value=default["kd"])

    sliders[f"{axis_name}_{level_name}_kp"] = kp_var
    sliders[f"{axis_name}_{level_name}_ki"] = ki_var
    sliders[f"{axis_name}_{level_name}_kd"] = kd_var

    create_slider(frame, "Kp", kp_var, -10, 10, 0.001)
    create_slider(frame, "Ki", ki_var, -1, 1, 0.001)
    create_slider(frame, "Kd", kd_var, -0.05, 0.05, 0.0001)

def send_all():
    for axis_name, level_name, axis_val, level_val in [
        ("roll","angle",0,0),
        ("roll","rate",0,1),
        ("pitch","angle",1,0),
        ("pitch","rate",1,1)
    ]:
        kp = sliders[f"{axis_name}_{level_name}_kp"].get()
        ki = sliders[f"{axis_name}_{level_name}_ki"].get()
        kd = sliders[f"{axis_name}_{level_name}_kd"].get()
        send_pid(axis_val, level_val, kp, ki, kd)
    print("All PIDs sent!")

send_btn = ttk.Button(root, text="Send All PIDs", command=send_all)
send_btn.pack(pady=10)

root.mainloop()
