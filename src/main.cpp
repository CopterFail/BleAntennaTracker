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
#include "button.h"

tracker mytracker;
led myled;
servo myservo;
stepper mystepper;
extern button mybutton;



void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE Client Antenna Tracker Application V0.0.1");
  myled.setup(); //start LED at first
  BLE_setup( false );
  mytracker.setup( false );
  myservo.setup( true );
  mystepper.setup( false );
  mybutton.setup( true );

  Serial.println("... End of setup");
}

void loop() 
{
  bool bBle, bTracker;
  
  bBle = BLE_loop();
  if( bBle )
  {
      bTracker = mytracker.loop();
      if( bTracker )
      {
          myservo.setServos( mytracker.getPan(), mytracker.getTilt() );
          mystepper.setStepper( mytracker.getPan() );
      }
  }

  myled.setState( LED_BLE, bBle ? STATUS_OK : STATUS_WAIT);
  myled.setState( LED_TRACKER, bTracker ? STATUS_OK : STATUS_WAIT ); 
  myled.setState( LED_STEPPER, mystepper.getState() ? STATUS_OK : STATUS_WAIT );
  myled.setState( LED_GPS, mytracker.isPlaneSet() ? STATUS_OK : STATUS_WAIT );
  myled.setState( LED_HOME, mytracker.isHomeSet() ? STATUS_OK : STATUS_WAIT );


//myanalog.loop(); is still missing....
  mybutton.loop();
  myled.loop();
  delay(50);
}
