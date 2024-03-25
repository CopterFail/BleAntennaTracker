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

void SetServos( int ipan, int itilt) {
}


void tracker_setup( void ) 
{

}

void tracker_loop( void ) 
{
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE Client Antenna Tracker Application...");

  BLE_setup();
  mytracker.setup();
}

void loop() 
{
  BLE_loop();
  mytracker.loop( mycrsf );
  delay(50);
}
