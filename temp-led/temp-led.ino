#include <Adafruit_NeoPixel.h>
#include <OneWire.h>

OneWire ds(2);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, 14, NEO_GRB + NEO_KHZ800);

byte type_s;
byte addr[8];

void setup() {
  Serial.begin(9600);
  pixels.begin();
  
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
      type_s = 1;
      break;
    case 0x28:
    case 0x22:
      type_s = 0;
      break;
    default:
      return;
  } 
}

void loop() {
  float temp = getTemperature();
  if(temp > 40){
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  }else if(temp < 25){
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
  }else{
    float factor = (temp - 25) / (float) 15;
    byte value = (factor * 255);
    pixels.setPixelColor(0, pixels.Color(value,0,255 - value));
  }
  pixels.show();
  
  //delay(100);
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
  if (type_s) {
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
