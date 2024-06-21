import serial
import tkinter as tk
from tkinter import simpledialog
import time


def choose_serial_ports():
    import serial.tools.list_ports
    ports = list(serial.tools.list_ports.comports())
    if len(ports) >2:
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


def read_fault_knowledge(serialz):
    for i in range(len(serialz)):
        try:
            fault_knowledge = serialz[i].readline().decode().strip()
            if fault_knowledge == "":
                print(f"Arduino {i} did not respond.")
            elif fault_knowledge == "Slave not responding, taking over...":
                print(f"Arduino {i} did not respond. Taking over control.")
                time.sleep(3)
            elif fault_knowledge == "M1 fail":
                print(f"Fault detected in M1 on Arduino {i}. M2 has been activated.")
            else:
                print(f"Arduino {i} responded: {fault_knowledge}")
        except serial.SerialException:
            print(f"Error reading from Arduino {i}.")



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

    try:
        pls_Stop = True
        print(serialz[0].readline().decode().strip())
        print(serialz[1].readline().decode().strip())
        while pls_Stop:
            angle_in = simpledialog.askfloat("Input", "Enter the angle:")
            if angle_in is None:
                print("User canceled the input")
                break
            if isinstance(angle_in, float) or isinstance(angle_in, int):
                if abs(angle_in) > 360:
                    angle_in = angle_in % 360
                send_angle(angle_in, serialz)
                time.sleep(1)
                time.sleep((abs(angle_in)/360)/(2/60))
                read_fault_knowledge(serialz)
            else:
                print("Invalid input. Please enter a number.")

    except KeyboardInterrupt:
        print("Program terminated by user.")
    finally:
        for i in range(len(serialz)):
            serialz[i].close()



if __name__ == "__main__":
    main()