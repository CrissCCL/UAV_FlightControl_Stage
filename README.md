# 🧠🛩️ UAV Control & Navigation Board — Teensy Carrier + Dual IMU (I2C)

![Hardware](https://img.shields.io/badge/Hardware-Control%20Stage-blue)
![PCB](https://img.shields.io/badge/PCB-KiCad-lightgrey)
![IMU](https://img.shields.io/badge/IMU-BMI088%20%2B%20ICM--42605-orange)
![Bus](https://img.shields.io/badge/Bus-I2C-lightgrey)
![Teensy](https://img.shields.io/badge/MCU-Teensy%204.x-green)
![RF](https://img.shields.io/badge/RF-1%20RC%20Input-lightgrey)
![PWM](https://img.shields.io/badge/PWM-4%20Outputs-lightgrey)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

This repository documents the **control & navigation stage PCB** of my modular UAV platform.

It upgrades the previous **single-IMU (MPU6050)** design to a **dual-IMU architecture**
based on **BMI088 + ICM-42605**, connected through **I2C**, while preserving the essential
real-time flight interfaces:

- **1× RC input** (radio control command)
- **4× PWM outputs** (ESC / propulsion control)
- **UART telemetry**
- **Teensy 4.x carrier socket**

## 📂 Contents
- `/Hardware` →  Schematic, Gerbers.


## 🧩 Modular UAV Architecture

The UAV electronics are intentionally split into **independent functional boards**:

| Stage | Responsibility |
|-------|----------------|
| Power Stage | Distribution & regulation |
| Control Stage (this repo) | Sensing + control + I/O |
| IMU Module | Redundant inertial sensing |

This separation improves:

- electrical noise isolation  
- power integrity  
- maintainability  
- faster hardware iteration  

The dedicated power board is available here:  
👉 https://github.com/CrissCCL/UAV_PowerStage



## ✨ Key Features

- Dual IMU (**BMI088 + ICM-42605**) over **I2C**
- Teensy 4.x socketed carrier design
- 1× RC input interface
- 4× PWM outputs for ESCs
- UART header for telemetry/debug
- Fully separated from the power stage
- Mature **V5 hardware revision**, validated through multiple control iterations


## 🔌 Interfaces (I/O Summary)

| Interface | Qty | Description |
|-----------|----|-------------|
| Dual IMU (I2C) | 1 | Redundant inertial sensing |
| RC Input | 1 | Pilot command |
| PWM Outputs | 4 | Propulsion control |
| UART | 1 | Telemetry / debug |
| Teensy Socket | 1 | MCU carrier |



## 🖼️ PCB Render Visualization

<table>
  <tr>
    <td align="center">
      <img alt="flight control stage dron_v5 top" src="https://github.com/user-attachments/assets/406f617d-9878-42c9-9c5d-0eda53a336fc" width="550"><br>
      <sub> flight control stage V5 – Top View </sub>
    </td>
    <td align="center">
        <img  alt="flight control stage dron_v5 bottom" src="https://github.com/user-attachments/assets/68846fde-b554-45c8-acc2-ae7cef270093" width="550"><br>
      <sub> Flight control stage PCB V5 – Bottom View </sub>
    </td>
  </tr>
</table>


## ⚡ Physical Prototype (V5 Installed on UAV)

The **V5 prototype** is directly integrated into the UAV for **full-system validation**.

Given the maturity of the control architecture, this board is deployed directly for:

- flight control testing  
- system integration  
- real-time experiments  

Add your real photos under:




## 🔗 Related Repositories

- UAV platform main repository: https://github.com/CrissCCL/DIY_UAV
- Power stage PCB: https://github.com/CrissCCL/UAV_PowerStage
- Dual IMU module repository: https://github.com/CrissCCL/UAV_Dual_IMU

## ⚠️ Disclaimer

Educational and experimental use only.
Always follow safe practices when working with UAV propulsion systems.

## 🤝 Support Projects

Support my work on Patreon:  
https://www.patreon.com/c/CrissCCL


## 📜 License

MIT License


