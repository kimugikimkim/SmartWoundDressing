  #include "Adafruit_SHT31.h"
  #include "Adafruit_TCS34725.h"
  #include "Adafruit_CCS811.h"
  #include <wire.h>
  // the i2c device are ccs811 (gas detector), TCS3472 (color sensor), SHT31 (temp and humid)
  Adafruit_SHT31 sht31 = Adafruit_SHT31();
  Adafruit_CCS811 ccs;
  Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
  // innit all instances
bool i2c_connection()
{
  bool con_loop = true;
  while (con_loop)
  {
    con_loop = !(sht31.begin()&& ccs.begin() && tcs.begin());
    if(sht31.begin()==0)
      Serial.print("SHT31 ");delay(100);
    if(ccs.begin()==0)
      Serial.print("CCS811 ");delay(100);
    if(tcs.begin()==0)
      Serial.print("CCS811 ");delay(100);
    if(con_loop!=true)
      Serial.println("not connected");delay(100);
  }
  return !con_loop;
}

void setup() {
    Serial.begin(115200);
    int itr=1;
    bool i2c_finish = i2c_connection();
    if(i2c_finish == true)
    {Serial.println("i2c connection complete");}
    else 
    {Serial.println("i2c connection failed");
    while(!i2c_finish)
    {
      itr++;
      Serial.print("i2c connect: ");
      Serial.print(itr);
      Serial.println("attemps");
      if(itr==10)
      {Serial.println("\n set up failed... exit the program");
       exit(0);
      }
      delay(100);
      }
    }
}

void loop() {
uint16_t r, g, b, c, colorTemp, lux;

  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K ,");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(",");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(",");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(",");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(",");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(",");
  Serial.println(" ");

}
