#include "Adafruit_SHT31.h"
#include "Adafruit_TCS34725.h"
#include "Adafruit_CCS811.h"
#include "tcs34725autorange.h"
#include <wire.h>

// the i2c device are ccs811 (gas detector), TCS34725 (color sensor), SHT31 (temp and humid)
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_CCS811 ccs;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
tcs34725Autorange TCSautoRange;
// innit all instances
bool i2c_connection()
{
  bool con_loop = true;
  while (con_loop)
  {
    con_loop = !(sht31.begin() && ccs.begin() && tcs.begin());
    if (sht31.begin() == 0)
      Serial.print("SHT31 "); delay(100);
    if (ccs.begin() == 0)
      Serial.print("CCS811 "); delay(100);
    if (tcs.begin() == 0)
      Serial.print("TCS34725 "); delay(100);
    if (con_loop = true)
      Serial.println("\tnot connected"); delay(100);
  }
  return !con_loop;
}

void setup() {
  Serial.begin(115200);
  int itr = 1;
  bool i2c_finish = i2c_connection();
  if (i2c_finish == true)
  {
    Serial.println("i2c connection complete");
  }
  else
  { Serial.println("i2c connection failed");
    while (!i2c_finish)
    {
      itr++;
      Serial.print("i2c connect: ");
      Serial.print(itr);
      Serial.println("attemps");
      if (itr == 10)
      {
        Serial.println("\n set up failed... exit the program");
        exit(0);
      }
      delay(100);
    }
  }
}

void TCSMeasure() {

   if (TCSautoRange.begin() == 1)
  { 
      DebugGetColorData();
      //getColorData();
  }
  

 
    /*debug strings
      Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K");
      Serial.print("Lux: "); Serial.print(lux, DEC);
      Serial.print("R: "); Serial.print(r, DEC);
      Serial.print("G: "); Serial.print(g, DEC);
      Serial.print("B: "); Serial.print(b, DEC);
      Serial.print("C: "); Serial.print(c, DEC);
      Serial.println(" ");
    */
  
}

void SHTMeasure() {
  // we need to measure 100 values in 10 sec then average all the value into 1
  float Temper = 0, tempTemper = 0;
  float Humid = 0, tempHumid = 0;
  int ite = 100, i = 0;
  char buffer[40];
  /*debug strings
     Serial.print("Temp *C = "); Serial.println(Temp);
     Serial.print("Hum. % = "); Serial.println(Humid);
  */
  for (i = 0; i < 100; i++)
  {
    // handle null value
    if (!isnan(Temper) && !isnan(Humid))
    {
      /*debug strings
         Serial.print("Temp *C = "); Serial.println(Temp);
         Serial.print("Hum. % = "); Serial.println(Humid);
      */
      Temper = sht31.readTemperature();
      Humid = sht31.readHumidity();
    }
    else {
      ite--;
      sprintf(buffer, "WARNING::SHT Droped %d data", 100 - ite);
    }
    if (ite <= 0) {
      break;
    }
    tempTemper = tempTemper + Temper;
    tempHumid = tempHumid + Humid;
    delay(100);
  }
  //debug string
  Serial.print("AVG Temp *C = "); Serial.println(tempTemper / ite);
  Serial.print("AVG Hum. % = "); Serial.println(tempHumid / ite);
}

void CCSMeasure() {
  float CO2 = 0, TVOC = 0;
  float tempCO2 = 0, tempTVOC = 0;
  int ite = 100, i = 0;
  char buffer[100];
  for (i = 0; i < 100; i++)
  {
    if (ccs.available()) {
      if (!ccs.readData()) {
        Serial.print("CO2: ");
        Serial.print("ppm, TVOC: ");
        /* debug string
          Serial.print(ccs.geteCO2());
          Serial.println(ccs.getTVOC());
        */
      }
      else {
        ite--;
        sprintf(buffer, "WARNING::CCS Droped %d data", 100 - ite);
      }
      if (ite <= 0) {
        break;
      }
      tempCO2 = tempCO2 + CO2;
      tempTVOC = tempTVOC + TVOC;
      delay(100);
    }
  }
  //debug string
  Serial.print("AVG CO2 = "); Serial.print(tempCO2 / ite);Serial.print("PPM");
  Serial.print("AVG TVOC = "); Serial.print(tempTVOC / ite);Serial.print("PPM");
}
