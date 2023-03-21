#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Connections.h>
#include <Functions.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* mqtt_server = MQTT_SERVER;
int mqtt_port = MQTT_PORT;
const char* topic = MQTT_TOPIC;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long epochTime; 
unsigned long current_time, prev_time;
uint16_t dt_ms = 10000;

const int LIGHT_PIN = A0;
int LIGHT_VALUE = 0;

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
  setup_wifi(ssid, password);
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
	current_time = millis();

	if ( current_time - prev_time > dt_ms ) {
		prev_time = current_time;
    LIGHT_VALUE = analogRead(LIGHT_PIN);
    epochTime = getTime();

    String payload = "";
    StaticJsonDocument<300> jsonDoc;
    jsonDoc["sensor"] = LIGHT_VALUE;
    jsonDoc["timeStamp"] = epochTime;
    serializeJson(jsonDoc, payload);

    Serial.print("Publish message: ");
    Serial.println(payload);
    client.publish(topic, (char*)payload.c_str());
  }
}