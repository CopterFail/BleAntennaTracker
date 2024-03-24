#ifndef classtracker
#define classtracker

#include "gps.h"
#include "crsf_telemetrie.h"


#define LOWTILT 0
#define HIGHTILT 90
#define LOWPAN  0
#define HIGHPAN 360

class tracker
{
private:
    /* data */
    gps home;
    gps plane;
    bool update;
    bool HomeIsSet = false;
    int16_t i16pan;
    int16_t i16tilt;

public:
    tracker(/* args */);
    ~tracker();

    void setHome( gps h );
    void setPlane( gps p );
    bool updateCalculation( crsf_telemetrie &crsf );
    int16_t getPan( void );
    int16_t getTilt( void );
};

#endif
