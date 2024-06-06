from time import sleep
from RpiMotorLib import RpiMotorLib

# Define GPIO pins
M1 = [17, 18, 19, 20]
M2 = [21, 22, 23, 24]


# Create stepper motor object
my_stepper = RpiMotorLib.BYJMotor("MyStepper", "28BYJ")

# Define number of steps per rotation
steps_per_revolution = 2048

Arduino1_not_active = False
Motor1_not_active = False


def move_motor_to_angle(angle, Motor1_not_active=False):
    # Convert angle to steps
    steps = int(angle * steps_per_revolution / 360)

    # Set direction based on angle
    if angle >= 0:
        direction = False  # Clockwise
    else:
        direction = True  # Counter-clockwise

    # Move motor
    if Motor1_not_active:
        my_stepper.motor_run(M2, 0.001, abs(steps), direction, False, "half", 0.001)
    else:
        my_stepper.motor_run(M1, 0.001, abs(steps), direction, False, "half", 0.001)


def main():
    try:
        while True:
            angle_in = input("Enter the angle: ")
            try:
                angle_in = float(angle_in)
            except ValueError:
                print("Invalid input. Please enter a number.")
                continue

            if abs(angle_in) > 360:
                angle_in = angle_in % 360

            move_motor_to_angle(angle_in)

    except KeyboardInterrupt:
        print("Program terminated by user.")


if __name__ == "__main__":
    main()
