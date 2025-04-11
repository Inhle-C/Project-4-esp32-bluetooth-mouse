
# Project 4: Bluetooth Mouse with ESP32

**Part 4 of my IoT Lab Series using ESP32 and Raspberry Pi 4**

## Overview

This project transforms the ESP32-C3 board into a Bluetooth-enabled mouse using the ICM-42670-P sensor.  
The board detects its inclination and controls mouse movement accordingly, with acceleration and speed levels based on tilt intensity and duration.

The goal is to integrate hardware sensors and Bluetooth HID functionality for real-time motion control.

## Objectives

- ✅ Detect board inclination and print UP/DOWN or LEFT/RIGHT using ESP_LOGI
- ✅ Connect the ESP32 to the Raspberry Pi as a Bluetooth mouse
- ✅ Move the mouse left/right based on board tilt and pause for 5 seconds
- ✅ Integrate movement detection and mouse control with speed scaling
- ✅ Implement acceleration control based on tilt intensity and time
- ✅ Demonstrate full control by clicking a target on the screen in under 5 seconds

## Project Structure

esp32-lab4-bluetooth-mouse/  
├── report.pdf # Lab report (required)  
├── lab4_1/ # Lab 4.1: Board Movement Detection  
│ ├── sdkconfig  
│ ├── CMakeLists.txt  
│ ├── README.md  
│ └── main/  
│ ├── CMakeLists.txt  
│ ├── main.c  
│ └── sensor.h  
├── lab4_2/ # Lab 4.2: Bluetooth Mouse Left/Right Movement  
│ ├── sdkconfig  
│ ├── CMakeLists.txt  
│ ├── README.md  
│ └── main/  
│ ├── CMakeLists.txt  
│ ├── main.c  
│ └── bluetooth_mouse.h  
├── lab4_3/ # Lab 4.3: Integration and Acceleration Control  
│ ├── sdkconfig  
│ ├── CMakeLists.txt  
│ ├── README.md  
│ └── main/  
│ ├── CMakeLists.txt  
│ ├── main.c  
│ ├── bluetooth_mouse.h  
│ └── sensor.h  

## Setup Instructions

### 🧭 Lab 4.1: Board Movement Detection

1. Read data from the ICM-42670-P sensor
2. Print board inclination to terminal using ESP_LOGI
- Example outputs:  
UP LEFT  
DOWN RIGHT  
3. Detect multiple directions simultaneously

### 🖱️ Lab 4.2: Bluetooth Mouse Left/Right Movement

1. Connect ESP32-C3 as a Bluetooth HID mouse to the Raspberry Pi
2. Reproduce the class Bluetooth project with mouse movement instead of volume control
3. Move the mouse left and right, then pause for 5 seconds

### 🚀 Lab 4.3: Integration and Acceleration Control

1. Combine Lab 4.1 and Lab 4.2
2. Control mouse direction and speed based on board inclination
- Use at least two tilt levels (e.g., A_BIT_LEFT, A_LOT_LEFT)
3. Implement acceleration:
- Increase movement factor (a) over time if inclination persists
4. Test scenario:
- TA moves mouse to random position and opens a terminal
- Close the terminal window in less than 5 seconds using the ESP32 as mouse
- 3 tries allowed, penalty for additional attempts

## Notes

- Exclude build/ directories when zipping the project.
- Submit the required directories only: lab4_1/*, lab4_2/*, lab4_3/*
- Document issues or learnings in report.pdf and subfolder README.md.
- All external code must follow APACHE or BSD-like licenses.
- Reference any helpful resources properly in report.pdf (No StackOverflow, Reddit).

## What I Learned

- Reading motion data from the ICM-42670-P sensor
- Using ESP_LOGI for real-time debugging
- Implementing Bluetooth HID mouse control with ESP32
- Combining sensor input and Bluetooth output for integrated control
- Developing acceleration algorithms for smooth mouse movement
- Testing and debugging embedded user interfaces

## Future Improvements

- Add click functionality for left and right buttons
- Improve acceleration curve for smoother control
- Integrate more complex gestures or tilt-based commands
- Optimize response time and accuracy
- Explore battery-powered wireless operation

## License

This project is for educational purposes.

Previous Project: [ESP32 Display and Sensor Integration](https://github.com/Inhle-C/Project-3-esp32-display-sensor)  
(Part 3 of the series)

Next Project: ESP32 Sensor Data Logger 🔗  
(To be uploaded as Part 5 of the series)
