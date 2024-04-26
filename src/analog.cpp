
#include <Arduino.h>
#include "analog.h"
#include "led.h"
#include "hardware.h"

#define AKKUFACTOR (10.7f)

extern led myled;

 analog::analog()
 {
 }

analog::~analog()
{
}

void analog::setup( bool Simulation )
{
    sim = Simulation;
}
    
void analog::loop( void )
{
}

int16_t analog::median5( int16_t a, int16_t b, int16_t c, int16_t d, int16_t e )
{
    int16_t values[5] = {a,b,c,d,e};
    // Sortiere die Zahlen
    for (int i = 0; i < 5 - 1; i++) {
        for (int j = 0; j < 5 - i - 1; j++) {
            if (values[j] > values[j + 1]) {
                int16_t temp = values[j];
                values[j] = values[j + 1];
                values[j + 1] = temp;
            }
        }
    }
    return values[2];
}

int16_t analog::readNorth( void )
{
    static int16_t valadcpan = 1700;
    float valadcbat = 0.0;
    static int16_t index = 0, values[5] = {0,0,0,0,0}, v;
    
    values[ index ] = analogRead(POTIPIN);
    index = (index+1) % 5;
    v = median5(values[0], values[1], values[2], values[3], values[4]);

    valadcpan = ( 2 * valadcpan + v ) / 3; // Poti value in in range of, 0..3510 (4092 is not reached)

    if( sim ){
        Serial.println( "North offset: " + String(valadcpan) ); // Wert ausgeben
    }
    return valadcpan;
}

int16_t analog::readBattery( void )
{
    float valadcbat = 0.0;
    static float AkkuVoltage =0.0f;
    static int16_t index = 0, values[5] = {0,0,0,0,0}, v;
    
    values[ index ] = analogRead(AKKUPIN);
    index = (index+1) % 5;
    v = median5(values[0], values[1], values[2], values[3], values[4]);


    valadcbat = AKKUFACTOR * 4.0 / 4096 * float(v); 
    AkkuVoltage = ( 10.0 * AkkuVoltage + valadcbat ) / 11.0;

    if( AkkuVoltage > 10.0 ) myled.setState( LED_POWER, STATUS_OK );
    else  myled.setState( LED_POWER, STATUS_FAIL );

    if( sim ){
        Serial.println( "Battery: " + String(AkkuVoltage) );
    }
    return AkkuVoltage;
}


