# 🌦️ Smart Weather Station Using ESP32

An IoT-based weather monitoring system built using an ESP32, DHT11 temperature and humidity sensor, and an LDR light sensor. The system collects environmental data and displays it on a locally hosted web dashboard accessible through a browser.

## 📌 Project Overview

The Smart Weather Station is a low-cost IoT project designed to monitor basic environmental conditions in real time. It uses the ESP32's Wi-Fi connectivity to host a web server that presents sensor readings in a user-friendly format.

The project demonstrates how embedded systems, sensors, wireless communication, and web technologies can work together to build a simple environmental monitoring solution.

## ✨ Features

- **Temperature Monitoring:** Measures ambient temperature using the DHT11 sensor.
- **Humidity Monitoring:** Measures relative humidity using the DHT11 sensor.
- **Light Detection:** Detects bright and dark conditions using an LDR module.
- **Web-Based Dashboard:** Displays sensor readings through a browser.
- **Wireless Connectivity:** Uses the ESP32's built-in Wi-Fi.
- **Local Web Server:** Hosts the dashboard directly on the ESP32 without requiring a separate server.
- **Real-Time Environmental Insights:** Helps users understand the relationship between temperature, humidity, and surrounding light conditions.

## 🛠️ Hardware Requirements

| Component | Purpose |
|---|---|
| ESP32 development board | Main controller and web server |
| DHT11 sensor module | Temperature and humidity measurement |
| LDR sensor module | Light-level detection |
| Breadboard | Circuit prototyping |
| Jumper wires | Electrical connections |
| USB data cable | Programming and power supply |

## 💻 Software and Technologies

- Arduino IDE
- Embedded C/C++ (Arduino framework)
- ESP32 Wi-Fi library (`WiFi.h`)
- SimpleDHT library
- HTML and CSS for the web dashboard
- HTTP for communication between the browser and ESP32

## 🔌 Circuit Connections

### DHT11 Sensor

| DHT11 Pin | ESP32 Pin |
|---|---|
| VCC (+) | 3V3 |
| DATA (OUT) | GPIO 4 |
| GND (-) | GND |

### LDR Sensor Module

Connect the LDR module according to the labels printed on your specific board.

| LDR Module Pin | ESP32 Connection |
|---|---|
| VCC | 3V3 |
| GND | GND |
| Digital Output (DO) | GPIO 34 |

**Important:** Power the LDR module at 3.3 V to help ensure its output is safe for the ESP32's GPIO. This configuration detects bright/dark states rather than providing a calibrated numerical light-intensity measurement.

## ⚙️ How It Works

1. **Initialization:** The ESP32 initializes the sensors and Wi-Fi connection.
2. **Network Connection:** The ESP32 connects to the configured Wi-Fi network.
3. **Web Server:** It starts an HTTP server and prints its local IP address to the Serial Monitor.
4. **Sensor Reading:** The DHT11 provides temperature and humidity readings, while the LDR module reports a digital light state.
5. **Data Presentation:** When a browser requests the webpage, the ESP32 responds with an HTML dashboard containing the available sensor readings.
6. **Environmental Interpretation:** The readings can help users understand the current indoor environment, although they do not constitute a professional weather forecast.

## 🚀 Getting Started

### 1. Install the Required Software

Install the Arduino IDE and add ESP32 board support using the Arduino IDE Boards Manager. Install the SimpleDHT library through the Library Manager.

### 2. Clone the Repository

```bash
git clone https://github.com/pranjal0526/Smart_weather_station.git
cd Smart_weather_station
```

### 3. Configure Wi-Fi

Open `weather_station.ino` and update the Wi-Fi credentials:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

Use a compatible 2.4 GHz Wi-Fi network or phone hotspot.

### 4. Select the Board and Port

In Arduino IDE, select **ESP32 Dev Module** and the serial port corresponding to your ESP32.

### 5. Upload the Code

Connect the ESP32 using a USB data cable, compile the sketch, and upload it to the board.

### 6. Open the Dashboard

Open Serial Monitor at the baud rate specified in the code. After the ESP32 connects to Wi-Fi, copy the displayed IP address into a browser on a device connected to the same network.

Example: `http://192.168.1.22`

The actual IP address may differ depending on the network.

## 📊 Understanding the Measurements

| Measurement | What It Tells You |
|---|---|
| Temperature (°C) | The surrounding air temperature |
| Relative humidity (%) | The amount of water vapour in the air relative to saturation |
| LDR digital state | Whether the light sensor detects a bright or dark condition according to its output polarity and threshold |

Temperature and humidity can help describe indoor comfort conditions. Light detection can help identify whether an area is brightly lit or dark.

The DHT11 is a basic sensor, and the LDR module is not a calibrated lux meter. Sensor readings should be treated as approximate environmental observations rather than professional meteorological measurements.

## 🎯 Applications

- Indoor environmental monitoring
- Classroom and laboratory demonstrations
- IoT and embedded-systems learning
- Basic smart-room automation experiments
- Demonstrating sensor data collection and web-based visualization

## 🔮 Future Improvements

- Add a calibrated light-intensity sensor for lux measurements.
- Store historical readings and display graphs.
- Add data logging and downloadable reports.
- Introduce configurable thresholds and alerts.
- Add a barometric pressure sensor for additional weather-related measurements.
- Improve sensor error handling and display sensor status on the dashboard.
- Deploy remote monitoring through a cloud platform.

## ⚠️ Limitations

- The DHT11 provides basic temperature and humidity measurements with limited accuracy.
- The current digital LDR setup reports light states, not precise light intensity.
- The local dashboard generally requires the viewing device to be connected to the same network.
- The system does not currently measure rainfall, wind speed, wind direction, or atmospheric pressure.
- It should not be considered a professional weather forecasting instrument.

## 📚 Learning Outcomes

This project demonstrates:

- Interfacing sensors with an ESP32
- Reading digital sensor data
- Connecting embedded devices to Wi-Fi
- Hosting a local HTTP web server
- Generating web content using HTML and CSS
- Integrating hardware and software into an IoT prototype

## 👨‍💻 Author

**Pranjal Pandey**

GitHub: [@pranjal0526](https://github.com/pranjal0526)

## 📄 License

This project is available for educational and learning purposes. A formal open-source license can be added to the repository if redistribution and reuse permissions need to be explicitly defined.
