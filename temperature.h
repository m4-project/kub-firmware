
// OneWire protocol handler for the temperature sensor

byte sensorType;
byte addr[8];
OneWire ds(2);

void setupTemperatureSensor(){
  // Search any OneWire compatible hardware
  if(!ds.search(addr)){
  	// Nothing was found. Try again in 0.25 seconds
    ds.reset_search();
    delay(250);
    return;
  }

  // Check if the crc sum of the address is valid
  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }

  // Check the sensor type
  switch (addr[0]) {
    case 0x10:
      sensorType = 1;
      break;
    case 0x28:
    case 0x22:
      sensorType = 0;
      break;
    default:
      return;
  } 
}

// Asks the temperature sensor for the temperature
float getTemperature(){
  byte i;
  byte data[12]; // The data buffer
  
  ds.reset();
  ds.select(addr); // Select the temperature sensor at the specified address
  ds.write(0x44, 1); // Activate the sensor
  
  delay(100); // Wait a little while for the sensor to activate
  
  ds.reset();
  ds.select(addr); // Select the temperature sensor at the specified address
  ds.write(0xBE); // Send the request to the sensor

  // Read all the data into the data buffer
  for (i = 0; i < 9; i++){
    data[i] = ds.read();
  }

  // Make a 16 bit signed integer from the first 2 bytes
  int16_t raw = (data[1] << 8) | data[0];
  if (sensorType) {
  	// Shift the first 3 bits away
    raw = raw << 3;
    if(data[7] == 0x10){
      // Read the data
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }else{
    byte cfg = (data[4] & 0x60); // Data length reported by the sensor
    // Depending on the data length, read the amount of available bytes
    if (cfg == 0x00) raw = raw & ~7;
    else if (cfg == 0x20) raw = raw & ~3;
    else if (cfg == 0x40) raw = raw & ~1;
  }
  return (float) raw / 16.0; // Return the raw value divided by 16 (big integer to smaller float with decimals)
}
