#ifndef classstepper
#define classstepper

#include <cstdint>



class stepper
{
private:
    /* data */
    bool bSim = false;
    int iMinIsrTime;

public:
    stepper(/* args */){}
    ~stepper(){}

    void setup( bool bSimulation );
    void loop( void ){};
    bool findIndex( void );
    void setStepper( int16_t i16PanPWM );
    //void setMaxSpeed( ); //deg/us ? 
    uint8_t getState( void );
};

#endif