# 🧠🛩️ UAV Flight Control Stage — Teensy Carrier + Dual IMU (I2C)

![Hardware](https://img.shields.io/badge/Hardware-Flight%20Control%20Stage-blue)
![PCB](https://img.shields.io/badge/PCB-Custom%20Design-lightgrey)
![Sensing](https://img.shields.io/badge/Sensing-Dual%20IMU-orange)
![Architecture](https://img.shields.io/badge/Architecture-Modular%20Control%20%2F%20Power-lightgrey)
![Integration](https://img.shields.io/badge/Integration-Embedded%20Carrier-green)
![License](https://img.shields.io/badge/License-MIT-lightgrey)


## Overview

This repository documents the **Flight Control Stage PCB**, a custom carrier board
designed to integrate sensing, command, and actuation interfaces for a modular UAV platform.

The goal of this stage is to provide a **clean and reliable hardware foundation**
for embedded flight control electronics, concentrating all signal routing and I/O
on a dedicated board while keeping the **power distribution stage electrically separated**.

This revision replaces the previous single-IMU layout with a **redundant dual-IMU
architecture (BMI088 + ICM-42605)** connected through independent I²C buses to improve
measurement reliability and fault tolerance.

This hardware revision has been validated using a **Teensy 4.0** microcontroller as the target flight-control MCU.


## 📂 Contents

- `/Hardware` → Schematics, PCB layout, Gerbers
- `/test_dual_angle_failover` → Embedded test firmware (dual-IMU fault injection, validation utilities)

## 🧩 System Architecture

<p align="center">
  <img src="https://github.com/user-attachments/assets/3c0c4e86-03ec-4817-90f4-cec5f8e356ae" width="700">
</p>


## 🧩 Modular UAV Architecture

The UAV electronics are intentionally divided into **two independent hardware stages**:

| Stage | Responsibility |
|--------|------------------------------|
| **Flight Control Stage (this repo)** | Sensing + control + I/O |
| **Power Stage** | Power distribution & regulation |

### Flight Control Stage Composition

This board integrates:

- Teensy 4.x carrier board (real-time control MCU)
- Dual IMU module (BMI088 + ICM-42605)
- RC input interface
- 4× PWM outputs for ESCs
- UART telemetry/debug

### Why separation?

Separating **control electronics** from **power electronics** provides:

- lower electrical noise coupling  
- improved signal integrity  
- safer power handling  
- easier maintenance and board replacement  
- faster hardware iteration



Dedicated power board:  
👉 https://github.com/CrissCCL/UAV_PowerStage


## ✨ Key Features

- Dual IMU redundancy (**BMI088 + ICM-42605**) over **I²C ×2**
- Real-time embedded control on Teensy 4.x
- 1× RC input
- 4× PWM outputs
- UART telemetry interface
- Physically isolated from power stage
- Mature **V5 hardware revision**, validated through multiple control iterations

## 🧪 Tested Platform

- **MCU:** Teensy 4.0  
- **Test mode:** Bench testing (no flight)  
- **Fault injection:** Firmware-controlled via UART  


## 🧪 Dual-IMU Fault Injection & Validation

To validate the robustness of the **redundant dual-IMU architecture**, a **firmware-based fault injection framework** was implemented on the Flight Control Stage.

Instead of relying on physical disconnections or uncontrolled hardware failures, IMU faults are **intentionally emulated in firmware**, enabling deterministic, repeatable, and safe validation of the sensing and fallback logic.

### 🎯 Validation Objectives

- Verify **fault detection** on each IMU (BMI088 / ICM-42605)
- Validate **primary / secondary IMU selection logic**
- Confirm correct **telemetry reporting** under fault conditions
- Observe system behavior during **degraded sensing scenarios**

### 🔧 Emulated Fault Modes

The firmware allows selective injection of failures on either IMU, including:

- IMU not responding (communication failure)
- Frozen sensor data (stuck output)
- Corrupted or invalid measurements
- Forced mismatch between redundant sensors

These modes reproduce realistic sensor failure scenarios without modifying or stressing the hardware.

### 🧠 System Behavior Under Test

During fault injection, the following mechanisms are evaluated:

- IMU health monitoring
- Mismatch detection between redundant sensors
- Automatic fallback to the alternate IMU
- Consistent reporting through UART telemetry and monitoring interfaces

This validation strategy allows early verification of **fault-tolerant sensing behavior** before flight testing.

### ✅ Benefits of Firmware-Based Fault Injection

- Fully repeatable and controllable tests  
- No physical stress on sensors or connectors  
- Faster validation cycles  
- Increased confidence in flight-control sensing reliability  

> This approach is a key step toward a **robust and maintainable flight control sensing architecture**, suitable for real-world UAV operation.

### 📈 Experimental Results — IMU Fault Injection

The following plot shows the system response during **firmware-emulated IMU fault scenarios**.

It illustrates:

- Real-time attitude estimation from both IMUs
- Detection of inconsistent or invalid sensor data
- Automatic switching between primary and secondary IMU
- Continuity of the estimated state during sensor failure events

<p align="center">
  <img alt="imu_failover_test" src="https://github.com/user-attachments/assets/b9fde24f-86cf-4e0f-8dd4-fc9af02aaa6d" width="700">
</p>

<p align="center">
  <sub>
    Dual-IMU fault injection test — mismatch detection and automatic IMU fallback under emulated failure conditions
  </sub>
</p>
> Note: The test was performed with the UAV mechanically fixed on a test bench.
> IMU failures were emulated in firmware to ensure repeatability, safety, and deterministic validation.


During the test, faults were injected dynamically via firmware commands, without any hardware disconnection.
The system maintained valid state estimation while transparently switching between sensors, confirming the
correct operation of the redundancy and failover logic.

### 🧾 Fault Injection Control Interface (Serial Commands)

IMU fault scenarios are triggered via **serial commands**, allowing controlled and repeatable testing without hardware modification.

Faults are injected at runtime through the **UART serial monitor**, using newline-terminated commands (`\n`).

#### Supported Commands
- F0 -> No fault (normal operation)
- F1 -> BMI088 dead (bmi_data_ok = 0)
- F2 -> ICM-42605 dead (icm_data_ok = 0)
- F3 -> BMI088 data corruption (forces mismatch)
- F4 -> ICM-42605 data corruption
- F5 -> BMI088 stuck output (frozen data)
- F6 -> ICM-42605 stuck output (frozen data)
- F3,5000 -> Apply F3 for 5000 ms, then automatically return to F0

#### Usage Notes

- All commands must be terminated with `\n`
- Timed fault injection (`Fx,<time_ms>`) allows transient failure testing
- Commands can be issued dynamically during operation
- No hardware reset or reconnection is required between tests

This interface enables **deterministic validation of IMU redundancy, fault detection, and failover behavior** under realistic failure conditions.

> The firmware implementation for fault injection is available under `/test_dual_angle_failover/`.



## 🔌 Hardware Interfaces

| Interface | Qty | Description |
|-----------|----|-------------|
| Dual IMU (BMI088 + ICM-42605) | 1 | Two independent I²C buses |
| RC Input | 1 | Pilot command |
| PWM Outputs | 4 | Propulsion control |
| UART | 1 | Telemetry / debug |
| Teensy Socket | 1 | MCU carrier |


## 🖼️ PCB Render Visualization

<table>
  <tr>
    <td align="center">
      <img alt="flight control stage top" src="https://github.com/user-attachments/assets/406f617d-9878-42c9-9c5d-0eda53a336fc" width="520"><br>
      <sub>Top View</sub>
    </td>
    <td align="center">
      <img alt="flight control stage bottom" src="https://github.com/user-attachments/assets/75adb4cf-c401-4bad-8cbf-d95335059692" width="520"><br>
      <sub>Bottom View</sub>
    </td>
  </tr>
</table>


## ⚡ Physical Prototype (V5 Installed on UAV)

The **V5 prototype** is directly integrated into the UAV for **full-system validation**.

Given the maturity of the control architecture, this stage is deployed directly for:

- flight control testing  
- system integration  
- real-time experimentation  

The following image shows the complete setup of the **prototype version V1**:

<table>
  <tr>
    <td align="center">
      <img alt="light control stage" src="https://github.com/user-attachments/assets/99a2fe58-2dc6-4015-a1dc-597217e6dd90" width="550"><br>
      <sub> light control stage </sub>
    </td>
    <td align="center">
        <img  alt="Flight control stage + MCU " src="https://github.com/user-attachments/assets/1c74cf11-d5d3-479b-a549-90cbfa393560" width="550"><br>
      <sub> Flight control stage + MCU </sub>
    </td>
  </tr>
</table>


## 🔗 Related Repositories

- UAV platform main repository: https://github.com/CrissCCL/DIY_UAV
- Power stage PCB: https://github.com/CrissCCL/UAV_PowerStage
- Dual IMU evaluation module (sensor characterization and early validation):  
  https://github.com/CrissCCL/UAV_Dual_IMU

## ⚠️ Disclaimer

Educational and experimental use only.
Always follow safe practices when working with UAV propulsion systems.

## 🤝 Support Projects

Support my work on Patreon:  
https://www.patreon.com/c/CrissCCL


## 📜 License

MIT License
