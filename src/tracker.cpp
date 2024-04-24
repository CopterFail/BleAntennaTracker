/**
 * BLE Anenna Tracker
 * by CopterFail 2024
*/

#include <Arduino.h>
#include <Preferences.h>

#include "tracker.h"
#include "crsf_telemetrie.h"
#include "led.h"

#define PAN_CHANNEL   0 //crsf channels
#define TILT_CHANNEL  1
#define OK_CHANNEL    3
#define ARM_CHANNEL   4

Preferences preferences;
crsf_telemetrie crsf; 
extern led myled;

void  tracker::setup( bool bSimulation )
{
  bSim = bSimulation;
  if( bSim ){
    Serial.println("Simulation for tracker is active ");
    }
  
  //readback nvs:
  preferences.begin("tracker", false); 
  home.setLat( preferences.getInt("pos_lat", 0) );
  home.setLon( preferences.getInt("pos_lon", 0) );
  preferences.end();

  bPlaneIsSet = false;
  bHomeIsSet = (home.getLat() != 0 ) && (home.getLon() != 0 );
  if( bHomeIsSet )
    Serial.println( "Home: " + String(home.getLat()) + "/" + String(home.getLon()) );
}

bool tracker::loop( void )
{
  bool result = false;
  result = updateCalculation();
  return result;
}

bool tracker::updateCalculation( void )
{
    bool result = false;
    float distance = 0.0;

    if( bSim || (crsf.getLatestGps( plane ) && plane.getSatelites() >= MINSATELITES ))
    {
        result = true;
        if( !bHomeIsSet )
        {
            if( bSim )
            {
              home.set(510000000, 67000000, 5, 0 );
              bHomeIsSet = true;
              simAng = 0.0f;
              simHeight = 0.0f;
              simDir = 1.0f;
            }
            else
            {
              setHome();
            }
        }
        else
        {
            bPlaneIsSet = true;
            if( bSim )
            {

              simAng += simDir;
              simHeight += 10.0f;
              if( simAng > 100 ){ simDir = -1; simHeight = 0.0f; }
              else if( simAng < -100 ){ simDir = +1; }

              simNow = millis();
              if( (simNow - simLast) > simInterval ){
                plane.simulate( home, 500, simAng, simHeight );
                simLast = simNow;
              }else{
                return false;
              }
          
            }

            i16pan = (int16_t)(10.0 * home.degree( plane )); /* range is [-1800;+1800] , direction fixed */
            i16pan += i16panzero;
            if( i16pan < -1800 ) i16pan += 3600; /* set range to [-1800..1800] */
            if( i16pan > +1800 ) i16pan -= 3600; 

            i16tilt = home.tilt( plane ) * (10); /* range is [0;900]*/
            i16tilt += i16tiltzero;

            distance = home.dist( plane );
            
            // limit angles to the hardware range
            if( i16tilt < LOWTILT )  i16tilt = LOWTILT;
            if( i16tilt > HIGHTILT )  i16tilt = HIGHTILT;
            if( i16pan < LOWPAN )  i16pan = LOWPAN;
            if( i16pan > HIGHPAN )  i16pan = HIGHPAN;

#if 0
            Serial.println( "home: " + String(home.getLat()) + "/" + String(home.getLon()) );
            Serial.println( "plane: " + String(plane.getLat()) + "/" + String(plane.getLon()) );
#endif
            
            Serial.println(" Dist:" + String(distance) + " Ang:" + String(i16pan) + " Tilt:" + String(i16tilt));
        }
    }   
    return result;
}

bool tracker::setHome( void )
{
  bool result = false;
  if( (plane.getSatelites() >= MINSATELITES) && (plane.getLat() != 0 ) && ( plane.getLon() != 0) )
  {
    home = plane;
    bHomeIsSet = true;
    result = true;
    if( bHomeIsSet )
    {
      preferences.begin("tracker", false); 
      preferences.putInt("pos_lat", home.getLat());
      preferences.putInt("pos_lon", home.getLon());
      preferences.end();
      Serial.println( "Home: " + String(home.getLat()) + "/" + String(home.getLon()) );
    }
  }
  else{
    result = false;
  }
  return result;
}

int16_t tracker::readNorth( void )
{
    static int valadcpan = 1700;
    float valadcbat = 0.0;
    valadcpan = ( 2 * valadcpan + analogRead(POTIPIN)) / 3; // Poti value in in range of, 0..3510 (4092 is not reached)
    i16panzero = map( valadcpan, 0, 3840, LOWPAN, HIGHPAN ); 
    //i16panzero = 0; // big noise, try median filter?

    //Serial.println( "North offset: " + String(i16panzero) + " / " + String(valadcpan) ); // Wert ausgeben
    return i16panzero;
}

void tracker::readBattery( void )
{
    float valadcbat = 0.0;

    valadcbat = AKKUFACTOR * 4.0 / 4096 * analogRead(AKKUPIN); //why ?
    AkkuVoltage = ( 10.0 * AkkuVoltage + valadcbat ) / 11.0;

    if( AkkuVoltage > 10.0 ) myled.setState( LED_POWER, STATUS_OK );
    else  myled.setState( LED_POWER, STATUS_FAIL );

    //Serial.println( "Battery: " + String(AkkuVoltage) );
}


