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

 **Water Pump:**

   
  **Soil Moisture Sensor:**

   
**DHT11 Sensor:**


**Relay Module:**
  

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

- **Services Running in Docker (MTIG Stack):**
  - **Mosquitto** (MQTT Broker)
  - **Telegraf** (Data collector)
  - **InfluxDB** (Time-series database)
  - **Grafana** (Visualization platform)

**MTIG Stack Setup Based On:**  
[docker-compose-mosquitto-influxdb-telegraf-grafana](https://github.com/Miceuz/docker-compose-mosquitto-influxdb-telegraf-grafana) by [Miceuz](https://github.com/Miceuz) and [AkaJuliaan](https://github.com/akaJuliaan)

---
