/**
 * GPS class
 * by CopterFail 2024
*/

#ifndef classtracker
#define classtracker

#include "gps.h"

//#define SIMULATE


#define LOWTILT     (+0)    /* [degree * 0.1]*/
#define HIGHTILT    (+900)
#define LOWPAN      (-1800)
#define HIGHPAN     (+1800)

#define CENTERPAN       ((HIGHPAN-LOWPAN) / 2)
#define CENTERTILT      ((HIGHTILT-LOWTILT) / 2)

#define MINSATELITES    (4) // 4 is for indoor testing


class tracker
{
private:
    /* data */
    bool bSim = false;
    gps home;
    gps plane;
    bool update;
    bool bHomeIsSet = false;
    bool bPlaneIsSet = false;
    int16_t i16pan = CENTERPAN;
    int16_t i16tilt = CENTERTILT;
    int16_t i16panzero = 0;
    int16_t i16tiltzero = 0;


    float simAng = 0.f;
    float simDir = 1.0f;
    float simHeight = 0.0f;
    uint32_t simLast = 0;
    uint32_t simNow = 0;
    const uint32_t simInterval = 500;


public:
    tracker(/* args */){};
    ~tracker(){};

    void setup( bool bSimulation );
    bool loop( void );
    bool setHome( void );
    void setPlane( gps &p );
    void setPanZero( int16_t i16NewZero );

    bool updateCalculation( void );
    int16_t getPan( void ){ return i16pan; }
    int16_t getTilt( void ){ return i16tilt; };
    void setServos( int16_t i16pan, int16_t i16tilt );
    bool isHomeSet( void ){ return bHomeIsSet; };
    bool isPlaneSet( void ){ return bPlaneIsSet; };

};

#endif
