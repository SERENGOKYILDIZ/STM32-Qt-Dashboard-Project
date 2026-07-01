# 🛰️ STM32 & Qt Full-Duplex UART Communication Project

This project is an industrial-inspired embedded systems application that enables **real-time bidirectional (Full-Duplex) UART** communication between an **STM32F407** microcontroller and a desktop Graphical User Interface (GUI) developed using **Qt 6 (C++)**.

The system features two main functionalities:
1. **Telemetry (Data Acquisition & Transmission):** 12-bit ADC values read from a potentiometer connected to the STM32's `PA0` pin are packaged and transmitted to the PC every 200 ms. The Qt GUI visualizes this incoming real-time telemetry data using a progress bar (`QProgressBar`).
2. **Control (Command Reception):** Commands sent from the Qt GUI trigger UART interrupts on the STM32, dynamically controlling the built-in LED connected to pin `PD12`.

## 👨‍💻 Author

**Author:** Semi Eren Gökyıldız
- **Email:** [gokyildizsemieren@gmail.com](mailto:gokyildizsemieren@gmail.com)
- **GitHub:** [SERENGOKYILDIZ](https://github.com/SERENGOKYILDIZ)
- **LinkedIn:** [Semi Eren Gökyıldız](https://www.linkedin.com/in/semi-eren-gokyildiz/)

## 🛠️ Tech Stack & Tools

### Embedded Firmware
*   **Microcontroller:** STM32F407VGT6 (ARM Cortex-M4)
*   **Development Environment:** VS Code & STM32 VS Code Extension
*   **Hardware Abstraction:** STM32 HAL Library (configured via STM32CubeMX)
*   **Toolchain:** GCC ARM Embedded Toolchain

### Desktop Software
*   **Framework:** Qt 6
*   **Language:** C++ (C++17/20 Standards)
*   **Build System:** CMake
*   **Modules:** `QSerialPort` (Serial Communication Interface)

---

## 🔌 Hardware Connections

A **USB-to-UART converter (FTDI / CH340 / CP2102)** is used to establish serial communication between the STM32F407 board and the PC.

### Pinout Table

| STM32F407 Pin | USB-to-UART Pin | Function | Description |
| :--- | :--- | :--- | :--- |
| **PA2 (USART2_TX)** | RXD | Transmit | Outgoing data stream from STM32 to PC |
| **PA3 (USART2_RX)** | TXD | Receive | Incoming command stream from PC to STM32 |
| **GND** | GND | Ground | Common reference ground |
| **PA0** | Potentiometer wiper (center) | ADC Input (Channel 0) | Analog voltage level reading (0 - 3.3V) |
| **PD12-PD15** | Built-in LEDs | GPIO Output | Status LEDs (controlled via commands) |

> [!WARNING]
> Ensure that the ground pin (GND) of the STM32 board is connected to the ground pin (GND) of the USB-to-UART converter. Failure to do so will result in transmission instability, parity errors, and noise.

---

## 📡 Communication Protocol

Communication is asynchronous and operates at **115200 Baudrate** in Full-Duplex mode.

### UART Configuration
*   **Baudrate:** 115200 bps
*   **Data Bits:** 8
*   **Parity:** None
*   **Stop Bits:** 1
*   **Flow Control:** None

### Frame Structure

#### 1. Telemetry: STM32 ➡️ PC
The STM32 reads the 12-bit ADC value (ranging from `0` to `4095`), formats it into a string, and transmits it every 200 ms.
*   **Format:** `S:<Value>\n` (e.g., `S:2048\n`)
*   `S:` Start Header (Prefix identifier)
*   `<Value>` ASCII-encoded ADC value (`0` - `4095`)
*   `\n` Carriage Return / Newline character (Packet Delimiter)

#### 2. Commands: PC ➡️ STM32
Commands sent from the desktop application are handled instantly on the STM32 via non-blocking UART receive interrupts (`HAL_UART_RxCpltCallback`).
*   **"1":** Sets the PD12-15 pins high (`SET`) ➡️ LED ON.
*   **"0":** Sets the PD12-15 pins low (`RESET`) ➡️ LED OFF.

---

## 📁 Repository Structure

The project separates the hardware firmware and the desktop software into distinct, clean-architecture directories:

```
STM32 & QT First Project/
├── .gitignore               # Unified Git ignore rules
├── README.md                # Project documentation
│
├── QT_STM32_First/          # Qt 6 Desktop GUI App (CMake)
│   ├── CMakeLists.txt       # CMake build configuration
│   ├── main.cpp             # Application entry point
│   ├── mainwindow.h         # Main window header
│   ├── mainwindow.cpp       # Main window logic (QSerialPort handling)
│   ├── mainwindow.ui        # UI layout design
│   └── .qtcreator/          # Qt Creator local workspace settings
│
└── STM32_QT_First/          # STM32F407 Firmware Project (VS Code & CubeMX)
    ├── STM32_QT_First.ioc   # CubeMX configuration file
    ├── Core/
    │   ├── Src/
    │   │   ├── main.c       # ADC loop and UART Interrupt callback logic
    │   │   └── stm32f4xx_it.c # Interrupt vector table and ISR subroutines
    │   └── Inc/
    │       └── main.h       # Peripheral declarations and macro definitions
    └── Drivers/             # STM32 HAL and CMSIS driver libraries
```

---

## 💻 Architecture & Workings

### STM32 Firmware Logic
*   **Analog-to-Digital Conversion:** The ADC reads values using the polling method. During each loop iteration, `HAL_ADC_Start` and `HAL_ADC_PollForConversion` capture the voltage on pin `PA0`. The 12-bit raw integer is formatted into `S:<Value>\n` using `sprintf` and sent to the PC via `HAL_UART_Transmit`.
*   **Interrupt-Driven Reception:** To maximize efficiency, receiving command bytes is offloaded to hardware interrupts. `HAL_UART_Receive_IT` keeps the interrupt listener active. When a byte arrives, `HAL_UART_RxCpltCallback` triggers. If the byte matches `'1'` or `'0'`, the state of pins `PD12-15` is updated, and the interrupt is immediately re-enabled for the next byte.

### Qt GUI Application Logic
*   **Serial Port Management:** `QSerialPort` handles COM port configuration (defaulting to `COM11` in the source code) in asynchronous mode.
*   **Connection Status Display:** The status of the serial connection is displayed on a `QLabel` (`lblDurum`) in English:
    *   On successful connection: `"Connected (COM11)"` / `"Connected"`
    *   On connection failure: `"Connection Failed"` / `"Disconnected"`
*   **Data Parsing:** As data bytes arrive, `readyRead` triggers `readSerial()`. The incoming bytes are accumulated in a `QByteArray` buffer. Once the newline delimiter (`\n`) is detected, the packet is parsed. The prefix `S:` is stripped, and the extracted value is mapped to update the `QProgressBar` (`barSensor`) within the range of `0` to `4095`.
*   **Command Transmission:** Clicking "LED YAK" (`btnLedYak`) or "LED SÖNDÜR" (`btnLedSondur`) sends the string `"1"` or `"0"` respectively to the serial port.

---

## 🚀 Getting Started

### Prerequisites
*   STM32F407 Discovery or compatible development board.
*   Keil uVision, STM32CubeIDE, or VS Code with ARM GCC compiler.
*   Qt Creator (Qt 6.x installed) & CMake.
*   USB-to-UART converter module.

### How to Run

1.  **Hardware Connection Setup:**
    *   Connect `PA2`, `PA3`, and `GND` of the STM32 to the converter's `RXD`, `TXD`, and `GND` respectively.
    *   Plug the converter into the PC and note the COM port number (e.g., using Device Manager).
    *   Connect the center pin of a potentiometer to `PA0`. Connect the remaining pins to `3.3V` and `GND`.

2.  **Flash the Microcontroller:**
    *   Open `STM32_QT_First` in your IDE/environment.
    *   Build the project and flash the resulting `.bin`/`.elf` binary onto the board using an ST-LINK programmer.

3.  **Run the Qt Application:**
    *   Open `QT_STM32_First` in Qt Creator.
    *   In `mainwindow.cpp`, replace `"COM11"` with your actual COM port.
    *   Configure with CMake, build, and run the project.
    *   Verify that the connection label displays connection status in English and the progress bar updates when you turn the potentiometer.
