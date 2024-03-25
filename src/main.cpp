/**
 * BLE Anenna Tracker main
 * by CopterFail 2024
*/

#include <Arduino.h>
#include "PWM.h"


#include "BLEDevice.h"
//#include "BLEScan.h"
#include "crsf_telemetrie.h"
#include "tracker.h"

#include "BLE_client.h"



#define PANPIN  2
#define TILTPIN 4

PWMController PWM;
tracker mytracker;
crsf_telemetrie mycrsf;

void SetServos( int ipan, int itilt);
void tracker_setup( void );
void tracker_loop( void );

int16_t i16North;
int16_t i16Horizontal;

pwm_channel_t chPan, chTilt;


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
  PWM.setMicroseconds( chPan, ipan );
  PWM.setMicroseconds( chTilt, itilt );
}


void tracker_setup( void ) 
{

  //ToDo: readback nvs:
  i16North = 0;
  i16Horizontal = 0;

   // Init the 2 servo pwm channels
  chPan = PWM.allocate( PANPIN, 50 ); 
  PWM.setDuty( chPan, 20000 );  // 20ms duty
  chTilt = PWM.allocate( TILTPIN, 50 ); 
  PWM.setDuty( chTilt, 20000 );  // 20ms duty
  
  //PWM.setMicroseconds( chPan, 1500 ); // intial value?
  //PWM.setMicroseconds( chTilt, 1500 ); // intial value?
}

void tracker_loop( void ) 
{
  static int16_t i16pan = 1500, i16tilt = 1500;

  if( mycrsf.getChannel(4) < 1200 )
  {
    i16pan = mycrsf.getChannel(0);
    i16tilt = mycrsf.getChannel(1);
    if( mycrsf.getChannel(3) > 1900 )
    {
        i16North = i16pan;
        i16Horizontal = i16tilt;
        mytracker.setZero( i16pan, i16tilt );
        //ToDo: save non volatile
        delay(500);
    }
  }
  else if( mytracker.updateCalculation( mycrsf ) )
  {
    i16pan = mytracker.getPan();
    i16tilt = mytracker.getTilt();
  }
  SetServos( i16pan, i16tilt );
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE Client Antenna Tracker Application...");

  BLE_setup();
  tracker_setup();
}

void loop() 
{
  BLE_loop();
  tracker_loop();
  delay(50);
}
