#ifndef classtracker
#define classtracker

#include "gps.h"
#include "crsf_telemetrie.h"


#define LOWTILT     0
#define HIGHTILT    90
#define LOWPAN      0
#define HIGHPAN     360

#define LOWTILT_PWM     1000
#define HIGHTILT_PWM    2000
#define LOWPAN_PWM      (1500-338)
#define HIGHPAN_PWM     (1500+338)

#define LOWPANLIMIT     (-20)
#define HIGHPANLIMIT    (+380)


#define CENTERPAN       ((HIGHPAN-LOWPAN) / 2)
#define CENTERTILT      ((HIGHTILT-LOWTILT) / 2)

#define MINSATELITES    (4) // 4 is for indoor testing

#define PANPIN  2
#define TILTPIN 4


class tracker
{
private:
    /* data */
    gps home;
    gps plane;
    bool update;
    bool HomeIsSet = false;
    int16_t i16pan = CENTERPAN;
    int16_t i16tilt = CENTERTILT;
    int16_t i16panzero = 0;
    int16_t i16tiltzero = 0;
    

public:
    tracker(/* args */);
    ~tracker();

    void setup( void );
    void loop( crsf_telemetrie &crsf );
    void setHome( gps h );
    void setPlane( gps p );
    bool updateCalculation( crsf_telemetrie &crsf );
    int16_t getPan( void );
    int16_t getTilt( void );
    void setZero( int16_t i16pan, int16_t i16tilt );
    void setServos( int16_t i16pan, int16_t i16tilt );
};

#endif
