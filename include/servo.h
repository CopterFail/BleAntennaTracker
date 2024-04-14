#ifndef classservo
#define classservo

#include <cstdint>


// PWM Values for double servo mechanic:
#define LOWTILT_PWM     (1000)
#define HIGHTILT_PWM    (1700)
#define LOWPAN_PWM      (1500-338)
#define HIGHPAN_PWM     (1500+338)

class servo
{
private:
    /* data */
    bool bSim = false;

public:
    servo(/* args */){}
    ~servo(){}

    void setup( bool bSimulation );
    void loop( void );
    void setServos( int16_t i16PanPWM, int16_t i16TiltPWM );
};

#endif