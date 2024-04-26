
#include "PWM.h"
#include "tracker.h"
#include "servo.h"
#include "hardware.h"

PWMController PWM;
pwm_channel_t chPan, chTilt;


void servo::setup( bool bSimulation )
{
    bSim = bSimulation;
    if( bSim ){
        Serial.println("Simulation for servo is active ");
    }
    // Init the 2 servo pwm channels
    #ifdef PANPIN
    chPan = PWM.allocate( PANPIN, 50 ); 
    PWM.setDuty( chPan, 20000 );  // 20ms duty
    #endif
    chTilt = PWM.allocate( TILTPIN, 50 ); 
    PWM.setDuty( chTilt, 20000 );  // 20ms duty

    // Set initial servo position, or wait for more information?
    //PWM.setMicroseconds( chPan, 1500 ); 
    //PWM.setMicroseconds( chTilt, 1500 ); 
}

void servo::loop( void )
{

}

void servo::setServos( int16_t i16Pan, int16_t i16Tilt )
{
    int16_t i16PanPWM = map( i16Pan, LOWPAN, HIGHPAN, LOWPAN_PWM, HIGHPAN_PWM);
    int16_t i16TiltPWM = map( i16Tilt, LOWTILT, HIGHTILT, LOWTILT_PWM, HIGHTILT_PWM);
    
    if( bSim )
    {
        Serial.println( String(i16PanPWM) + " / " + String(i16TiltPWM) ); 
    }
    else
    {
        #ifdef PANPIN
        PWM.setMicroseconds( chPan, i16PanPWM );
        #endif
        PWM.setMicroseconds( chTilt, i16TiltPWM);
    }
}


