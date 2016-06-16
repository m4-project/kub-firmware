#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "constants.h"
#include "mqtt.h"
#include "temperature.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2, 14, NEO_GRB + NEO_KHZ800);

byte mode = 1; // By default on mode 1 (Automatic led control based on temperature)

void setup() {
  Serial.begin(115200);
  pixels.begin();
  
  pinMode(4, INPUT);
  
  // Initialise the OneWire protocol for the temperature sensor and connect to wlan/mqtt
  setupTemperatureSensor();
  initWifi();
  // initWifiWPS(); -- WPS Is considered harmful. It does work, but i do not have any WPS-enabled equipment. Also, for the presentation, we use the hardcoded SSID
  initMqtt();
  
  // Subscribe on the topics
  client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/kubreq");
  client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/setled");
  client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/setmode");
  client.setCallback(mqttCallback);
  
  // Flash the leds green, so the user knows we are done initialising
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.setPixelColor(1, pixels.Color(0, 255, 0));
  pixels.show();
  
  delay(500);
  
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.setPixelColor(1, pixels.Color(0, 0, 0));
  pixels.show();
}

// Callback for the MQTT client. Will handle any message sent by the broker
void mqttCallback(char* topic_b, byte* payload, unsigned int length){
  if(length < 5){
    return;
  }
  byte protocolVersion = payload[0];
  uint32_t payloadLength = payload[4] + (payload[3] << 8) + (payload[2] << 16) + (payload[1] << 24);
  String topic = String(topic_b);
  if(String(topic) == "kub/c90141cb-fff5-4c90-b917-de66816dee9f/kubreq"){
    if(payloadLength < 3){
      return;
    }
    byte request = payload[5];
    uint32_t temp = (uint32_t) getTemperature();
    byte* response = new byte[11]{1, 0, 0, 0, 6, payload[6], payload[7], (temp >> 24) & 0xFF, (temp >> 16) & 0xFF, (temp >> 8) & 0xFF, temp & 0xFF};
    client.publish("kub/c90141cb-fff5-4c90-b917-de66816dee9f/kubres", response, 11);
  }else if(String(topic) == "kub/c90141cb-fff5-4c90-b917-de66816dee9f/setled"){
    if(payloadLength < 4){
      return;
    }
    if(mode == 0){
      byte ledid = payload[5];
      byte r = payload[6];
      byte g = payload[7];
      byte b = payload[8];
      pixels.setPixelColor(ledid, pixels.Color(r, g, b));
      pixels.show();
    }
  }else if(topic == "kub/c90141cb-fff5-4c90-b917-de66816dee9f/setmode"){
    if(payloadLength < 1){
      return;
    }
    mode = payload[5];
  }
}

void loop() {
  wifiLoop(); // Check if wlan connection is still active, and try to receive data
  refreshMode(); // Refresh the mode for the leds
}

void refreshMode(){
  if(mode == 0){
    // Mode 0 is manual control. No special handling needed
  }else if(mode == 1){
  	// Mode 1 is automatic temperature control for the leds. Scaling between 30 and 65 degrees celsius
    float temp = getTemperature();
    if(temp > 65){
      // If hotter than 65 degrees, just make the leds red to prevent byte overflows
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    }else if(temp < 30){
      // If colder than 30 degrees, just make the leds blue to prevent byte underflows
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
    }else{
      // Scale the color between 30 and 65 degrees
      float factor = (temp - 30) / (float) 35;
      byte value = (factor * 255);
      pixels.setPixelColor(0, pixels.Color(value,0,255 - value));
      pixels.setPixelColor(1, pixels.Color(value,0,255 - value));
    }
    pixels.show();
    delay(100);
  }
}

