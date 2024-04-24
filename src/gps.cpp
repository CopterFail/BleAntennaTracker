/**
 * GPS class
 * by CopterFail 2024
*/

#include <cstdint>
#include <cmath>

#include "gps.h"

#define EARTH_RADIUS_KM 6371.0
#define EARTH_RADIUS_M 6371000.0

gps::gps()
{
    gps( 510000000, 67000000, 5 );
}

gps::gps( int32_t i32latitude, int32_t i32longitude, int8_t i8satelites, int32_t i32altitude )
{
    set( i32latitude, i32longitude, i8satelites, i32altitude);
}

gps::~gps()
{
}

void gps::set( int32_t i32latitude, int32_t i32longitude, int8_t i8satelites, int32_t i32altitude )
{
    i32lat = i32latitude;
    i32lon = i32longitude;
    i8sats = i8satelites;
    i32alt = i32altitude;
}

void gps::simulate( gps &home, float a, float ang, float h )
{
    i32lon = home.getLon() - (int32_t)( a * 1e7 / 71500.0 * sinf(ang/180*M_PI) ); //dx
    i32lat = home.getLat() + (int32_t)( a * 1e7 / 111300.0 * cosf(ang/180*M_PI) ); //dy
    i32alt = (int32_t)h;
}

#if 0
gps gps::diff( gps &b )
{
    //ToDo ? dx,dy , correction
    return;
}
#endif

float gps::dist( gps &b)
{
    float lat1 = deg2rad((float)i32lat/1.0e7);
    float lon1 = deg2rad((float)i32lon/1.0e7);
    float lat2 = deg2rad((float)b.i32lat/1.0e7);
    float lon2 = deg2rad((float)b.i32lon/1.0e7);

    // Berechnen der Differenzen
    float dLat = lat2 - lat1;
    float dLon = lon2 - lon1;

    // Anwenden der Haversine-Formel
    float a = sinf(dLat / 2) * sinf(dLat / 2) +
               cosf(lat1) * cosf(lat2) *
               sinf(dLon / 2) * sinf(dLon / 2);
    float c = 2 * atan2f(sqrtf(a), sqrtf(1 - a));
    float distance = EARTH_RADIUS_M * c;

    return distance;
}

float gps::degree( gps &b)
{
    float lat1 = deg2rad((float)i32lat/1.0e7);
    float lon1 = deg2rad((float)i32lon/1.0e7);
    float lat2 = deg2rad((float)b.i32lat/1.0e7);
    float lon2 = deg2rad((float)b.i32lon/1.0e7);

    // Berechnen der Differenzen
    float dLon = lon2 - lon1;

   // Berechnung der Peilung
    float y = sinf(dLon) * cosf(lat2);
    float x = cosf(lat1) * sinf(lat2) - sinf(lat1) * cosf(lat2) * cosf(dLon);
    float bearing_rad = atan2f(y, x);

    // Umwandlung von Radiant in Grad
    fBearingDeg = rad2deg(bearing_rad);

    float fDiffDeg = fBearingDeg - fLastBearingDeg;
    if(fDiffDeg > +2.0 ) fDiffDeg = +2.0;
    if(fDiffDeg < -2.0 ) fDiffDeg = -2.0;
    fLastBearingDeg = fBearingDeg;
    //fBearingDeg += fDiffDeg; test this laster....

    return fBearingDeg;
}

float gps::tilt( gps &b )
{
    float dx,dy,grad;

    dx = float( b.i32alt - i32alt );
    dy = dist( b );
    if( fabsf(dy) < 0.01 ) dy = 0.01;
    grad = atan2f(dx,dy); /* results is [-pi:+pi] */
    return rad2deg( grad );
}

