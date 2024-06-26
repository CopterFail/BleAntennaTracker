/**
 * led class
 * by CopterFail 2024
*/

#ifndef classled
#define classled

#include <cstdint>

#define LED_POWER   1
#define LED_BLE     2
#define LED_GPS     3
#define LED_STEPPER 4
#define LED_HOME    5
#define LED_TRACKER 6
#define LED_FLASH1  0
#define LED_FLASH2  7

#define STATUS_WAIT 0
#define STATUS_BUSY 1
#define STATUS_FAIL 2
#define STATUS_OK   3

class led
{
private:
    /* data */
    bool update;
    uint8_t i8flashcnt;
    
public:
    led(/* args */);
    ~led();

    void setup( void );
    void loop( void );
    void setState( uint8_t ledNr, uint8_t ledStatus );
    uint8_t getState( void );
    void flash( bool bOk );

    //ack nack....
};

#endif