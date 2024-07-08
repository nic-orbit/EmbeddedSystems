import serial
import serial.tools.list_ports
import tkinter as tk
from PIL import Image, ImageTk
import threading
import time

def choose_serial_ports():
    ports = list(serial.tools.list_ports.comports())
    if len(ports) > 2:
        print("More than 2 serial ports detected. Please disconnect the unwanted serial ports.")
        exit()
    if len(ports) == 0:
        print("No serial port detected. Please connect the serial port.")
        exit()
    return [port.device for port in ports]

def send_angle(angle, serialz):
    for ser in serialz:
        try:
            ser.write(f"{angle}\n".encode('utf-8'))
            print(f"Sent angle {angle}.")
            time.sleep(0.1)
        except serial.SerialException:
            print("Error sending angle to Arduino.")

def read_fault_knowledge(serialz):
    responses = []
    for ser in serialz:
        try:
            if ser.in_waiting > 0:
                fault_knowledge = ser.readline().decode().strip()
                responses.append(fault_knowledge)
        except serial.SerialException:
            print("Error reading from Arduino.")
            responses.append("serial_error")
    return responses

def update_image_label(image_label, images, fault_knowledge):
    if fault_knowledge == "":
        image_label.config(image=images['master_error'])
    elif fault_knowledge == "Slave NOT RESPONDING!!!":
        image_label.config(image=images['slave_error'])
    elif fault_knowledge == "Motor fail":
        image_label.config(image=images['m1_fail'])
    else:
        image_label.config(image=images['default'])

def main():
    arduino_ports = choose_serial_ports()
    serialz = []

    try:
        for port in arduino_ports:
            ser = serial.Serial(port, 9600)
            serialz.append(ser)
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        return

    root = tk.Tk()
    root.withdraw()

    # Load images
    try:
        default_image = ImageTk.PhotoImage(Image.open("default.png"))
        master_error_image = ImageTk.PhotoImage(Image.open("Master.png"))
        slave_error_image = ImageTk.PhotoImage(Image.open("Slave.png"))
        m1_fail_image = ImageTk.PhotoImage(Image.open("M1.png"))
        m2_fail_image = ImageTk.PhotoImage(Image.open("M2.png"))
        serial_error_image = ImageTk.PhotoImage(Image.open("Serial.png"))
    except IOError as e:
        print(f"Error loading image: {e}")
        return

    images = {
        'default': default_image,
        'master_error': master_error_image,
        'slave_error': slave_error_image,
        'm1_fail': m1_fail_image,
        'm2_fail': m2_fail_image,
        'serial_error': serial_error_image
    }

    dialog = tk.Toplevel(root)
    dialog.geometry("1300x700")
    dialog.title("Input Dialog")

    image_label = tk.Label(dialog, image=images['default'])
    image_label.pack()

    angle_entry = tk.Entry(dialog)
    angle_entry.pack()

    def submit():
        angle_in = angle_entry.get()
        try:
            angle_in = float(angle_in)
            if abs(angle_in) > 360:
                angle_in = angle_in % 360
            send_angle(angle_in, serialz)
            time.sleep(1)
            responses = read_fault_knowledge(serialz)
            if responses:
                latest_response = responses[-1]
                update_image_label(image_label, images, latest_response)
        except ValueError:
            print("Invalid input. Please enter a number.")
        finally:
            dialog.destroy()
            root.quit()

    submit_button = tk.Button(dialog, text="Submit", command=submit)
    submit_button.pack()

    cancel_button = tk.Button(dialog, text="Cancel", command=lambda: root.quit())
    cancel_button.pack()

    def serial_thread():
        while True:
            responses = read_fault_knowledge(serialz)
            if responses:
                latest_response = responses[-1]
                root.after(0, update_image_label, image_label, images, latest_response)
            time.sleep(0.1)

    threading.Thread(target=serial_thread, daemon=True).start()

    root.deiconify()
    root.mainloop()

    for ser in serialz:
        ser.close()

if __name__ == "__main__":
    main()
