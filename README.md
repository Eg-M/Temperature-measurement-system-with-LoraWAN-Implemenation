# Temperature Measurement System with LoRaWAN Implementation

## Overview
This project is a temperature measurement system using LoRaWAN for remote data transmission. It's ideal for monitoring in distant locations, taking advantage of LoRaWAN's long-range communication.

## Hardware Requirements
- MKR WAN 1300 or compatible LoRaWAN board
- DS18B20 or similar temperature sensor
- Basic wiring and power supply components

## Software Dependencies
```cpp
MKRWAN.h (for LoRaWAN connectivity)
CayenneLPP.h (for payload formatting)
OneWire.h (for communication with temperature sensor)
DallasTemperature.h (for interfacing with Dallas temperature sensors)
ArduinoLowPower.h (for power management)
arduino_secrets.h (for storing sensitive data like keys)
```

## Configuration

Update arduino_secrets.h with your LoRaWAN keys and credentials.
Ensure proper wiring between the temperature sensor and the MKR WAN board.

## Usage
The system reads temperature data from the sensor.
Data is formatted using CayenneLPP.
LoRaWAN is used to transmit data to a configured network.
Low power features are employed for energy efficiency in remote deployments.

## Extending the Project
Integrate additional sensors for more environmental data.
Enhance power management for longer deployment life.
Implement data analysis tools for the received data.
