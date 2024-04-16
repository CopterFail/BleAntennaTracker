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

#define POTIPIN 34
#define AKKUPIN 35

#define AKKUFACTOR (10.7f)



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

    float AkkuVoltage =0.0f;
    float simAng = 0.f;
    float simDir = 1.0f;
    float simHeight = 0.0f;

public:
    tracker(/* args */){};
    ~tracker(){};

    void setup( bool bSimulation );
    bool loop( void );
    void setHome( gps &h );
    void setPlane( gps &p );
    bool updateCalculation( void );
    int16_t getPan( void ){ return i16pan; }
    int16_t getTilt( void ){ return i16tilt; };
    void setZero( int16_t i16pan, int16_t i16tilt, bool bStore );
    void setServos( int16_t i16pan, int16_t i16tilt );
    int16_t readNorth( void );
    bool isHomeSet( void ){ return bHomeIsSet; };
    bool isPlaneSet( void ){ return bPlaneIsSet; };
};

#endif
