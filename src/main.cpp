/**
 * BLE Anenna Tracker main
 * by CopterFail 2024
*/

#include <Arduino.h>

#define _USE_BLE_ 0 //Compile switch, 1 to use BLE or 0 use ELRS instead

//#include "BLEDevice.h"
//#include "BLE_client.h"
#include "tracker.h"
#include "led.h"
#include "servo.h"
#include "stepper.h"
#include "button.h"
#include "analog.h"
#include "crsf_telemetrie.h"
#include "hardware.h"

tracker mytracker;
led myled;
servo myservo;
stepper mystepper;
analog myanalog;
Crsf mycrsf; 
extern button mybutton;




void setup() 
{
  Serial.begin(115200);
  Serial1.begin(CRSF_BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);
  myled.setup(); //start LED at first
#if _USE_BLE_
  BLE_setup( false );
#endif
  mycrsf.setup( true, &Serial1 );
  mytracker.setup( false );
  myservo.setup( false );
  mystepper.setup( false );
  mybutton.setup( false );
  myanalog.setup( false );

  Serial.println("Starting BLE Client Antenna Tracker Application V0.0.2");
  Serial.println("... End of setup");
}


void loop() 
{
  bool bConnected = false;
  bool bTracker = false;
  bool bAngleUpdate = false;
  
#if _USE_BLE_ 
  /*
  Operation:
  1. Connect BLE to transmitter with pull 2305 modified SW , BLE need some delay()
  2. Get GPS by the received telemetrie (BLE, tracker, gps)
  3. Calculate pan / tilt angle (mytracker)
  4. Correct / filter angles
  5. Set outputs ( led, stepper, servo )
  */
  delay(50);
  bConnected = BLE_loop();
  if( bConnected )
  {
      bTracker = mytracker.loop();
      if( bTracker )
      {
          myservo.setServos( +1 * mytracker.getPan(), mytracker.getTilt() );
          mystepper.setStepper( -1 * mytracker.getPan() );
      }
  }
#else
  /*
  Operation:
  1. Connect ELRS 2nd receiver, telemetrie is disabled, regular SW is used
  2. Get the channels 15 / 16 for pan / tilt angles (mycrsf)
  3. The received information is already calculated by the etx transmitter with lua scripts
  4. Correct / filter angles
  5. Set outputs ( led, stepper, servo )
  */
  delay(5);
  bConnected = mycrsf.loop();
  if( bConnected )
  {
    static int16_t i16Pan=0, i16Tilt=450;
    bAngleUpdate = mycrsf.getAngles( i16Pan, i16Tilt );
    if( bAngleUpdate )
    {
      // correct / filter angles
      // to do?
      
      Serial.println( " Pan / Tilt: " + String(i16Pan) + " / " + String(i16Tilt));

      // set outputs
      myservo.setServos( +1 * i16Pan, i16Tilt );
      mystepper.setStepper( -1 * i16Pan );
    }

  }
#endif


  myled.setState( LED_BLE, bConnected ? STATUS_OK : STATUS_WAIT);
  myled.setState( LED_TRACKER, bTracker ? STATUS_OK : STATUS_WAIT ); 
  myled.setState( LED_STEPPER, mystepper.getState() ? STATUS_OK : STATUS_WAIT );
  myled.setState( LED_GPS, mytracker.isPlaneSet() ? STATUS_OK : STATUS_WAIT );
  myled.setState( LED_HOME, mytracker.isHomeSet() ? STATUS_OK : STATUS_WAIT );

  mytracker.setPanZero( myanalog.readNorth() );
  myanalog.readBattery();

  if( mybutton.bPressedBlue ) // blue button (right) tries to set home, if gps is valid
  {
    myled.flash( mytracker.setHome() ); 
  } 

  if( mybutton.bPressedRed ) 
  {
    myled.flash( true );
    for( int i=0; i<500; i++ )
    {
      mystepper.setStepper( myanalog.readNorth() );  // point north
      //mystepper.setStepper( 900 /*mytracker.readNorth()*/ );  // point north
      // adjust with poti
      myled.loop();
      mystepper.loop();
      delay( 20 );
    }
  }

  mystepper.loop();
  mybutton.loop();
  myled.loop();
  myanalog.loop();
}
