/**
 * BLE Anenna Tracker
 * by CopterFail 2024
*/

#include <Arduino.h>
#include "tracker.h"
#include "crsf_telemetrie.h"


tracker::tracker(/* args */)
{
}

tracker::~tracker()
{
}

void tracker::setHome( gps h )
{

}

void tracker::setPlane( gps p )
{

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
            home = plane;
            HomeIsSet = true;
        }
        else
        {
            i16pan = (int16_t)(home.degree( plane ) / M_PI * 180.0);
            i16pan += i16panzero;
            i16tilt += i16tiltzero;
            distance = home.dist( plane );
            i16tilt = 45; //hoehe????


            //Serial.println( "home: " + String(home.getLat()) + "/" + String(home.getLon()) );
            //Serial.println( "plane: " + String(plane.getLat()) + "/" + String(plane.getLon()) );
            Serial.println(" Dist:" + String(distance) + " Ang:" + String(i16pan));
        }


    if( i16pan < -20 ) i16pan += 360; /* format is 0..359 degree , -20 degree overlap */
    if( i16pan >= 380 ) i16pan -= 360; /* format is 0..359 degree , + 20 degree overlap */
    if ( i16tilt < 0 ) i16tilt +=360;
    if ( i16tilt >= 360 ) i16tilt -=360;

    if( i16tilt < LOWTILT )  i16tilt = LOWTILT;
    if( i16tilt > HIGHTILT )  i16tilt = HIGHTILT;
    if( i16pan < LOWPANLIMIT )  i16pan = LOWPANLIMIT;
    if( i16pan > HIGHPANLIMIT )  i16pan = HIGHPANLIMIT;


    i16pan = map( i16pan, LOWPAN, HIGHPAN, LOWPAN_PWM, HIGHPAN_PWM);
    i16tilt = map( i16tilt, LOWTILT, HIGHTILT, LOWTILT_PWM, HIGHTILT_PWM );
    
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

void tracker::setZero( int16_t i16pan, int16_t i16tilt )
{
    /* parameter range is 1000 .. 2000 [us] */

    i16panzero = map( i16pan, LOWPAN_PWM, HIGHPAN_PWM, LOWPAN, HIGHPAN );
    i16tiltzero = map( i16tilt, LOWTILT_PWM, HIGHTILT_PWM, LOWTILT, HIGHTILT );

    i16panzero -= CENTERPAN;
    i16tiltzero -= CENTERTILT;

    Serial.println(" Zero (p/t):" + String(i16panzero) + " Ang:" + String(i16tiltzero));
}