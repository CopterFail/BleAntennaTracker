/**
 * BLE Anenna Tracker main
 * by CopterFail 2024
*/

#include <Arduino.h>

#include "BLEDevice.h"
#include "BLE_client.h"
#include "tracker.h"
#include "led.h"
#include "servo.h"
#include "stepper.h"

tracker mytracker;
led myled;
servo myservo;
stepper mystepper;



void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE Client Antenna Tracker Application V0.0.1");
  myled.setup(); //start LED at first
  BLE_setup( true );
  mytracker.setup( true );
  myservo.setup( true );
  mystepper.setup( false );

  Serial.println("... End of setup");
}

void loop() 
{
  if( BLE_loop() )
  {
    if( mytracker.loop() )
    {
     myservo.setServos( mytracker.getPan(), mytracker.getTilt() );
     mystepper.setStepper( mytracker.getPan() );
    }
  }

  myled.loop();
  delay(50);
}
