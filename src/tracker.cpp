/**
 * BLE Anenna Tracker
 * by CopterFail 2024
*/

#include <Arduino.h>
#include <Preferences.h>
#include "PWM.h"

#include "tracker.h"
#include "crsf_telemetrie.h"

#define PAN_CHANNEL   0
#define TILT_CHANNEL  1
#define OK_CHANNEL    3
#define ARM_CHANNEL   4

PWMController PWM;
pwm_channel_t chPan, chTilt;
Preferences preferences;




tracker::tracker(/* args */)
{
}

tracker::~tracker()
{
}


void  tracker::setup( void )
{
  int16_t i16North;
  int16_t i16Horizontal;

  //readback nvs:
  preferences.begin("tracker", false); 
  i16North = preferences.getShort("dir_north", 1500);
  i16Horizontal =  preferences.getShort("dir_horizontal", 1500);
  setZero( i16North, i16Horizontal, false );
  home.setLat( preferences.getInt("pos_lat", 0) );
  home.setLon( preferences.getInt("pos_lon", 0) );
  preferences.end();

  HomeIsSet = (home.getLat() != 0 ) && (home.getLon() != 0 );
  Serial.println( "home: " + String(home.getLat()) + "/" + String(home.getLon()) );

   // Init the 2 servo pwm channels
  chPan = PWM.allocate( PANPIN, 50 ); 
  PWM.setDuty( chPan, 20000 );  // 20ms duty
  chTilt = PWM.allocate( TILTPIN, 50 ); 
  PWM.setDuty( chTilt, 20000 );  // 20ms duty
  
  // Set initial servo position, or wait for more information?
  //PWM.setMicroseconds( chPan, 1500 ); 
  //PWM.setMicroseconds( chTilt, 1500 ); 
}

void  tracker::loop( crsf_telemetrie &crsf )
{
  static int16_t i16pan = 1500, i16tilt = 1500;

  if( crsf.getChannel(ARM_CHANNEL) < 1200 )
  {
    i16pan = crsf.getChannel(PAN_CHANNEL);
    i16tilt = crsf.getChannel(TILT_CHANNEL);
    setServos( i16pan, i16tilt );
    if( crsf.getChannel(OK_CHANNEL) > 1900 )
    {
        setZero( i16pan, i16tilt, true );
    }
    if( crsf.getChannel(OK_CHANNEL) < 1200 )
    {
        HomeIsSet = false;
    }
  }
  else if( updateCalculation( crsf ) )
  {
    i16pan = getPan();
    i16tilt = getTilt();
    setServos( i16pan, i16tilt );
  }

}

bool tracker::updateCalculation( crsf_telemetrie &crsf )
{
    bool result = false;
    float distance = 0.0;

    if( crsf.getLatestGps( plane ) && plane.getSatelites() >= MINSATELITES )
    {
        result = true;
        if( !HomeIsSet )
        {
            setHome( plane );
        }
        else
        {
            i16pan = (int16_t)(home.degree( plane ) * (1800.0/M_PI)); /* range is [-2700;+900] */
            i16pan += i16panzero; /* add offset, check the limits? */
            i16tilt = home.tilt( plane ) * (1800.0/M_PI); /* range is [0;900]*/
            i16tilt += i16tiltzero;
            distance = home.dist( plane );

            /* overlap will not work to code hysteresis */
            if( i16pan < -1800 ) i16pan += 3600; /* set range to [-1800..1800] degree * 0.1 */
            if( i16pan < -1800 ) i16pan += 3600;
            if( i16pan >= +1800 ) i16pan -= 3600; 

            if( i16tilt < LOWTILT )  i16tilt = LOWTILT;
            if( i16tilt > HIGHTILT )  i16tilt = HIGHTILT;
            if( i16pan < LOWPAN )  i16pan = LOWPAN;
            if( i16pan > HIGHPAN )  i16pan = HIGHPAN;

#if 0
            Serial.println( "home: " + String(home.getLat()) + "/" + String(home.getLon()) );
            Serial.println( "plane: " + String(plane.getLat()) + "/" + String(plane.getLon()) );
#endif
            Serial.println(" Dist:" + String(distance) + " Ang:" + String(i16pan) + " Tilt:" + String(i16tilt));

            i16pan = map( i16pan, LOWPAN, HIGHPAN, LOWPAN_PWM, HIGHPAN_PWM);  /* swap to change direction? */
            i16tilt = map( i16tilt, LOWTILT, HIGHTILT, LOWTILT_PWM, HIGHTILT_PWM );
    
        }
    }    
    return result;
}

int16_t tracker::getPan( void )
{
    return i16pan;
}

int16_t tracker::getTilt( void )
{
    return i16tilt;
}

void tracker::setHome( gps &h )
{
  home = h;
  HomeIsSet = (home.getLat() != 0 ) && (home.getLon() != 0 );
  if( HomeIsSet )
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
    i16panzero = map( i16pan, LOWPAN_PWM, HIGHPAN_PWM, LOWPAN, HIGHPAN );
    i16tiltzero = map( i16tilt, LOWTILT_PWM, HIGHTILT_PWM, LOWTILT, HIGHTILT );

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

void tracker::setServos( int16_t i16pan, int16_t i16tilt )
{
  PWM.setMicroseconds( chPan, i16pan );
  PWM.setMicroseconds( chTilt, i16tilt );
}