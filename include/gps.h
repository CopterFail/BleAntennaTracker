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
    int32_t i32lat;         /* latitude in [degree * 1e7] */
    int32_t i32lon;         /* longitute in [degree * 1e7 ] */
    int32_t i32alt;         /* altitude in [m], offset is 1000 ? */
    int8_t i8sats;          /* number of received satelites */
    /* groundSpeed [km/h *10], groundCourse [degree * 1e2]*/
public:
    gps();
    gps( int32_t i32latitude, int32_t i32longitude, int8_t i8satelites=0, int32_t i32altitude=0 );
    ~gps();

    float dist( gps &b);
    float degree( gps &b);
    float tilt( gps &b );
    void set( int32_t i32latitude, int32_t i32longitude, int8_t i8satelites=0,  int32_t i32altitude=0 );
    int8_t getSatelites( void ) {return i8sats; }
    int32_t getLat( void ) {return i32lat; }
    int32_t getLon( void ) {return i32lon; }
    void setLat( int32_t i32latitude ) { i32lat = i32latitude; }
    void setLon( int32_t i32longitude ) { i32lon = i32longitude; }
    void simulate( gps &home, float a, float ang, float h );

public:
    float deg2rad( float deg ){ return (deg * M_PI / 180.0); }
    float rad2deg( float rad ){ return (rad *180.0 / M_PI ); }
};

#endif
