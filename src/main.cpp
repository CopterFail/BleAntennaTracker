

#include <Arduino.h>
#include <ESP32Servo.h>


#include "BLEDevice.h"
//#include "BLEScan.h"
#include "crsf_telemetrie.h"
#include "tracker.h"

#include "BLE_client.h"

#define PANPIN  2
#define TILTPIN 4

Servo panservo;
Servo tiltservo;
tracker mytracker;
crsf_telemetrie mycrsf;

void SetServos( int ipan, int itilt);
void tracker_setup( void );
void tracker_loop( void );

int16_t i16North;
int16_t i16Horizontal;

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
	panservo.writeMicroseconds(ipan); // resolution is 20us
	tiltservo.writeMicroseconds(itilt);
#if 0
    // this shows the resolution is 20us :(
    String newValue = String(ipan) + "/" + String(panservo.readMicroseconds());
    Serial.println("PWM: " + newValue);
#endif
}

void tracker_setup( void ) 
{

  //ToDo: readback nvs:
  i16North = 0;
  i16Horizontal = 0;

   // Init the 2 servos
  ESP32PWM::allocateTimer(3);
  //ESP32PWM::setTimerWidth(20); // ab welcher version der lib?
  //readTimerWidth();
  panservo.setPeriodHertz(50); // standard 50 hz servo
  panservo.attach(PANPIN, 1000, 2000); // Attach the servo after it has been detatched
  tiltservo.setPeriodHertz(50); // standard 50 hz servo
  tiltservo.attach(TILTPIN, 1000, 2000); // Attach the servo after it has been detatched
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
        i16North = i16pan - 1500;
        i16Horizontal = i16tilt - 1500;
        Serial.println("Zeropoint: " + String(i16North) + "/" + String(i16Horizontal));
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
