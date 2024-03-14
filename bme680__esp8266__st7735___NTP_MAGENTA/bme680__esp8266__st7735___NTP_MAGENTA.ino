/***************************************************************************
  This is a library for the BME680 gas, humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME680 Breakout
  ----> http://www.adafruit.com/products/3660

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  
          ***   Modified all by YO2LDK - ALEXANDER  ***

  - chanage before upload,  ***** with your data for WIFI, callsing and QTH  
  - made your own correction if is need it, for altitude and pressure;
    for that, find on internet the METAR station near to you and compare data 
    
    
    
    
                    ST7735               ESP8266
                    
                    CS                      D8
                    DC                      D3
                    RST                     D4
                    SDA                     D7
                    SCL                     D5
                    BLK                      +
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <TFT_eSPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>  
#include <TimeLib.h>      

#define TEMP_CORR (-3)   
#define PIN_SDA D1        
#define PIN_SCL D2
#define SEALEVELPRESSURE_HPA (1013.25)       // or change it with correction


// set WiFi network SSID and password
const char *ssid     = "xxxxx";        // YOUR SSID ROUTER NAME
const char *password = "xxxxxxxxxxx";  // YOUR ROUTER PASSWORD
unsigned long unix_epoch;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.nist.gov", 0, 60000);

Adafruit_BME680 bme; 

TFT_eSPI tft = TFT_eSPI();
//#define TFT_GREY 0x5AEB

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  //tft.init();
  tft.init(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLUE);
  //tft.fillScreen(TFT_GOLD);
  //tft.setTextWrap(false);
 
  if (!bme.begin()) {
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  WiFi.begin(ssid, password);
  Serial.print("Connecting.");
  while ( WiFi.status() != WL_CONNECTED ) {
  delay(500);
  Serial.print(".");
  timeClient.begin();  
}
}

void RTC_display()
{
  char dow_matrix[7][10] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
  byte x_pos[7] = {29, 29, 23, 11, 17, 29, 17};
  static byte previous_dow = 0;
  // print day of the week
  if( previous_dow != weekday(unix_epoch) )
  {
    previous_dow = weekday(unix_epoch);
    //tft.fillRect(5, 5, 123, 45, ST7735_NAVY);
    //tft.fillScreen(TFT_GOLD);
    tft.setTextSize(1);
    tft.setCursor(4, 4);   //24
    tft.setTextColor(ST7735_CYAN);    
    tft.print( dow_matrix[previous_dow-1] );
  
  }

    // print date
  tft.setCursor(66, 4);   //75
  tft.setTextColor(ST7735_CYAN);     
  tft.printf( " %02u - %02u - %04u", day(unix_epoch), month(unix_epoch), year(unix_epoch) );
  
  
    // print info
  tft.setCursor(3, 16); 
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);                          
  tft.print("KN05WH - ");  // YOUR QTH
  tft.setCursor(58, 16); 
  //tft.setTextColor(TFT_WHITE);
  //tft.setTextSize(1);                          
  tft.print("UTC TIME ");
  tft.setCursor(110, 16); 
  //tft.setTextColor(TFT_WHITE);
  //tft.setTextSize(1);                          
  tft.print("- YO2LDK");   // YOUR callsign / NAME

    // print clock
  tft.setCursor(10,30 );   //32
  tft.setTextColor(ST7735_YELLOW, ST7735_BLUE);    
  tft.setTextSize(2);
  tft.printf("%02u : %02u : %02u", hour(unix_epoch), minute(unix_epoch), second(unix_epoch) );
}

// main loop

void loop() {
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }  
    
  timeClient.update();
  unix_epoch = timeClient.getEpochTime();  
  

  RTC_display();
  delay(200);    // wait 200ms
 
  
  tft.setCursor(16, 55);
  tft.setTextColor(ST7735_WHITE,ST7735_BLUE);
  tft.setTextSize(1);
  tft.print("Temperature = ");
  tft.print(String(bme.temperature)+(" *C"));
  tft.setCursor(16, 70);
  tft.print("Pressure = ");
  tft.print(String(bme.pressure / 100.0+ 7.4)+ (" hPa"));  //7.4 correction factor (find METAR CLOSE TO YOU)
  tft.setCursor(16, 85);
  tft.print("Humidity = ");
  tft.print(String(bme.humidity)+ (" %"));
  tft.setCursor(16, 100);
  tft.print("Q Air = ");
  tft.print(String(bme.gas_resistance / 1000.0)+ (" KOhms"));
  tft.setCursor(16, 115);
  tft.print("Altitude = ");
  tft.print(String(bme.readAltitude(SEALEVELPRESSURE_HPA)+ 82)+(" m"));  //82 correction factor (find METAR CLOSE TO YOU)
  

  //while(1) yield();

  //  73 !!!
}
