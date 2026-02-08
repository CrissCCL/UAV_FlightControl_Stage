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



## 📂 Contents

- `/Hardware` → Schematics, PCB layout, Gerbers

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

---


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
- Dual IMU module repository: https://github.com/CrissCCL/UAV_Dual_IMU

## ⚠️ Disclaimer

Educational and experimental use only.
Always follow safe practices when working with UAV propulsion systems.

## 🤝 Support Projects

Support my work on Patreon:  
https://www.patreon.com/c/CrissCCL


## 📜 License

MIT License
