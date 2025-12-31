# SafeCycle  
### Smart Two-Wheeler Safety and Anti-Theft Monitoring System

SafeCycle is a microcontroller-based smart **two-wheeler safety system** designed to enhance rider safety and vehicle security.  
It is suitable for **motorbikes and bicycles** or any other two-wheeler.

The system integrates **anti-theft detection, crash monitoring, rear-collision warning, GPS location tracking, automatic lighting, vibration alerts, and Bluetooth connectivity** into a compact embedded solution.

---

## System Diagrams

### Circuit Diagram
![Circuit Diagram](https://i.ibb.co.com/tPQHh9nn/circuit-diagram.png)

### Workflow Diagram
![Workflow Diagram](https://i.ibb.co.com/Wvm3sRqc/workflow-diagrama-removebg-preview.png)

---

## Key Features

- GPS-based live tracking with Google Maps link  
- Anti-theft detection using motion sensing and alarm  
- Crash detection with SOS alert if the two-wheeler remains fallen  
- Rear-collision warning using ultrasonic sensing  
- Automatic headlight control using LDR  
- Smart brake light using motion-based deceleration detection  
- Rider vibration alerts  
- Bluetooth mobile control and monitoring  
- Real-time status on 16×2 LCD display  

---

## System Overview

The system is controlled by an **Arduino Uno R3**, which continuously reads data from connected sensors.  
The controller processes motion, distance, light, and positional data to detect:

- theft attempts  
- crashes  
- fast-approaching rear vehicles  
- low-light conditions  

The rider interacts with the device through **Bluetooth commands from a smartphone**.  
Alerts are provided using **buzzer, LEDs, vibration motor, LCD display, and GPS links**.

The device operates in two main modes:

- **Armed Mode (Anti-Theft / Locked)**
- **Riding Mode (Active Safety Assist)**

---

## Hardware Components

- Arduino Uno R3  
- HC-05 Bluetooth Module  
- NEO-6M GPS Receiver  
- MPU-6050 Accelerometer + Gyroscope  
- HC-SR04 Ultrasonic Sensor  
- LDR Sensor Module  
- 16×2 I²C LCD Display  
- White LED Headlight  
- Red LED Taillight / Brake Light  
- Buzzer  
- Vibration Motor Module  
- Logic Level Shifter  
- Buck Converter  
- 2× 18650 Lithium Batteries + Holder  
- Rocker Switch  
- Resistors, wiring, and mounting enclosure  

---

## Communication Protocols

- **UART Serial**  
  - HC-05 Bluetooth  
  - NEO-6M GPS  

- **I²C Bus**  
  - MPU-6050  
  - LCD Display  

- **Digital I/O**  
  - Ultrasonic trigger/echo  
  - LEDs, buzzer, vibration motor  

- **PWM Output**  
  - Variable taillight brightness  

---

## Mobile App Support

### Custom MIT App Inventor Application

A dedicated Android app was developed using **MIT App Inventor**.  
When connected with the **HC-05 Bluetooth module**, users can control the entire system.

- The **APK** and **.AIA source files** are located in the `/app` folder of this repository.

#### App Interface Screenshot
![App Interface](https://i.ibb.co.com/1tj2gpn8/Screenshot-2025-12-31-222322-Photoroom.png)

---

### Serial Bluetooth Terminal Support (Alternative App)

This project is also compatible with the **Serial Bluetooth Terminal** app from Google Play Store.

Google Play link:  
https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&pcampaignid=web_share

#### Serial Terminal Interface During Operation
![Terminal Screenshot](https://i.ibb.co.com/bRHG1vPK/Gemini-Generated-Image-453n29453n29453n.png)

---

## Operating Modes

### Armed / Anti-Theft Mode
- Enabled via Bluetooth command  
- Detects unusual motion or vibration  
- Triggers buzzer and flashing lights  
- Sends GPS Google Maps link  

### Riding / Active Safety Mode
- Automatic headlights in low light  
- Motion-based smart brake light  
- Rear-collision warning with vibration alerts  
- Crash detection and SOS location message  

---

## Bluetooth Commands

| Command | Description |
|--------|-------------|
| **A** | Arm / Lock system |
| **D** | Disarm / Unlock system |
| **C** | Clear alarms |
| **S** | System status + live GPS |
| **L** | Send current location |

---

## Workflow Summary

1. System powers on and initializes sensors  
2. Bluetooth connects with smartphone  
3. User arms or disarms system  
4. Arduino continuously monitors sensors  
5. System reacts automatically:
   - theft → alarm + location link  
   - crash → SOS alert + location  
   - dark → headlights on  
   - approaching vehicle → flashing taillight + vibration alert  
6. LCD displays system state in real time  

---

## Future Enhancements

- Native Android/iOS application  
- GSM/SMS emergency alert integration  
- Custom PCB hardware design  
- Fully waterproof enclosure  

---

