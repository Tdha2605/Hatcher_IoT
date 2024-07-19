# Hatcher_IoT


This project is an incubator monitoring and control system using an ESP32, a DHT22 temperature sensor, an RTC (DS1307), an I2C LCD, and various buttons for user input. The system allows you to set the start time, the expected hatch time, and the target temperature. It monitors the temperature and controls a heating element and a fan to maintain the desired temperature. The system also provides alerts when the hatch time is approaching.

## Features

- Set start time and expected hatch time using buttons
- Set target temperature
- Display current temperature, elapsed time, and remaining time to hatch on an LCD
- Control heating element and fan to maintain the desired temperature
- Connect to WiFi and provide status updates
- Buzzer alerts for hatch time

## Components

- ESP32
- DHT22 Temperature Sensor
- DS1307 RTC Module
- I2C LCD (16x2)
- Buttons (5 buttons: Menu, Up, Down, Left, Right)
- Buzzer
- Relay Modules for heating element and fan

## Pin Configuration

- **Buzzer**: GPIO 15
- **Menu Button**: GPIO 12
- **Up Button**: GPIO 13
- **Down Button**: GPIO 14
- **Left Button**: GPIO 25
- **Right Button**: GPIO 26
- **Temperature Sensor (DHT22)**: GPIO 4
- **Relay for Heating Element**: GPIO 16
- **Relay for Fan**: GPIO 17
- **I2C LCD SDA**: GPIO 21
- **I2C LCD SCL**: GPIO 22
