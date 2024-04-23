#ifndef classstepper
#define classstepper

#include <cstdint>



class stepper
{
private:
    /* data */
    bool bSim = false;
    int iMinIsrTime;
    int16_t i16AngSetpoint;
    int16_t i16AngFiltered;
    uint32_t u32setpointInterval;

public:
    stepper(/* args */){}
    ~stepper(){}

    void setup( bool bSimulation );
    void loop( void );
    bool findIndex( void );
    void setStepper( int16_t i16AngValue );
    void setMicroStep( int ifactor );

    //void setMaxSpeed( ); //deg/us ? 
    uint8_t getState( void );

private:    
    void filter( void );
    void update( void );
};

#endif