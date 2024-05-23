import serial
import tkinter as tk
from tkinter import simpledialog
import time


def choose_serial_port():
    import serial.tools.list_ports
    ports = list(serial.tools.list_ports.comports())
    for i, port in enumerate(ports):
        print(f"{i + 1}. {port.device} - {port.description}")
    port_index = int(input("Select the serial port number: ")) - 1
    return ports[port_index].device


def send_angle(angle):
    ser.write(f"{angle}\n".encode('utf-8'))
    time.sleep(0.1)


def main():
    global ser
    arduino_port = choose_serial_port()
    ser = serial.Serial(arduino_port, 9600)

    root = tk.Tk()
    root.withdraw()

    try:
        pls_Stop = True
        while pls_Stop:
            angle_in = simpledialog.askfloat("Input", "Enter the angle:")
            if angle_in is None:
                print("User canceled the input")
                break
            if isinstance(angle_in, float) or isinstance(angle_in, int):
                if abs(angle_in) > 360:
                    angle_in = angle_in % 360
                send_angle(angle_in)
                time.sleep((abs(angle_in)/360)/(5/60))
                fault_knowledge = ser.readline()
                if fault_knowledge == b"M1 fail":
                    print("Fault detected in M1. M2 has been activated.")
                    break
            else:
                print("Invalid input. Please enter a number.")

    except KeyboardInterrupt:
        print("Program terminated by user.")
    finally:
        ser.close()


if __name__ == "__main__":
    main()
