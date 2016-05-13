#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "OneWire.h"

#define KUB_ID "944db936-47aa-43e8-b404-ef5ca9b15756"
#define WIFI_SSID "SAMBA"
#define WIFI_PASSWORD ""
#define MQTT_SERVER "home.jk-5.nl"

WiFiClient espClient;
PubSubClient client(espClient);

OneWire ds(2);

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  
  delay(10);

  //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  //while (WiFi.status() != WL_CONNECTED) {
  //  delay(500);
  //}
  
  //client.setServer(MQTT_SERVER, 1883);
  //client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  if ((char) payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(KUB_ID)) {
      char topic[46];
      sprintf(topic, "kub/%s/hello", KUB_ID);
      client.publish(topic, "hello");
      
      char topic2[48];
      sprintf(topic2, "kub/%s/request", KUB_ID);
      client.subscribe(topic2);
    } else {
      delay(5000);
    }
  }
}

void loop() {
  //if(!client.connected()){
  //  reconnect();
  //}
  //client.loop();
  
  float temperature = getTemp();
  Serial.println(temperature);
 
  delay(100);
}

float getTemp(){
 //returns the temperature from one DS18S20 in DEG Celsius

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
 
}


