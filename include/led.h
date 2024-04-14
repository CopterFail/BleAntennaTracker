#ifndef classled
#define classled


#include <cstdint>

class led
{
private:
    /* data */

public:
    led(/* args */);
    ~led();

    void setup( void );
    void loop( void );
    void setStatus( uint8_t ledNr, uint8_t letStatus );
};

#endif