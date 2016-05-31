
WiFiClient espClient;
PubSubClient client(espClient);

bool wasConnected = false;

void initWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
}

void wifiLoop(){
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", "", "")) {
      //Connected
      client.subscribe("kub/1234/kubreq");
      client.subscribe("kub/1234/setled");
      client.subscribe("kub/1234/setmode");
      Serial.println("Connected to mqtt");
    } else {
      //Failed
      delay(5000);
    }
  }
  client.loop();
}

void initMqtt(){
  client.setServer(MQTT_SERVER, 1883); 
}

