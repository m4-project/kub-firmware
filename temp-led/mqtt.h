
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
    if (client.connect("Kub/c90141cb-fff5-4c90-b917-de66816dee9f", "", "")) {
      //Connected
      client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/kubreq");
      client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/setled");
      client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/setmode");
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

