/**
 * GPS class
 * by CopterFail 2024
*/

#include <Arduino.h>
#include "button.h"
#include "hardware.h"

button mybutton;

void IRAM_ATTR buttonisr() 
{
    if( digitalRead(BLUE_BUTTON_PIN) == LOW )
	    mybutton.bPressedBlue = true;
    if( digitalRead(RED_BUTTON_PIN) == LOW )
	    mybutton.bPressedRed = true;
}

void button::setup( bool bSimulation )
{
    bSim = bSimulation;
    pinMode(BLUE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
	attachInterrupt(BLUE_BUTTON_PIN, buttonisr, FALLING);
	attachInterrupt(RED_BUTTON_PIN, buttonisr, FALLING);
}

void button::loop( void )
{
    if( bPressedBlue ) 
    {
        if( bSim )
        {
            Serial.println("Blue Button pressed");
        }
        bPressedBlue = false;
    }
    if( bPressedRed ) 
    {
        if( bSim )
        {
            Serial.println("Red Button pressed");
        }
        bPressedRed = false;
    }
}

void button::getButton( void )
{

}
