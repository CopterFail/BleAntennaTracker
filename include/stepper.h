#ifndef classstepper
#define classstepper

#include <cstdint>



class stepper
{
private:
    /* data */
    bool bSim = false;

public:
    stepper(/* args */){}
    ~stepper(){}

    void setup( bool bSimulation );
    void loop( void ){};
    void setStepper( int16_t i16PanPWM );
};

#endif