#include <Connections.h>
#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <Ticker.h>

int measurementTimeValue;
Ticker timer;
extern void startMeasurement();


void setup_wifi() {

  // Init WiFimanager
  WiFiManager wifiManager;

  // wifiManager.resetSettings();

  WiFiManagerParameter customParam("variable", "Measurement Time (5, 10, 15 or 30)", "15", 2);
  wifiManager.addParameter(&customParam);

  wifiManager.setTimeout(180);

  if(!wifiManager.autoConnect("WeatherStation")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again
    ESP.reset();
    delay(5000);
  } 

  String customParamValue = customParam.getValue();
  measurementTimeValue = customParamValue.toInt();
  if (measurementTimeValue != 5 && measurementTimeValue != 10 && measurementTimeValue != 15 && measurementTimeValue != 30) {
    measurementTimeValue = 15;
  }

  Serial.println("connected...");
  Serial.print("Measurement time: ");
  Serial.println(measurementTimeValue);

  int measurementTimeValueMinuts = measurementTimeValue * 60;

  timer.attach(measurementTimeValueMinuts, startMeasurement);


}
