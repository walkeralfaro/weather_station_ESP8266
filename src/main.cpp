#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* mqtt_server = MQTT_SERVER;
int mqtt_port = MQTT_PORT;
const char* topic = MQTT_TOPIC;

WiFiClient espClient;
PubSubClient client(espClient);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, UDP_SERVER);

unsigned long epochTime; 

unsigned long lastMsg = 0;
int value = 0;

int fotopin=A0;
int fotoval = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
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
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  unsigned long now = millis();


  if (now - lastMsg > 30000) {
    lastMsg = now;
    fotoval = analogRead(fotopin);
    epochTime = getTime();

    String payload = "";
    StaticJsonDocument<300> jsonDoc;
    jsonDoc["sensor"] = fotoval;
    jsonDoc["timeStamp"] = epochTime;
    serializeJson(jsonDoc, payload);

    Serial.print("Publish message: ");
    Serial.println(payload);
    client.publish(topic, (char*)payload.c_str());
  }
}