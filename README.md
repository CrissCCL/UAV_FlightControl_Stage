# 🧠🛩️ UAV Control & Navigation Board — Teensy + Dual IMU Upgrade

![Hardware](https://img.shields.io/badge/Hardware-Control%20%26%20Navigation-blue)
![PCB](https://img.shields.io/badge/PCB-KiCad-lightgrey)
![IMU](https://img.shields.io/badge/IMU-Dual%20IMU-orange)
![Teensy](https://img.shields.io/badge/MCU-Teensy%204.x-green)
![RF](https://img.shields.io/badge/RF-RC%20Inputs-lightgrey)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

This repository documents the **updated control stage PCB** for a custom UAV platform.
It replaces the previous **single IMU (MPU6050)** with a **validated Dual-IMU module**,
while preserving the full I/O required for flight experiments:

- **RC receiver RF inputs**
- **UART communication**
- **PWM outputs** for propulsion (ESCs)
- **Teensy socket** (carrier-style board)

✅ This control board is **physically and electrically separated** from the power board:

- **Power stage** (distribution + regulation): *UAV Power Distribution & Regulation Board*  
  *(separate repo / already developed and installed)*



## ✨ Key Features

- **Dual IMU upgrade** (replacing MPU6050-based sensing)
- **Teensy carrier board** with socketed MCU integration
- **RC input interface** (radio control signals)
- **PWM outputs** for ESC/propulsion control
- **UART connectivity** for telemetry / debugging / companion computer integration
- Modular architecture: **control stage separated from power stage**



## 🧩 System Role (Modular UAV Architecture)

This PCB implements the **control + sensing + I/O hub** of the UAV:

- Reads inertial signals from the **Dual IMU module**
- Reads pilot commands from the **RC receiver**
- Computes control actions on the Teensy (flight logic / stabilization / experiments)
- Outputs **PWM** to ESCs
- Streams telemetry over **UART** to external systems (e.g., Raspberry Pi / logger)

> The **power distribution and regulation** are intentionally isolated on a dedicated board
to improve robustness, power integrity, and maintainability.



## 📦 Repository Contents

- **/hardware/kicad/** → KiCad project (schematic + PCB)
- **/hardware/gerbers/** → manufacturing outputs (Gerbers, drill, etc.)
- **/hardware/bom/** → BOM and assembly references
- **/docs/** → bring-up notes, wiring, revision notes, images
- **/firmware_examples/** → basic Teensy smoke tests (pinout, IMU comms, PWM outputs)



## 🔌 Interfaces (I/O Summary)

| Interface | Description |
|---|---|
| Dual IMU | Sensor upgrade module integrated/connected to this board |
| RC Inputs | Inputs from RF receiver (pilot commands) |
| PWM Outputs | Outputs to ESCs / propulsion channels |
| UART | Telemetry / debugging / companion computer link |
| Teensy Socket | Socketed MCU integration for easy swap/testing |



## 🧪 Bring-up Checklist (Quick)

1. Visual inspection (solder bridges, polarity, connectors)
2. Power-up with current-limited supply (if applicable)
3. Verify **3V3/5V rails** (as designed)
4. Teensy boot + basic pin test
5. Validate **UART link**
6. Validate **RC inputs** (pulse width / edge integrity)
7. Validate **PWM outputs** (servo/ESC signal generation)
8. Validate Dual IMU comms + basic fusion sanity checks

Bring-up notes: see [`/docs/bringup/`](./docs/bringup)

## 📷 Hardware Photos

> Add images under: `docs/images/`
- Top view (assembled)
- Bottom view
- Installed on UAV (if available)
- Connector close-ups (Teensy socket, PWM, RC, UART, IMU)

Example:
```md
![Assembled Board](docs/images/board_assembled.jpg)
```

## 🛣️ Roadmap

- Upload KiCad project + fabrication outputs
- Publish wiring diagram (RC / ESC / UART / IMU)
- Add firmware smoke tests (Teensy + IMU + PWM)
- Flight-test integration notes (with Power Stage board)
-  Optional: EMI/power integrity notes (separation benefits)

## 🔗 Related Repositories

- UAV platform main repository: (add link if you want this to be referenced by DIY_UAV)
- Power stage PCB: UAV Power Distribution & Regulation Board (link to your repo here)
- Dual IMU module repository: (link to your Dual IMU repo here)


⚠️ Disclaimer

This project is shared for educational and experimental purposes.
Use at your own risk. Always follow safe practices when working with UAV propulsion systems.


## 🤝 Support Projects

Support my work on Patreon:  
https://www.patreon.com/c/CrissCCL


## 📜 License

MIT License


