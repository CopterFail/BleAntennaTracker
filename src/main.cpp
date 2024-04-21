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
  BLE_setup( true );
  mytracker.setup( true );
  myservo.setup( false );
  mystepper.setup( true );
  mybutton.setup( false );

  Serial.println("... End of setup");
}

void loop() 
{
  bool bBle = false;
  bool bTracker = false;
  
  bBle = BLE_loop();
  if( bBle )
  {
      bTracker = mytracker.loop();
      if( bTracker )
      {
          myservo.setServos( +1 * mytracker.getPan(), mytracker.getTilt() );  /* todo: check pan direction */
          mystepper.setStepper( -1 * mytracker.getPan() );
      }
  }

  myled.setState( LED_BLE, bBle ? STATUS_OK : STATUS_WAIT);
  myled.setState( LED_TRACKER, bTracker ? STATUS_OK : STATUS_WAIT ); 
  myled.setState( LED_STEPPER, mystepper.getState() ? STATUS_OK : STATUS_WAIT );
  myled.setState( LED_GPS, mytracker.isPlaneSet() ? STATUS_OK : STATUS_WAIT );
  myled.setState( LED_HOME, mytracker.isHomeSet() ? STATUS_OK : STATUS_WAIT );

  if( mybutton.bPressedBlue ) // blue button (right) tries to set home, if gps is valid
  {
    myled.flash( mytracker.setHome() ); 
  } 

  if( mybutton.bPressedRed ) 
  {
    myled.flash( true );
    for( int i=0; i<500; i++ )
    {
      mystepper.setStepper( mytracker.readNorth() );  // point north
      //mystepper.setStepper( 900 /*mytracker.readNorth()*/ );  // point north
      // adjust with poti
      myled.loop();
      delay( 20 );
    }
  }

// todo: myanalog.loop(); is still missing.... see north and akku
  mybutton.loop();
  myled.loop();
  delay(50);
}
