// Libraries and secret -file
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <DHT.h> 
#include "secrets.h"
 
// Pin definitions
#define SOIL_MOISTURE_PIN A0 // Analog pin for soil moisture sensor
#define DHT11_PIN 2 // Digital pin for DHT11 sensor
#define PUMP_PIN 3 // Digital pin for pump relay control


// Use values from secrets.h
const char ssid[] = WIFI_SSID;
const char pass[] = WIFI_PASSWORD;

const char broker[] = MQTT_SERVER;
const int mqttPort = MQTT_PORT;
const char clientId[] = CLIENT_ID;
 
// MQTT topics
const char soilMoistureTopic[] = "testing/soilmoisture";
const char airTemperatureTopic[] = "testing/airtemperature";
const char airHumidityTopic[] = "testing/airhumidity";
const char pumpStatusTopic[] = "testing/pumpstatus";

// Wi-Fi and MQTT clients
 
WiFiClient net;
MqttClient client(net);
DHT dht11(DHT11_PIN, DHT11);
 
unsigned long lastPumpActivationTime = 0;
bool pumpActivated = false;
const unsigned long pumpWaitTime = 12000000; // 200 minutes in milliseconds
 
// Function to publish JSON payload to MQTT
void publishToMQTT(const char* topic, const String& payload) {
    client.beginMessage(topic);
    client.print(payload);
    client.endMessage();
}
 
// Function to create JSON payloads
String createJsonPayload(const char* key, float value) {
    return String("{\"") + key + "\":" + String(value) + "}";
}
 
String createJsonPayload(const char* key, int value) {
    return String("{\"") + key + "\":" + String(value) + "}";
}
 
// Function to control the pump based on soil moisture
void controlPump(int moisturePercent) {
    if (moisturePercent < 30 && !pumpActivated) {
        Serial.println("⚠ Soil is too dry! Turning on the pump...");
        digitalWrite(PUMP_PIN, LOW); // Turn on the pump
        delay(3000);
        digitalWrite(PUMP_PIN, HIGH);
        delay(5000);
        digitalWrite(PUMP_PIN, LOW);
        delay(3000);
        digitalWrite(PUMP_PIN, HIGH);
 
        pumpActivated = true;
        lastPumpActivationTime = millis();
 
        // Publish pump status as 1 (on)
        publishToMQTT(pumpStatusTopic, createJsonPayload("pumpActive", 1));
    } else {
        Serial.println("🌊 Soil moisture is sufficient.");
 
        // Publish pump status as 0 (off)
        if (!pumpActivated) {
            publishToMQTT(pumpStatusTopic, createJsonPayload("pumpActive", 0));
        }
    }
}
 
// Arduino intialization

void setup() {
    Serial.begin(9600);
 
    dht11.begin();
 
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, HIGH); // Pump off initially

// Connect to Wi-Fi
 
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(2000);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi!");
    
// Connect to MQTT

    client.setId(clientId);
    Serial.print("Connecting to MQTT broker...");
    if (client.connect(broker, mqttPort)) {
        Serial.println("Connected to MQTT broker!");
    } else {
        Serial.println("Failed to connect to MQTT broker!");
    }
}

// Sensor readings and reconnects

void loop() {
    // Reconnect to MQTT broker if disconnected
    if (!client.connected()) {
        Serial.println("MQTT connection lost. Attempting to reconnect...");
        if (!client.connect(broker, mqttPort)) { 
            Serial.println("Failed to reconnect to MQTT broker!");
            delay(5000);
            return;
        }
    }
    // Read DHT11 sensor
    float humidity = dht11.readHumidity();
    float temperature = dht11.readTemperature();
    // Check if pump is allowed to activate
    if (!pumpActivated || (millis() - lastPumpActivationTime >= pumpWaitTime)) {
    
        if (pumpActivated && (millis() - lastPumpActivationTime >= pumpWaitTime)) {
            pumpActivated = false;
        }

        int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
        int moisturePercent = map(soilMoistureRaw, 250, 500, 0, 100);

        if (isnan(humidity) || isnan(temperature)) {
            Serial.println("⚠ Failed to read from DHT sensor!");
        } else {
            publishToMQTT(soilMoistureTopic, createJsonPayload("moisture", moisturePercent));
            publishToMQTT(airTemperatureTopic, createJsonPayload("temperature", temperature));
            publishToMQTT(airHumidityTopic, createJsonPayload("humidity", humidity));
            // Send values to serial monitor
            Serial.println("====================================");
            Serial.print("🌱 Soil Moisture (%): ");
            Serial.println(moisturePercent);
            Serial.print("💧 Air Humidity (%): ");
            Serial.println(humidity);
            Serial.print("🌡 Air Temperature (°C): ");
            Serial.println(temperature);
            Serial.println("====================================");
        }

        controlPump(moisturePercent);
    } else {
        Serial.println("⏳ Waiting 2 minutes before processing new soil moisture reading...");
    }

    delay(120000);
}
