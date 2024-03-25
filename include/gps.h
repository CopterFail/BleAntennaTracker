/**
 * GPS class
 * by CopterFail 2024
*/

#ifndef class_gps
#define class_gps

#include <cstdint>
#include <cmath>

class gps
{
private:
    /* data */
    int32_t i32altitude;
    int32_t i32lat;
    int32_t i32lon;
    int32_t i32alt;
    int8_t i8sats;
    
public:
    gps();
    gps( int32_t i32latitude, int32_t i32longitude, int8_t i8satelites=0, int32_t i32altitude=0 );
    ~gps();

    //gps diff( gps &b );
    float dist( gps &b);
    float degree( gps &b);
    float tilt( gps &b );
    void set( int32_t i32latitude, int32_t i32longitude, int8_t i8satelites=0,  int32_t i32altitude=0 );
    int8_t getSatelites( void ) {return i8sats; }
    int32_t getLat( void ) {return i32lat; }
    int32_t getLon( void ) {return i32lon; }
};

#endif
