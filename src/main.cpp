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
  bool bSimulation = false;

  Serial.begin(115200);
  Serial.println("Starting BLE Client Antenna Tracker Application V0.0.1");
  if( bSimulation ){
    Serial.println("Simulation for BLE / GPS / SERO / STEPPER ");
  }
  myled.setup(); //start LED at first
  BLE_setup( bSimulation );
  mytracker.setup( bSimulation );
  myservo.setup( bSimulation );
  mystepper.setup( bSimulation );

  Serial.println("... End of setup");
}

void loop() 
{
  if( BLE_loop() )
  {
    if( mytracker.loop() )
    {
     // myservo.setServos( mytracker.getPan(), mytracker.getTilt() );
     mystepper.setStepper( mytracker.getPan() );
    }
  }

  myled.loop();
  delay(50);
}
