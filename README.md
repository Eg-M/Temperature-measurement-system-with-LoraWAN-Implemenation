# Temperature Measurement System

Arduino firmware for reading two DS18B20 temperature sensors with an Arduino MKR WAN 1310. It reports the sensor readings over the serial connection.

> **Current scope:** Despite the repository name, the current firmware does not implement LoRaWAN transmission. It reads the sensors and writes their temperatures to serial.

## Hardware

- Arduino MKR WAN 1310
- Two DS18B20 temperature sensors
- A 4.7 kΩ pull-up resistor between the OneWire data line and 3.3 V

Connect both sensors to the same OneWire bus:

| DS18B20 | MKR WAN 1310 |
| --- | --- |
| VDD | 3.3 V |
| GND | GND |
| DATA | D4 |

Use the same shared DATA line for both sensors and connect the pull-up resistor between DATA and 3.3 V. Check the sensor pinout before wiring.

## Build and upload

This is a PlatformIO project targeting the MKR WAN 1310. Install PlatformIO Core or the PlatformIO IDE, connect the board, then run these commands from the project directory:

```sh
pio run
pio run --target upload
pio device monitor --baud 9600
