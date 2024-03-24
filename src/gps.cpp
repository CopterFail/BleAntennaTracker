/**
 * GPS class
 * by CopterFail 2024
*/

#include <cstdint>
#include <cmath>

#include "gps.h"

gps::gps()
{
    gps( 513676473, 67874330, 5 );
}

gps::gps( int32_t i32latitude, int32_t i32longitude, int8_t i8satelites )
{
    set( i32latitude, i32longitude, i8satelites);
}

gps::~gps()
{
}

void gps::set( int32_t i32latitude, int32_t i32longitude, int8_t i8satelites )
{
    i32lat = i32latitude;
    i32lon = i32longitude;
    i8sats = i8satelites;
}

gps diff( gps &b )
{
    //ToDo ?
}

float gps::dist( gps &b)
{
    float dx,dy,distance;

    // see: https://www.kompf.de/gps/distcalc.html

    dx = (71500.0 / 1.0e7) * float(i32lon - b.i32lon);
    dy = (111300.0 / 1.0e7) * float(i32lat - b.i32lat);
    distance = sqrt(dx * dx + dy * dy);
    return distance;
}

float gps::degree( gps &b)
{
    float dx,dy,grad;

    dx = (71500.0 / 1.0e7) * float(i32lon - b.i32lon);
    dy = (111300.0 / 1.0e7) * float(i32lat - b.i32lat);
    grad = atan2f(dy,dx);
    return grad;
}

