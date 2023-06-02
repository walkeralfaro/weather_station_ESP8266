#include <Arduino.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Connections.h> // retorna la funcion setup_wifi()
#include <Functions.h>
#include <DHT.h>
#include <Wire.h>
#include <SFE_BMP180.h>

SFE_BMP180 bmp180;

bool tickFlag = false;

const char* mqtt_server = MQTT_SERVER;
int mqtt_port = MQTT_PORT;
const char* topic = MQTT_TOPIC;

// MQTT class define
WiFiClient espClient;
PubSubClient client(espClient);

// Sensors class define
DHT dht;

// Epoch UNIX variable
unsigned long timeStamp; 

// DHT11 sensor variables
const int DHT11_PIN = 0;
float humidity;

// LIGHT sensor variables
const int LIGHT_PIN = A0;
int light = D3;

// BMP180 variables
double* TPAarr;

void startMeasurement() {
  tickFlag = true;
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(CLIENT_CNN)) {
      Serial.println("connected");
      client.publish(topic, "reconnected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);

  // Begin I2C communication
  Wire.begin(D2, D1);

  // Setup DHT11
  dht.setup(DHT11_PIN);

  bmp180.begin();

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

	if ( tickFlag ) {

    humidity = dht.getHumidity();
    light = analogRead(LIGHT_PIN);
    timeStamp = getTime();
    TPAarr = getTPA(bmp180);

    String payload = "";
    StaticJsonDocument<300> jsonDoc;
    jsonDoc["timeStamp"] = timeStamp;
    jsonDoc["light"] = light;
    jsonDoc["humidity"] = humidity;
    jsonDoc["temperature"] = round(TPAarr[0] * 100.0) / 100.0;
    jsonDoc["pressure"] = round(TPAarr[1] * 100.0) / 100.0;
    jsonDoc["altitude"] = round(TPAarr[2] * 100.0) / 100.0;
    serializeJson(jsonDoc, payload);

    Serial.print("Publish message: ");
    Serial.println(payload);
    client.publish(topic, (char*)payload.c_str());

    tickFlag = false;
  }
}

