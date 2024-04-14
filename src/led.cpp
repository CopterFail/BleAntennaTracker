


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
    strip.SetPixelColor(0, red);
    strip.Show();
}
    
void led::loop( void )
{
    strip.SetPixelColor(0, red);
    strip.SetPixelColor(1, green);
    strip.SetPixelColor(2, blue);
    strip.SetPixelColor(3, white);
    strip.SetPixelColor(4, red);
    strip.SetPixelColor(5, green);
    strip.SetPixelColor(6, blue);
    strip.SetPixelColor(7, white);
    strip.Show();
}

void led::setStatus( uint8_t ledNr, uint8_t letStatus )
{

}
