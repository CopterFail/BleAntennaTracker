/**
 * BLE Anenna Tracker main
 * by CopterFail 2024
*/

#include <Arduino.h>

#include "BLEDevice.h"
//#include "BLEScan.h"
#include "crsf_telemetrie.h"
#include "tracker.h"

#include "BLE_client.h"



#include <NeoPixelBus.h>
const uint16_t PixelCount = 8; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t DotDataPin = 23;  
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, DotDataPin);

#define colorSaturation 64 //128
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

tracker mytracker;
crsf_telemetrie mycrsf;

void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) 
{
    /*
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    */
    //Serial.println((char*)pData);
    mycrsf.parseData(pData, length, isNotify );

}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE Client Antenna Tracker Application V0.0.1");

  BLE_setup();
  mytracker.setup();


  //strip.Begin(DotClockPin, DotDataPin, DotDataPin, DotChipSelectPin);
  strip.Begin();
  strip.ClearTo(black); 
  strip.Show();

  Serial.println("... End of setup");
}

void loop() 
{
  #ifndef SIMULATE
  if( BLE_loop() )
  #endif
  {
    mytracker.loop( mycrsf );
  }

    strip.SetPixelColor(0, red);
    strip.SetPixelColor(1, green);
    strip.SetPixelColor(2, blue);
    strip.SetPixelColor(3, white);
    strip.SetPixelColor(4, red);
    strip.SetPixelColor(5, green);
    strip.SetPixelColor(6, blue);
    strip.SetPixelColor(7, white);
    strip.Show();

  delay(50);
}
