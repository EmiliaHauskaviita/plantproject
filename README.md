# JES - Automated Plant Watering and Monitoring System

## Overview

JES is an automatic plant watering and monitoring system designed to ensure optimal plant care with real-time monitoring capabilities. Built around an Arduino Uno R4 WiFi microcontroller, it utilizes analog sensors to measure air humidity, air temperature, and soil moisture. When the soil moisture drops below a set threshold, the system automatically activates a pump to water the plant. The collected data is processed through a pipeline hosted on a virtual machine (VM) in CSC’s cPouta cloud infrastructure. Essential services, including Mosquitto, Telegraf, InfluxDB, and Grafana, run within Docker containers as part of the MTIG stack, with Nginx serving as a reverse proxy to provide secure HTTPS access to Grafana.

Made for Haaga-Helia [ICT Infrastructure project - ICI008AS3A-3002 (10 credits)](https://opinto-opas.haaga-helia.fi/course_unit/PRO4TF023).

---

## Physical Parts

- **Plastic PVC Hose (5 mm):** [Puuilo product link](https://www.puuilo.fi/muoviletku-kirkas-5mm#tab-label-description)
- **Water Pump (3–24V DC):** [Partco product link](https://www.partco.fi/fi/saehkoemekaniikka/moottorit/dc-moottorit/23144-mot-pmma19004a001.html)
- **Arduino Uno R4 WiFi (ABX00087):** [Partco product link](https://www.partco.fi/fi/arduino/arduino-classic/26323-arduino-uno-r4w.html)
- **Arduino Starter Kit (Basic):** [Partco product link](https://www.partco.fi/fi/arduino/arduino-aloitussarjat/21389-ard-aak39525k.html)  
  *(Includes breadboard, 5V relay module, DHT11 sensor, soil moisture sensor, and jumper wires)*

## Wiring 

Arduino to breadboard:

- Arduino VCC 5,5V - breadboard red lane
  
- Arduino GND - breadboard blue lane
  

 **Water Pump:**

 - The pump’s negative terminal is connected to the power supply’s negative terminal.
 
 - The power supply’s positive terminal is connected to the relay’s COM (common) terminal.
 
 - The pump’s positive terminal is connected to the relay’s NO (normally open) terminal.


  **Soil Moisture Sensor:**
  
  - Sensor GND pin - Arduino GND
    
  - Sensor VCC pin - Arduino 3,3V VCC
    
  - Sensor OUT pin - Arduino Analog in A0

   
**DHT11 Sensor:**

- The DHT11 sensor is connected to the breadboard at positions I11–I14.

- Breadboard I11 is VCC (power supply).

- Breadboard I12 is OUT (data output).

- Breadboard I13 is NC (not connected).

- Breadboard I14 is GND (ground).

- Breadboard H12 (data output) is connected to Arduino’s digital pin 2.

- Breadboard F14 is linked to the breadboard’s blue lane, which serves as GND (ground).

- VCC (5.5V) is supplied through the breadboard and connects to breadboard position B12.

- Breadboard C12 (data output) is connected to breadboard F11 (power supply).

- Breadboard D12 is connected to F12 with resistor. This resistor is used to limit current flow and adjust voltage levels in a circuit. 


**Relay Module:**

- Realy VCC - breadboard red lane (VCC).

- Realy GND - breadboard blue lane (GND).

- Relay IN - Arduino Digital 3.
  

  ---

## Software

**Arduino Sketch:** `code.ino`

- **Libraries Used:**
  - `WiFiS3.h` (for Wi-Fi connectivity)
  - `PubSubClient.h` (for MQTT communication)
  - `DHT.h` (for DHT11 sensor handling)

---

## Virtual Machine Setup 

- **Cloud Provider:** [CSC’s cPouta IaaS environment](https://docs.csc.fi/cloud/pouta/)
- **VM Specifications:**
  - **Flavor:** `standard.large`
  - **VCPUs:** 4
  - **RAM:** 7.8 GB
  - **Disk Size:** 80 GB
  - **OS Image:** Ubuntu 24.04 LTS

- **Service Running Directly on VM:**
  - **Nginx** (Reverse proxy with HTTPS)
       - Nginx acts as a reverse proxy, securely routing traffic to the Grafana instance running in Docker and ensuring encrypted access via HTTPS at https://jeid.live.

- **Services Running in Docker (MTIG Stack):**
  
  The setup is based on the [docker-compose-mosquitto-influxdb-telegraf-grafana](https://github.com/Miceuz/docker-compose-mosquitto-influxdb-telegraf-grafana) by [Miceuz](https://github.com/Miceuz) and [AkaJuliaan](https://github.com/akaJuliaan).

  - **Mosquitto** (MQTT Broker)
     - Mosquitto acts as the MQTT broker, enabling communication from the Arduino Uno R4 WiFi (with ESP32) to the VM and other services.
     - The following MQTT topics handle the sensor data:
        - `testing/soilmoisture` –  Provides soil moisture sensor readings.
        - `testing/airhumidity` – Delivers air humidity data from the DHT11 sensor.
        - `testing/airtemperature` – Sends air temperature readings from the DHT11 sensor.
        - `testing/pumpstatus` – Reports pump activation when the system triggers the pump to water the plant (based on soil moisture falling below a predefined threshold).
          
  - **Telegraf** (Data collector)
       - Subscribes to the MQTT topics to listen for incoming messages.
       - Parses the JSON-formatted messages from the ESP32 (via MQTT) and transforms them into a format suitable for processing and storage in InfluxDB.
  - **InfluxDB** (Time-series database)
       - Receives the parsed data from Telegraf and stores sensor readings and pump activation logs in a time-series database bucket.
  - **Grafana** (Visualization platform)
       - Queries the data to create interactive dashboards, visualizing both real-time and historical sensor data and pump activation logs.
       - In individual and combined graph, data is aggregated over 10-minute intervals using the mean function, which averages the last 5 data points (published every 2 minutes) to provide more stable values.
         - Gauges: Three real-time gauges display the current readings for air humidity, air temperature, and soil moisture.
         - Individual graphs: Separate graphs show each sensor's values individually for more detailed analysis.
         - Combined graph: A graph displaying all three sensor readings together for easy comparison
         - Pump activation: Displays pump activation events in chronological order, showing when the pump was triggered.
           
   ---
