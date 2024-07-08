import serial
import tkinter as tk
from tkinter import simpledialog
from PIL import Image, ImageTk
import time


def choose_serial_ports():
    import serial.tools.list_ports
    ports = list(serial.tools.list_ports.comports())
    if len(ports) > 2:
        print("More than 2 serial ports detected. Please disconnect the unwanted serial ports.")
        exit()
    if len(ports) == 0:
        print("No serial port detected. Please connect the serial port.")
        exit()
    return [port.device for port in ports]


def send_angle(angle, serialz):
    for i in range(len(serialz)):
        serialz[i].write(f"{angle}\n".encode('utf-8'))
        print(f"Sent angle {angle} to Arduino {i}.")
        time.sleep(0.1)


def read_fault_knowledge(serialz, image_label, images):
    for i in range(len(serialz)):
        try:
            fault_knowledge = serialz[i].readline().decode().strip()
            print(fault_knowledge)
            if fault_knowledge == "":
                print(f"Arduino {i} did not respond.")
                image_label.config(image=images['master_error'])
            elif fault_knowledge == "Slave NOT RESPONDING!!!":
                print(f"Arduino {i} did not respond. Taking over control.")
                image_label.config(image=images['slave_error'])
                time.sleep(3)
            elif fault_knowledge == "Motor fail":
                print(f"Fault detected in M1 on Arduino {i}. M2 has been activated.")
                image_label.config(image=images['m1_fail'])
            else:
                print(f"Arduino {i} responded: {fault_knowledge}")
        except serial.SerialException:
            print(f"Error reading from Arduino {i}.")
            image_label.config(image=images['serial_error'])


def main():
    arduino_ports = choose_serial_ports()
    if len(arduino_ports) != 1:
        ser1 = serial.Serial(arduino_ports[0], 9600)
        ser2 = serial.Serial(arduino_ports[1], 9600)
        serialz = [ser1, ser2]
    else:
        ser1 = serial.Serial(arduino_ports[0], 9600)
        serialz = [ser1]

    root = tk.Tk()
    root.withdraw()

    # Load images
    default_image = ImageTk.PhotoImage(Image.open("default.png"))
    master_error_image = ImageTk.PhotoImage(Image.open("Master.png"))
    slave_error_image = ImageTk.PhotoImage(Image.open("Slave.png"))
    m1_fail_image = ImageTk.PhotoImage(Image.open("M1.png"))
    m2_fail_image = ImageTk.PhotoImage(Image.open("M2.png"))
    serial_error_image = ImageTk.PhotoImage(Image.open("Serial.png"))
    images = {
        'default': default_image,
        'master_error': master_error_image,
        'slave_error': slave_error_image,
        'm1_fail': m1_fail_image,
        'm2_fail': m2_fail_image,
        'serial_error': serial_error_image
    }

    try:
        # pls_Stop = True
        print(serialz[0].readline().decode().strip())
        # print(serialz[1].readline().decode().strip())

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
                read_fault_knowledge(serialz, image_label, images)
            except ValueError:
                print("Invalid input. Please enter a number.")
            finally:
                dialog.destroy()
                root.quit()

        submit_button = tk.Button(dialog, text="Submit", command=submit)
        submit_button.pack()

        def cancel():
            dialog.destroy()
            root.quit()

        cancel_button = tk.Button(dialog, text="Cancel", command=cancel)
        cancel_button.pack()

        root.mainloop()

    except KeyboardInterrupt:
        print("Program terminated by user.")
    finally:
        for i in range(len(serialz)):
            serialz[i].close()


if __name__ == "__main__":
    main()
