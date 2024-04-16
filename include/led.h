#ifndef classled
#define classled

#include <cstdint>

#define LED_POWER   1
#define LED_BLE     2
#define LED_GPS     3
#define LED_STEPPER 4
#define LED_HOME    5
#define LED_TRACKER 6

#define STATUS_WAIT 0
#define STATUS_BUSY 1
#define STATUS_FAIL 2
#define STATUS_OK   3

class led
{
private:
    /* data */
    bool update;

public:
    led(/* args */);
    ~led();

    void setup( void );
    void loop( void );
    void setState( uint8_t ledNr, uint8_t ledStatus );
    uint8_t getState( void );

    //ack nack....
};

#endif