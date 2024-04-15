


#include <NeoPixelBus.h>
#include "led.h"

const uint16_t PixelCount = 8; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t DotDataPin = 23;  
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, DotDataPin);


#define colorSaturation 64 //128
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);


 led::led(/* args */)
 {
 }

led::~led()
{
}

void led::setup( void )
{
    //strip.Begin(DotClockPin, DotDataPin, DotDataPin, DotChipSelectPin);
    strip.Begin();
    strip.ClearTo(black); 
    strip.Show();

    strip.SetPixelColor(LED_POWER, red);
    strip.SetPixelColor(LED_BLE, red);
    strip.SetPixelColor(LED_GPS, red);
    strip.SetPixelColor(LED_STEPPER, red);
    strip.SetPixelColor(LED_HOME, red);
    strip.SetPixelColor(LED_TRACKER, red);
    strip.Show();

    update = false;
}
    
void led::loop( void )
{
    if( update )
    {
        strip.Show();
    }
    update = false;
}

void led::setState( uint8_t ledNr, uint8_t ledStatus )
{
    switch( ledStatus ){
        case STATUS_BUSY:
            strip.SetPixelColor(ledNr, blue);
            break;
        case STATUS_OK:
            strip.SetPixelColor(ledNr, green);
            break;
        case STATUS_WAIT:
            strip.SetPixelColor(ledNr, white);
            break;
        case STATUS_FAIL:
        default:
            strip.SetPixelColor(ledNr, red);
            break;
    }
    update = true;
}
