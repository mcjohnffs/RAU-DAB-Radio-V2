# Hard- und Software Konfiguration

## Hardware

- Menübedienung über Touch-Funktion des Displays oder per Eingabe von Buttons und Encoder
- Statusanzeigen über addresierbare RGB-Leds
- Steuerung der weiteren Module des DAB-Radios
    - Bluetooth Modul - BM83 (UART)
    - Sound Prozessor - BD37544FS (I2C)
    - LiPo Charger - bq25895 (I2C)
    - Fuel Gauge - bq27441-G1 (I2C)
- inkl. erweiterbare I/O's per Port Expander - MCP23017 (I2C)


## Software

- Basis Code in C++ (Arduino)
- Hardware API per (Arduino ESP32 Core)
- Integriertes Echtzeitbetriebssystem im Core (FreeRTOS)
- GUI per (GuiSlice) Arduino Library


