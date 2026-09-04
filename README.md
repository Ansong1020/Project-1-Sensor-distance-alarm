# Project 1: Sensor Distance Alarm

A distance alarm that detects when an object is too close and sounds an alarm.

## Hardware

- ESP32-S3 development board
- HC-SR04 ultrasonic distance sensor
- 16×2 I2C LCD display
- Passive buzzer
- 74HC595 shift register + 8-LED bar
- Breadboard and jumper wires

## How it works
The program runs in a continuous loop:

1. **Measure** — the ultrasonic sensor sends a trigger pulse, then times how long the echo takes to return. Distance is calculated from the round-trip time and the speed of sound.
3. **Display** — the distance is shown live on the LCD, and the 8-LED bar lights up progressively as an object gets closer (from 100 cm down to 8 cm).
4. **Alarm** — if the distance is above 0 and below **10 cm**, the buzzer turns on; otherwise it stays off.
