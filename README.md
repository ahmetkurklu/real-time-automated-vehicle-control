## Real-Time Vehicle Control and Command Simulation

This series of lab assignments focuses on the study and implementation of real-time control and command of a vehicle using a microcontroller. For ease of implementation, the vehicle in this case will be a virtual car simulated on a computer. The overall system overview is shown in Figure 1.

![Figure 1 - System Overview](https://github.com/ahmetkurklu/real-time-automated-vehicle-control/assets/24780090/9d47f7dd-6796-416b-95f3-6d267bde1208)

The ultimate goal of the lab is to develop a program that enables the vehicle to complete three laps of each circuit in the minimum possible time. To achieve this, the application must ensure proper guidance of the vehicle within its environment by generating commands (speed and wheel angle) based on measurements (actual speed, distance to the wall, circuit zone, etc.). The application should be fully autonomous, operating without external intervention. Several versions of the program will be studied, increasing in complexity.

### Hardware Description

The physical boundaries of the system under study are depicted in Figure 2.

![Figure 2 - Physical System Boundaries](https://github.com/ahmetkurklu/real-time-automated-vehicle-control/assets/24780090/c943fc5e-dee9-4e49-a5ac-c71c98fd3d70)

- The simulator (vehicle + circuit) runs on a PC, and it is provided for the lab.
- The application will run on a Renesas M32C microcontroller, utilizing the M308/4 kernel (used in previous tutorials).
- The development platform includes a keyboard, joystick, and LCD display.
- Communication between the simulator and the microcontroller is established through an RS-232 serial connection.
- Low-level device access routines (LCD display, keyboard, joystick) are provided as part of the project, some of which have been studied in previous sessions.
- The software layer for communication with the simulator is integrated into the project.



