#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define KUB_ID "944db936-47aa-43e8-b404-ef5ca9b15756"
#define WIFI_SSID "SAMBA"
#define WIFI_PASSWORD ""
#define MQTT_SERVER "home.jk-5.nl"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  
  delay(10);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
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
  if(!client.connected()){
    reconnect();
  }
  client.loop();
}

