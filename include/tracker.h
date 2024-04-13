#ifndef classtracker
#define classtracker

#include "gps.h"
#include "crsf_telemetrie.h"

//#define SIMULATE


#define LOWTILT     (+0)    /* [degree * 0.1]*/
#define HIGHTILT    (+900)
#define LOWPAN      (-1800)
#define HIGHPAN     (+1800)

// PWM Values for double servo mechanic:
#define LOWTILT_PWM     (1000)
#define HIGHTILT_PWM    (1700)
#define LOWPAN_PWM      (1500-338)
#define HIGHPAN_PWM     (1500+338)

//#define LOWPANLIMIT     (-1800) 
//#define HIGHPANLIMIT    (+1800)


#define CENTERPAN       ((HIGHPAN-LOWPAN) / 2)
#define CENTERTILT      ((HIGHTILT-LOWTILT) / 2)

#define MINSATELITES    (4) // 4 is for indoor testing

#define PANPIN  2
#define TILTPIN 4
#define POTIPIN 34
#define AKKUPIN 35

#define AKKUFACTOR (10.7f)



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
    int16_t i16panpwm = 1500;
    int16_t i16tiltpwm = 1500;

    float AkkuVoltage =0.0f;
    

public:
    tracker(/* args */);
    ~tracker();

    void setup( void );
    void loop( crsf_telemetrie &crsf );
    void setHome( gps &h );
    void setPlane( gps &p );
    bool updateCalculation( crsf_telemetrie &crsf );
    int16_t getPanPwm( int16_t i16Angle );
    int16_t getTiltPwm( int16_t i16Angle );
    void setZero( int16_t i16pan, int16_t i16tilt, bool bStore );
    void setServos( int16_t i16pan, int16_t i16tilt );
    int16_t readNorth( void );

    void setStepper(  int16_t i16AngValue  );

};

#endif
