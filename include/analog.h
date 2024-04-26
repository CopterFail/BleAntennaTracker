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

    int16_t median5( int16_t a, int16_t b, int16_t c, int16_t d, int16_t e );
    int16_t readNorth( void );
    int16_t readBattery( void );

};

#endif