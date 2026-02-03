# 🧠🛩️ UAV Control & Navigation Board — Teensy Carrier + Dual IMU (I2C)

![Hardware](https://img.shields.io/badge/Hardware-Control%20%26%20Navigation-blue)
![PCB](https://img.shields.io/badge/PCB-KiCad-lightgrey)
![IMU](https://img.shields.io/badge/IMU-BMI088%20%2B%20ICM--42605-orange)
![Bus](https://img.shields.io/badge/Bus-I2C-lightgrey)
![Teensy](https://img.shields.io/badge/MCU-Teensy%204.x-green)
![RF](https://img.shields.io/badge/RF-1%20RC%20Input-lightgrey)
![PWM](https://img.shields.io/badge/PWM-4%20Outputs-lightgrey)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

This repository documents the **updated control stage PCB** for a custom UAV platform.
It upgrades the previous **single IMU (MPU6050)** to a **dual-IMU sensing stack**
(**BMI088 + ICM-42605**) over **I2C**, while preserving the essential flight I/O:

- **1× RC input** (radio control)
- **4× PWM outputs** (ESC / propulsion control)
- **UART** interface (telemetry / debug / companion link)
- **Teensy socket** (carrier-style integration)

✅ This control stage is **intentionally separated** from the power board:

- **Power distribution & regulation** lives on a dedicated PCB
  (*UAV Power Distribution & Regulation Board — separate repo, already developed and installed*)


---

## ✨ Key Features

- **Dual IMU upgrade**: **BMI088 + ICM-42605** (I2C)
- **Teensy carrier board** with socketed MCU integration
- **1× RC input** interface for pilot command
- **4× PWM outputs** for ESCs / propulsion
- **UART** header for telemetry / debugging / external integration
- Modular UAV architecture: **control stage isolated from power stage**

---

## 🧩 System Role (Modular UAV Architecture)

This PCB implements the **control + sensing + I/O hub** of the UAV:

- Reads inertial measurements from the **Dual IMU** via **I2C**
- Reads pilot command from **1 RC input**
- Runs control / stabilization logic on **Teensy**
- Drives propulsion through **4 PWM outputs**
- Streams telemetry through **UART** when required

> This board corresponds to the **matured control electronics revision**
used in a **v5 control architecture development cycle**.
Given the current maturity and prior validation, the board is intended to be
**installed directly on the UAV** for full-system integration testing.

---

## 🔌 Interfaces (I/O Summary)

| Interface | Quantity | Notes |
|---|---:|---|
| Dual IMU (I2C) | 1 | **BMI088 + ICM-42605** sensing stack |
| RC Input | 1 | Radio control command input |
| PWM Outputs | 4 | ESC / propulsion control channels |
| UART | 1 | Telemetry / debug / companion computer |
| Teensy Socket | 1 | Teensy 4.x carrier integration |

---

## 📦 Repository Contents

- **/hardware/kicad/** → KiCad project (schematic + PCB)
- **/hardware/gerbers/** → manufacturing outputs (Gerbers, drill, etc.)
- **/hardware/bom/** → BOM and assembly references
- **/docs/** → images, wiring notes, revision notes (optional)
- **/firmware_examples/** → minimal interface tests (optional)

---

## 📷 Hardware Photos

Add images under: `docs/images/`

Example:
```md
![Assembled Board](docs/images/board_assembled.jpg)
![Installed on UAV](docs/images/installed_on_uav.jpg)
```

## 🔗 Related Repositories

- UAV platform main repository: (link your DIY_UAV repo)
- Power stage PCB: (link your UAV Power Distribution & Regulation Board repo)
- Dual IMU module repository: (link your Dual IMU repo if it’s separate)

⚠️ Disclaimer

This project is shared for educational and experimental purposes.
Use at your own risk. Always follow safe practices when working with UAV propulsion systems.


## 🤝 Support Projects

Support my work on Patreon:  
https://www.patreon.com/c/CrissCCL


## 📜 License

MIT License


