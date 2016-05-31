
byte sensorType;
byte addr[8];
OneWire ds(2);

void setupTemperatureSensor(){
  if(!ds.search(addr)){
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }
 
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

float getTemperature(){
  byte i;
  byte data[12];
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);
  
  delay(100);
  
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);


  for (i = 0; i < 9; i++){
    data[i] = ds.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  if (sensorType) {
    raw = raw << 3;
    if(data[7] == 0x10){
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }else{
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;
    else if (cfg == 0x20) raw = raw & ~3;
    else if (cfg == 0x40) raw = raw & ~1;
  }
  return (float) raw / 16.0;
}
