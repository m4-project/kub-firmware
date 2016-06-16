
WiFiClient espClient;
PubSubClient client(espClient);

bool wasConnected = false;

void initWifi(){
  // Try to connect to the SSID specified in constants.h
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
}

void wifiLoop(){
  // If we are not connected or when the connection has been closed
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Try to connect to the MQTT broker
    if (client.connect("Kub/c90141cb-fff5-4c90-b917-de66816dee9f", "", "")) {
      // Connected to the broker. Subscribe to all topics we need
      client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/kubreq");
      client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/setled");
      client.subscribe("kub/c90141cb-fff5-4c90-b917-de66816dee9f/setmode");
      Serial.println("Connected to mqtt");
    } else {
      // Failed to connect. Try again in 5 seconds
      delay(5000);
    }
  }
  // Read data from the wlan connection
  client.loop();
}

void initMqtt(){
  client.setServer(MQTT_SERVER, 1883); 
}

