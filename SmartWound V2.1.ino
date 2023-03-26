#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <SPI.h>
#include <SD.h>
#include "Adafruit_SHT31.h"
#include "Adafruit_CCS811.h"

Adafruit_CCS811 ccs;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_420MS, TCS34725_GAIN_16X); //ถ้าไม่ set gain/integration time จะใช้ค่า default ในการอ่านค่าสีคือ gain=16 integration time = 420
//set gain โดยการเปลี่ยนเลขข้างในวงเล็บ โดยมี Gain ให้เลือกคือ 1, 4, 16, 60
//set integrationtime โดยการเปลี่ยนเลขข้างในวงเล็บ โดยมีค่าให้เลือกคือ 2_4 (คือ 2.4), 24, 50, 60, 101, 120, 154, 180, 199, 240, 300, 360, 401, 420, 480, 499, 540, 600, 614
const int chipSelect = 4;
File myFile;
String name = "CHANGE_NAME_HERE.log"; // เปลี่ยนชื่อไฟล์ตรงนี้ครับพี่บิ้ง
bool enableHeater = false;
bool sensorStatus = false;
int itt;
void InitializeTCS()
{ Serial.println("COLOR sensor TCS34725 status:: ");
  sensorStatus = tcs.begin();
  if (sensorStatus) {
    Serial.println("Found sensor");
  } else {
    for (itt = 0; itt < 10; itt++) {
      tcs.begin();
      if (sensorStatus == false) {
        Serial.println("No TCS34725 found ... check your connections");
      }
      else {
        Serial.println("Found sensor");
        sensorStatus = false;
        break;
      }
    }
  }
}
void InitializeSHT()
{ Serial.println("TEMP-HUMID sensor SHT31 status:: ");
  sensorStatus = sht31.begin(0x44);
  Serial.println("COLOR sensor TCS34725 status:: ");
  if (sensorStatus) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Found sensor");
  } else {
    for (itt = 0; itt < 10; itt++) {
      sht31.begin();
      if (sensorStatus == false) {
        Serial.println("No SHT31 found ... check your connections");
      }
      else {
        Serial.println("Found sensor");
        sensorStatus = false;
        break;
      }
    }
  }
  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
}
void InitializeCCS()
{ Serial.println("GAS sensor CCS811 status:: ");
  sensorStatus = ccs.begin();
  if (sensorStatus) {
    Serial.println("Found sensor");
  }
  else {
    for (itt = 0; itt < 10; itt++) {
      ccs.begin();
      if (sensorStatus == false) {
        Serial.println("No CCS811 found ... check your connections");
      }
      else {
        Serial.println("Found sensor");
        sensorStatus = false;
        break;
      }
    }
  }
  while (!ccs.available());
}
void setup(void) {

  Serial.begin(9600);
  while (!Serial)
    delay(10);
  pinMode(LED_BUILTIN, OUTPUT); //used to display all sensor status:: TCS34725,SHT31,CCS811,sd card
  digitalWrite(LED_BUILTIN, LOW);

  InitializeTCS();
  InitializeSHT();
  InitializeCCS();
  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);
  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  myFile = SD.open(name, FILE_WRITE);
  if (myFile) {
    myFile.print("temperature"); myFile.print("\t");
    myFile.print("%humidity"); myFile.print("\t");
    myFile.print("red component"); myFile.print("\t");
    myFile.print("green component"); myFile.print("\t");
    myFile.print("blue component"); myFile.print("\t");
    myFile.print("clear component"); myFile.print("\t");
    myFile.print("colorTemp"); myFile.print("\t");
    myFile.print("lux"); myFile.print("\t");
    myFile.print("CO2"); myFile.print("\t");
    myFile.println("TVOC");
    myFile.close();
  } else {
    Serial.print("error opening file to log:: "); Serial.println(name);
  }
}

void loop(void) {
  // check i2c sensor status:: SHT31 (0x44=68,0x45=69),CCS811(0x5A=90,0x5b=91),TCS34725 (0x29=41)
  digitalWrite(LED_BUILTIN, LOW);
  Wire.beginTransmission(68);
  if (Wire.endTransmission() == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }
  Wire.beginTransmission(69);
  if (Wire.endTransmission() == 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    InitializeSHT();
  }

  Wire.beginTransmission(90);
  if (Wire.endTransmission() == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }
  Wire.beginTransmission(91);
  if (Wire.endTransmission() == 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    InitializeCCS();
  }

  Wire.beginTransmission(41);
  if (Wire.endTransmission() == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    InitializeTCS();
  }

  uint16_t r, g, b, c, colorTemp, lux, rtemp = 0, gtemp = 0, btemp = 0, ctemp = 0, colorTemp_temp = 0, luxtemp = 0;

  for (itt = 0; itt < 50; itt++)
  {
    tcs.getRawData(&r, &g, &b, &c);
    colorTemp = tcs.calculateColorTemperature(r, g, b);
    lux = tcs.calculateLux(r, g, b);
    r = r + rtemp;
    g = g + gtemp;
    b = b + btemp;
    c = c + ctemp;
    colorTemp = colorTemp + colorTemp_temp;
    lux = lux + luxtemp;
  }
  r = rtemp / 50;
  g = gtemp / 50;
  b = btemp / 50;
  c = ctemp / 50;
  colorTemp = colorTemp_temp / 50;
  lux = luxtemp / 50;
  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");


  float t, temp_t = 0;
  float h, temp_h = 0;
  for (itt = 0; itt < 50; itt++)
  {
    t = sht31.readTemperature();
    h = sht31.readHumidity();
    temp_t = t + temp_t;
    temp_h = h + temp_h;
  }
  t = temp_t / 50;
  h = temp_h / 50;
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
  uint8_t loopCnt = 0;
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
  int CO2, tvoc, temp_CO2, temp_tvoc;
  for (itt = 0; itt < 50; itt++)
  {
    if (ccs.available()) {
      if (!ccs.readData()) {

        CO2 = ccs.geteCO2();
        tvoc = ccs.getTVOC();
        temp_CO2 = temp_CO2 + CO2;
        temp_tvoc = temp_tvoc + tvoc;
      }
      else {
        Serial.println("CCS811 ERROR!");
        itt--;
      }
    }
    delay(50);
  }
  CO2 = temp_CO2 / itt;
  tvoc = temp_tvoc / itt;
  Serial.print("CO2: ");
  if (CO2 > 400) {
    Serial.print(CO2);
  }
  else {
    Serial.print("<=400"); // the threshold for CO2 detection is at 400 ppm at the minimum
  }
  Serial.print("ppm, TVOC: ");
  Serial.println(tvoc);

  // log all data in to SD card
  myFile = SD.open(name, FILE_WRITE);
  if (myFile) {
    myFile.print(t); myFile.print("\t");
    myFile.print(h); myFile.print("\t");
    myFile.print(r, DEC); myFile.print("\t");
    myFile.print(g, DEC); myFile.print("\t");
    myFile.print(b, DEC); myFile.print("\t");
    myFile.print(c, DEC); myFile.print("\t");
    myFile.print(colorTemp, DEC); myFile.print("\t");
    myFile.print(lux, DEC); myFile.print("\t");
    myFile.print(ccs.geteCO2()); myFile.print("\t");
    myFile.println(ccs.getTVOC());
    myFile.close();
  } else {
    Serial.print("error opening file to log:: "); Serial.println(name);
  }
}
