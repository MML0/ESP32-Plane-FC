# pid_tuner_gui_sliders.py
import socket
import time
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
pid_defaults = {
    "roll_angle": {"kp": 4.0, "ki": 0.0, "kd": 0.0},
    "roll_rate":  {"kp": 0.15, "ki": 0.0, "kd": 0.002},
    "pitch_angle": {"kp": 4.0, "ki": 0.0, "kd": 0.0},
    "pitch_rate":  {"kp": 0.15, "ki": 0.0, "kd": 0.002}
}

try:
    with open(SAVE_FILE, "r") as f:
        pid_values = json.load(f)
except:
    pid_values = pid_defaults.copy()

# ================= SEND FUNCTION =================
def log_message(msg):
    log_text.insert(tk.END, msg + "\n")
    log_text.see(tk.END)

def send_pid(axis, level, kp, ki, kd):
    CMD_PID_TUNE = 1
    packet = struct.pack('<BBBfff', CMD_PID_TUNE, axis, level, kp, ki, kd)
    sock.sendto(packet, (UDP_IP, UDP_PORT))

    msg = f"Sent -> axis={axis}, level={level}, Kp={kp:.4f}, Ki={ki:.4f}, Kd={kd:.4f}"
    log_message(msg)

    key = f"{'roll' if axis==0 else 'pitch'}_{'angle' if level==0 else 'rate'}"
    pid_values[key] = {"kp": kp, "ki": ki, "kd": kd}

    with open(SAVE_FILE, "w") as f:
        json.dump(pid_values, f, indent=2)

# ================= GUI =================
root = tk.Tk()
root.title("ESP32 PID Tuner")
root.geometry("950x650")

sliders = {}

def create_slider_with_entry(parent, label_text, var, from_, to_, resolution=0.001):
    container = tk.Frame(parent)
    container.pack(anchor='w', pady=3)

    ttk.Label(container, text=label_text, width=4).grid(row=0, column=0)

    slider = tk.Scale(container,
                      variable=var,
                      from_=from_,
                      to=to_,
                      orient='horizontal',
                      resolution=resolution,
                      length=180)
    slider.grid(row=0, column=1, padx=5)

    entry = ttk.Entry(container, width=8)
    entry.grid(row=0, column=2)

    # Update entry when slider moves
    def update_entry(*args):
        entry.delete(0, tk.END)
        entry.insert(0, f"{var.get():.4f}")

    var.trace_add("write", update_entry)

    # Update slider when entry changes
    def update_slider(event):
        try:
            value = float(entry.get())
            var.set(value)
        except:
            pass

    entry.bind("<Return>", update_slider)
    entry.bind("<FocusOut>", update_slider)

    update_entry()

    return slider

# ====== MAIN GRID FRAME ======
main_frame = tk.Frame(root)
main_frame.pack(pady=10)

pid_configs = [
    ("roll", "angle", 0, 0),
    ("roll", "rate", 0, 1),
    ("pitch", "angle", 1, 0),
    ("pitch", "rate", 1, 1)
]

for index, (axis_name, level_name, axis_val, level_val) in enumerate(pid_configs):
    row = index // 2
    col = index % 2

    default = pid_values[f"{axis_name}_{level_name}"]

    frame = tk.LabelFrame(
        main_frame,
        text=f"{axis_name.capitalize()} {level_name.capitalize()} PID",
        padx=5,
        pady=5
    )
    frame.grid(row=row, column=col, padx=10, pady=10, sticky="n")

    kp_var = tk.DoubleVar(value=default["kp"])
    ki_var = tk.DoubleVar(value=default["ki"])
    kd_var = tk.DoubleVar(value=default["kd"])

    sliders[f"{axis_name}_{level_name}_kp"] = kp_var
    sliders[f"{axis_name}_{level_name}_ki"] = ki_var
    sliders[f"{axis_name}_{level_name}_kd"] = kd_var

    create_slider_with_entry(frame, "Kp", kp_var, -20, 20, 0.001)
    create_slider_with_entry(frame, "Ki", ki_var, -1, 1, 0.001)
    create_slider_with_entry(frame, "Kd", kd_var, -0.05, 0.05, 0.0001)

# ====== SEND BUTTON ======
def send_all():
    for axis_name, level_name, axis_val, level_val in pid_configs:
        kp = sliders[f"{axis_name}_{level_name}_kp"].get()
        ki = sliders[f"{axis_name}_{level_name}_ki"].get()
        kd = sliders[f"{axis_name}_{level_name}_kd"].get()
        send_pid(axis_val, level_val, kp, ki, kd)
        time.sleep(0.05)

    log_message("=== All PIDs Sent ===")

send_btn = ttk.Button(root, text="Send All PIDs", command=send_all)
send_btn.pack(pady=10)

# ====== LOG WINDOW ======
log_frame = tk.LabelFrame(root, text="Log Output")
log_frame.pack(fill="both", expand=True, padx=10, pady=10)

log_text = tk.Text(log_frame, height=8)
log_text.pack(fill="both", expand=True)

root.mainloop()