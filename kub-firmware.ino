#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "constants.h"
#include "mqtt.h"
#include "temperature.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2, 14, NEO_GRB + NEO_KHZ800);

byte mode = 1;

void setup() {
  Serial.begin(115200);
  pixels.begin();
  
  pinMode(4, INPUT);
  
  setupTemperatureSensor();
  initWifi();
  initMqtt();
  
  client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/kubreq");
  client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/setled");
  client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/setmode");
  client.setCallback(mqttCallback);
  
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.setPixelColor(1, pixels.Color(0, 255, 0));
  pixels.show();
  
  delay(500);
  
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.setPixelColor(1, pixels.Color(0, 0, 0));
  pixels.show();
}

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
  wifiLoop();
  refreshMode();
}

void refreshMode(){
  if(mode == 0){
    
  }else if(mode == 1){
    float temp = getTemperature();
    if(temp > 65){
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    }else if(temp < 30){
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
    }else{
      float factor = (temp - 30) / (float) 35;
      byte value = (factor * 255);
      pixels.setPixelColor(0, pixels.Color(value,0,255 - value));
      pixels.setPixelColor(1, pixels.Color(value,0,255 - value));
    }
    pixels.show();
    delay(100);
  }
}

