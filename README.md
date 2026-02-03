# 🧠🛩️ UAV Flight Control Stage — Teensy Carrier + Dual IMU (I2C)

![Hardware](https://img.shields.io/badge/Hardware-Flight%20Control%20Stage-blue)
![PCB](https://img.shields.io/badge/PCB-KiCad-lightgrey)
![IMU](https://img.shields.io/badge/IMU-BMI088%20%2B%20ICM--42605-orange)
![Bus](https://img.shields.io/badge/Bus-I2C-lightgrey)
![Teensy](https://img.shields.io/badge/MCU-Teensy%204.x-green)
![RF](https://img.shields.io/badge/RF-1%20RC%20Input-lightgrey)
![PWM](https://img.shields.io/badge/PWM-4%20Outputs-lightgrey)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

This repository documents the **Flight Control Stage PCB** of my modular UAV platform.

It replaces the previous **single-IMU (MPU6050)** design with a **dual-IMU architecture**
(**BMI088 + ICM-42605**) connected via **I2C**, while preserving all real-time flight interfaces:

- **1× RC input** (radio control command)
- **4× PWM outputs** (ESC / propulsion control)
- **UART telemetry**
- **Teensy 4.x carrier socket**



## 📂 Contents

- `/Hardware` → Schematics, PCB layout, Gerbers

## 🧩 System Architecture
<p align="center">
  <img src="https://github.com/user-attachments/assets/f7d678af-9d7e-4323-9c81-d6843dbee815" width="700">
</p>



## 🧩 Modular UAV Architecture

The UAV electronics are intentionally divided into **two independent hardware stages**:

| Stage | Responsibility |
|--------|------------------------------|
| **Flight Control Stage (this repo)** | Sensing + control + I/O |
| **Power Stage** | Power distribution & regulation |

### Flight Control Stage Composition

This board integrates:

- Teensy 4.x carrier board (control MCU)
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

---

## ✨ Key Features

- Dual IMU (**BMI088 + ICM-42605**) over **I2C**
- Teensy 4.x socketed carrier design
- 1× RC input
- 4× PWM outputs
- UART telemetry interface
- Physically isolated from power stage
- Mature **V5 hardware revision**, validated through multiple control iterations

---

## 🔌 Interfaces (I/O Summary)

| Interface | Qty | Description |
|-----------|----|-------------|
| Dual IMU (BMI088 + ICM-42605) | 1 | Two independent I²C buses |
| RC Input | 1 | Pilot command |
| PWM Outputs | 4 | Propulsion control |
| UART | 1 | Telemetry / debug |
| Teensy Socket | 1 | MCU carrier |

---

## 🖼️ PCB Render Visualization

<table>
  <tr>
    <td align="center">
      <img alt="flight control stage top" src="https://github.com/user-attachments/assets/406f617d-9878-42c9-9c5d-0eda53a336fc" width="520"><br>
      <sub>Top View</sub>
    </td>
    <td align="center">
      <img alt="flight control stage bottom" src="https://github.com/user-attachments/assets/68846fde-b554-45c8-acc2-ae7cef270093" width="520"><br>
      <sub>Bottom View</sub>
    </td>
  </tr>
</table>

---

## ⚡ Physical Prototype (V5 Installed on UAV)

The **V5 prototype** is directly integrated into the UAV for **full-system validation**.

Given the maturity of the control architecture, this stage is deployed directly for:

- flight control testing  
- system integration  
- real-time experimentation  

You may place real photos under:





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
