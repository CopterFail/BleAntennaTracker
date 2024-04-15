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
  BLE_setup( false );
  mytracker.setup( false );
  myservo.setup( true );
  mystepper.setup( false );

  Serial.println("... End of setup");
}

void loop() 
{
  if( BLE_loop() )
  {
      myled.setState( LED_BLE, STATUS_OK);
      if( mytracker.loop() )
      {
          myled.setState( LED_GPS, STATUS_OK);
          myled.setState( LED_HOME, STATUS_OK); //??
          myled.setState( LED_TRACKER, STATUS_OK); //??
          myservo.setServos( mytracker.getPan(), mytracker.getTilt() );
          mystepper.setStepper( mytracker.getPan() );
      }else myled.setState( LED_TRACKER, STATUS_WAIT);
  }else myled.setState( LED_BLE, STATUS_WAIT);
  myled.setState( LED_STEPPER, mystepper.getState() );

//myanalog.loop(); is still missing....

  myled.loop();
  delay(50);
}
