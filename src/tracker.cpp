



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

    if( crsf.getLatestGps( plane ) && plane.getSatelites()>4 )
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
            distance = home.dist( plane );
            //Serial.println( "home: " + String(home.getLat()) + "/" + String(home.getLon()) );
            //Serial.println( "plane: " + String(plane.getLat()) + "/" + String(plane.getLon()) );
            Serial.println(" Dist:" + String(distance) + " Ang:" + String(i16pan));
            i16tilt = 45; //hoehe????
        }

    if( i16pan < 0 ) i16pan += 360;
    if( i16tilt < LOWTILT )  i16tilt = LOWTILT;
    if( i16tilt > HIGHTILT )  i16tilt = HIGHTILT;
    if( i16pan < LOWPAN )  i16pan = LOWPAN;
    if( i16pan > HIGHPAN )  i16pan = HIGHPAN;


    i16pan = map( i16pan, 0, 360, 1000, 2000);
    i16tilt = map( i16tilt, 0, 360, 1000, 2000);
    
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
