/**
 * analog class
 * by CopterFail 2024
*/

#ifndef classanalog
#define classanalog

#include <cstdint>

class analog
{
private:
    /* data */
    bool sim = false;
    
public:
    analog();
    ~analog();

    void setup( bool Simulation );
    void loop( void );

    int16_t median5( int16_t *values );

    int16_t readNorth( void );
    int16_t readBattery( void );

};

#endif