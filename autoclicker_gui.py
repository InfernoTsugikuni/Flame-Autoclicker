import customtkinter as ctk
import subprocess
import threading
import keyboard

cursor_x = 0
cursor_y = 0
clicking = False
setting_position = False

def on_click(event):
    """ Updates the cursor position when the user clicks the Set Position button. """
    global cursor_x, cursor_y
    if setting_position:
        cursor_x = event.x
        cursor_y = event.y
        position_label.configure(text=f"Cursor Position: ({cursor_x}, {cursor_y})")

def start_clicking():
    """ Starts the auto-clicking process. """
    global clicking
    clicking = True

    # Check interval entry
    interval_str = interval_entry.get().strip()
    if interval_str == '':
        interval = 100
    else:
        try:
            interval = int(interval_str)
        except ValueError:
            interval_label.configure(text="Invalid interval! Using default (100 ms).", text_color="red")
            interval = 100

    clicks_str = clicks_entry.get().strip()
    if clicks_str == '':
        clicks = -1
    else:
        try:
            clicks = int(clicks_str)
        except ValueError:
            clicks_label.configure(text="Invalid clicks! Using default (never stop).", text_color="red")
            clicks = -1

    double_click = double_click_var.get()

    threading.Thread(target=run_autoclicker, args=(interval, clicks, double_click)).start()

def stop_clicking():
    """ Stops the auto-clicking process. """
    global clicking
    clicking = False

def run_autoclicker(interval, clicks, double_click):
    """ Executes the auto-clicking logic. """
    global clicking
    while clicking:
        subprocess.run(["autoclicker.exe", str(interval), str(clicks), "1", str(double_click)])
        if clicks != -1:
            clicks -= 1
        threading.Event().wait(0.1)

def toggle_setting_position():
    """ Toggles the setting position mode. """
    global setting_position
    setting_position = not setting_position
    if setting_position:
        position_button.configure(text="Click to Set Position")
        app.bind("<Button-1>", on_click)
    else:
        position_button.configure(text="Set Position")
        app.unbind("<Button-1>")

def toggle_clicking():
    """ Toggles the clicking state when F6 is pressed. """
    if clicking:
        stop_clicking()
        start_button.configure(text="Start/Stop (F6)")
        double_click_checkbox.configure(state="normal")
    else:
        start_clicking()
        start_button.configure(text="Clicking...")
        double_click_checkbox.configure(state="disabled")

app = ctk.CTk()
app.title("Auto Clicker")
app.geometry("400x450")
app.configure(bg='#333')

interval_label = ctk.CTkLabel(app, text="Interval (ms):", text_color="#ff6b00")
interval_label.pack(pady=(20, 5))
interval_entry = ctk.CTkEntry(app)
interval_entry.pack(pady=(0, 20))

clicks_label = ctk.CTkLabel(app, text="Number of Clicks (0 for 'Never stop'):", text_color="#ff6b00")
clicks_label.pack(pady=(0, 5))
clicks_entry = ctk.CTkEntry(app)
clicks_entry.pack(pady=(0, 20))

double_click_var = ctk.IntVar(value=0)
double_click_checkbox = ctk.CTkCheckBox(app, text="Double Click", variable=double_click_var, text_color="white")
double_click_checkbox.pack(pady=(10, 20))

position_label = ctk.CTkLabel(app, text="Cursor Position: (0, 0)", text_color="white")
position_label.pack(pady=(10, 20))

position_button = ctk.CTkButton(app, text="Set Position", command=toggle_setting_position, fg_color="#ff6b00", hover_color="#ff8c00")
position_button.pack(pady=(10, 20))

hotkey_message = ctk.CTkLabel(app, text="Press F6 to start/stop clicking...", text_color="white")
hotkey_message.pack(pady=(0, 5))

start_button = ctk.CTkButton(app, text="Start/Stop (F6)", command=toggle_clicking, fg_color="#ff6b00", hover_color="#ff8c00")
start_button.pack(pady=(10, 20))

start_button.bind("<Enter>", lambda e: start_button.configure(cursor="hand2"))
start_button.bind("<Leave>", lambda e: start_button.configure(cursor=""))

app.bind("<F6>", lambda event: toggle_clicking())

threading.Thread(target=lambda: keyboard.add_hotkey('F6', toggle_clicking), daemon=True).start()

app.mainloop()
