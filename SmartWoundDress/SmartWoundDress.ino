#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <SPI.h>
#include <SD.h>
#include "Adafruit_SHT31.h"
#include "Adafruit_CCS811.h"

bool enableHeater = false;
uint8_t loopCnt = 0;

Adafruit_CCS811 ccs;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_4X);
const int chipSelect = 4;
File myFile;
void setup(void) {
  Serial.begin(9600);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");

  // Now we're ready to get readings!

  Serial.println("CCS811 test");

  if (!ccs.begin()) {
    Serial.println("Failed to start sensor! Please check your wiring.");
    while (1);
  }
  else {
    Serial.println("Initailized");
  }

  // Wait for the sensor to be ready
  while (!ccs.available());

  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);
  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  myFile = SD.open("test.log", FILE_WRITE);
 if (myFile) {
    myFile.print("temperature"); myFile.print("\t");
    myFile.print("%humidity");myFile.print("\t");
    myFile.print("red component"); myFile.print("\t");
    myFile.print("green component"); myFile.print("\t");
    myFile.print("blue component"); myFile.print("\t");
    myFile.print("clear component"); myFile.print("\t");
    myFile.print("colorTemp"); myFile.print("\t");
    myFile.print("lux");myFile.print("\t");
    myFile.print("CO2");myFile.print("\t");
    myFile.println("TVOC");
    myFile.close();
  } else {
    Serial.println("error opening test.txt");
  }
}

void loop(void) {
  uint16_t r, g, b, c, colorTemp, lux;

  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");


  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (! isnan(t)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
  } else {
    Serial.println("Failed to read temperature");
  }

  if (! isnan(h)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(h);
  } else {
    Serial.println("Failed to read humidity");
  }

  delay(100);

  // Toggle heater enabled state every 30 seconds
  // An ~3.0 degC temperature increase can be noted when heater is enabled
  if (loopCnt >= 30) {
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    Serial.print("Heater Enabled State: ");
    if (sht31.isHeaterEnabled())
      Serial.println("ENABLED");
    else
      Serial.println("DISABLED");

    loopCnt = 0;
  }
  loopCnt++;


  if (ccs.available()) {
    if (!ccs.readData()) {
      Serial.print("CO2: ");
      Serial.print(ccs.geteCO2());
      Serial.print("ppm, TVOC: ");
      Serial.println(ccs.getTVOC());
    }
    else {
      Serial.println("CCS811 ERROR!");
      while (1);
    }
  }
  delay(50);


  
  myFile = SD.open("test.log", FILE_WRITE);
  
  if (myFile) {
    myFile.print(t); myFile.print("\t");
    myFile.print(h);myFile.print("\t");
    myFile.print(r, DEC); myFile.print("\t");
    myFile.print(g, DEC); myFile.print("\t");
    myFile.print(b, DEC); myFile.print("\t");
    myFile.print(c, DEC); myFile.print("\t");
    myFile.print(colorTemp, DEC); myFile.print("\t");
    myFile.print(lux, DEC);myFile.print("\t");
    myFile.print(ccs.geteCO2());myFile.print("\t");
    myFile.println(ccs.getTVOC());
    myFile.close();
  } else {
    Serial.println("error opening test.txt");
  }
}
