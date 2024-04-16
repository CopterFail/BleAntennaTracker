/**
 * BLE Anenna Tracker
 * by CopterFail 2024
*/

#include <Arduino.h>
#include <Preferences.h>

#include "tracker.h"
#include "crsf_telemetrie.h"
#include "led.h"

#define PAN_CHANNEL   0
#define TILT_CHANNEL  1
#define OK_CHANNEL    3
#define ARM_CHANNEL   4

Preferences preferences;
crsf_telemetrie crsf; 
extern led myled;

void  tracker::setup( bool bSimulation )
{
  int16_t i16North;
  int16_t i16Horizontal;

  bSim = bSimulation;
  if( bSim ){
    Serial.println("Simulation for tracker is active ");
    }
  
  //readback nvs:
  preferences.begin("tracker", false); 
  i16North = preferences.getShort("dir_north", 1500);
  i16Horizontal =  preferences.getShort("dir_horizontal", 1500);
  setZero( i16North, i16Horizontal, false );
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

  (void)readNorth();
  result = updateCalculation();

  return result;



#if 0
  if( crsf.getChannel(ARM_CHANNEL) < 1200 )
  {
    // overwrite the pwm values:
    //i16panpwm = crsf.getChannel(PAN_CHANNEL);
    //i16panpwm = getPanPwm( i16panzero );
    //i16panpwm = crsf.getChannel(PAN_CHANNEL);
//    i16tiltpwm = crsf.getChannel(TILT_CHANNEL);
//    setServos( i16panpwm, i16tiltpwm );
//  setStepper( i16panzero );

    if( crsf.getChannel(OK_CHANNEL) > 1900 )
    {
        //setZero( i16panpwm, i16tiltpwm, true );
    }
    if( crsf.getChannel(OK_CHANNEL) < 1200 )
    {
        //HomeIsSet = false;
    }
  }
  else 
#endif

  
}

bool tracker::updateCalculation( void )
{
    bool result = false;
    float distance = 0.0;
    static int16_t i16GpsPacketCount = 11;

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
              setHome( plane );
            }
        }
        else
        {
            bPlaneIsSet = true;
            if( bSim )
            {
              plane.simulate( home, 500, simAng, simHeight );
              simAng += simDir;
              simHeight += 1.0f;
              if( simAng > 100 ){ simDir = -1; }
              else if( simAng < -100 ){ simDir = +1; }
            }

            i16pan = (int16_t)(home.degree( plane ) * (1800.0/M_PI)); /* range is [-1800;+1800] */
            i16pan += i16panzero; /* add offset, i16panzero is in range [-1800;1800] -> [-3600;+3600]*/
            if( i16pan < -1800 ) i16pan += 3600; /* set range to [-1800..1800] */
            if( i16pan > +1800 ) i16pan -= 3600; 

            i16tilt = home.tilt( plane ) * (1800.0/M_PI); /* range is [0;900]*/
            i16tilt += i16tiltzero;
            
            // limit angles to the hardware range
            if( i16tilt < LOWTILT )  i16tilt = LOWTILT;
            if( i16tilt > HIGHTILT )  i16tilt = HIGHTILT;
            if( i16pan < LOWPAN )  i16pan = LOWPAN;
            if( i16pan > HIGHPAN )  i16pan = HIGHPAN;

#if 0
            Serial.println( "home: " + String(home.getLat()) + "/" + String(home.getLon()) );
            Serial.println( "plane: " + String(plane.getLat()) + "/" + String(plane.getLon()) );
#endif
            distance = home.dist( plane );
            Serial.println(" Dist:" + String(distance) + " Ang:" + String(i16pan) + " Tilt:" + String(i16tilt));
        }
    }   
    return result;
}

void tracker::setHome( gps &h )
{
  home = h;
  bHomeIsSet = (home.getLat() != 0 ) && (home.getLon() != 0 );  //sat count?
  if( bHomeIsSet )
  {
    preferences.begin("tracker", false); 
    preferences.putInt("pos_lat", home.getLat());
    preferences.putInt("pos_lon", home.getLon());
    preferences.end();
    Serial.println( "Home: " + String(home.getLat()) + "/" + String(home.getLon()) );
  }
}

void tracker::setPlane( gps &p )
{

}

void tracker::setZero( int16_t i16pan, int16_t i16tilt, bool bStore )
{
    /* parameter range is 1000 .. 2000 [us] */
    //i16panzero = map( i16pan, LOWPAN_PWM, HIGHPAN_PWM, LOWPAN, HIGHPAN );
    //i16tiltzero = map( i16tilt, LOWTILT_PWM, HIGHTILT_PWM, LOWTILT, HIGHTILT );

    i16panzero -= CENTERPAN;
    i16tiltzero -= CENTERTILT;

    //ToDo: save non volatile
    if( bStore )
    {
      preferences.begin("tracker", false); 
      preferences.putShort("dir_north", i16pan);
      preferences.putShort("dir_horizontal", i16tilt);
      preferences.end();
    }

    Serial.println("Zero (p/t):" + String(i16panzero) + " Ang:" + String(i16tiltzero));
}

int16_t tracker::readNorth( void )
{
      //I cannot use these functions:
    //analogSetCycles(255);
    //adcStart(POTIPIN);
    //adcBusy(POTIPIN);
    //resultadcEnd(POTIPIN);
    static int valadcpan = 1700;
    float valadcbat = 0.0;
    valadcpan = ( 2 * valadcpan + analogRead(POTIPIN)) / 3; // Poti value in in range of, 0..3510 (4092 is not reached)
    i16panzero = map( valadcpan, 0, 3510, LOWPAN, HIGHPAN ); // das muss an eine bedingung gebunden werden
//i16panzero = 0; // big noise, try median filter?
    valadcbat = AKKUFACTOR * 4.0 / 4096 * analogRead(AKKUPIN); //why ?
    AkkuVoltage = ( 10.0 * AkkuVoltage + valadcbat ) / 11.0;
     
    //Serial.println( String(i16panzero) + " / " + String(AkkuVoltage) ); // Wert ausgeben

    if( AkkuVoltage > 10.0 ) myled.setState( LED_POWER, STATUS_OK );
    else  myled.setState( LED_POWER, STATUS_FAIL );
    //if( i16panzero != 0 ) myled.setState( LED_??, STATUS_OK );
    //else  myled.setState( LED_POWER, STATUS_FAIL );
    
    return i16panzero;
}


