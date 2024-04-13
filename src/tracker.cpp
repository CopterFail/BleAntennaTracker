/**
 * BLE Anenna Tracker
 * by CopterFail 2024
*/

#include <Arduino.h>
#include <Preferences.h>
#include "PWM.h"

#include "tracker.h"
#include "crsf_telemetrie.h"

#define PAN_CHANNEL   0
#define TILT_CHANNEL  1
#define OK_CHANNEL    3
#define ARM_CHANNEL   4

#define STEP_PIN 14
#define DIR_PIN 12
#define STEP_LIMIT (400*16) //counting micro steps and 1:4 gear



PWMController PWM;
pwm_channel_t chPan, chTilt;
Preferences preferences;


volatile int iStepperPos=0;
volatile int iStepperSet=0;

 
hw_timer_t * timer = NULL;      //H/W timer defining (Pointer to the Structure)

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {      //Defining Inerrupt function with IRAM_ATTR for faster access
  static bool state = true;
  int diff;

  if( state )
  {
    diff = iStepperSet - iStepperPos;
    portENTER_CRITICAL_ISR(&timerMux);
    if( (diff > 0) && (iStepperPos < +STEP_LIMIT) )
    {
      digitalWrite(DIR_PIN, HIGH );
      digitalWrite(STEP_PIN, HIGH);
      iStepperPos++;
      // iStepperPos+=4; check speedup
    }
    else if( (diff < 0) && (iStepperPos > -STEP_LIMIT) ) 
    {
      digitalWrite(DIR_PIN, LOW );
      digitalWrite(STEP_PIN, HIGH);
      iStepperPos--;
      // iStepperPos-=4; for full steps
    }
    else
    {
      digitalWrite(STEP_PIN, LOW);
    }
    state = false;
    
    portEXIT_CRITICAL_ISR(&timerMux);
      
  }
  else
  {
      digitalWrite(STEP_PIN, LOW);
      state = true;
  } 
}
 


tracker::tracker(/* args */)
{
}

tracker::~tracker()
{
}


void  tracker::setup( void )
{
  int16_t i16North;
  int16_t i16Horizontal;

  //readback nvs:
  preferences.begin("tracker", false); 
  i16North = preferences.getShort("dir_north", 1500);
  i16Horizontal =  preferences.getShort("dir_horizontal", 1500);
  setZero( i16North, i16Horizontal, false );
  home.setLat( preferences.getInt("pos_lat", 0) );
  home.setLon( preferences.getInt("pos_lon", 0) );
  preferences.end();

  HomeIsSet = (home.getLat() != 0 ) && (home.getLon() != 0 );
  Serial.println( "Home: " + String(home.getLat()) + "/" + String(home.getLon()) );

   // Init the 2 servo pwm channels
  chPan = PWM.allocate( PANPIN, 50 ); 
  PWM.setDuty( chPan, 20000 );  // 20ms duty
  chTilt = PWM.allocate( TILTPIN, 50 ); 
  PWM.setDuty( chTilt, 20000 );  // 20ms duty
  
  // Set initial servo position, or wait for more information?
  //PWM.setMicroseconds( chPan, 1500 ); 
  //PWM.setMicroseconds( chTilt, 1500 ); 

  pinMode( STEP_PIN, OUTPUT );
  pinMode( DIR_PIN, OUTPUT );

  timer = timerBegin(0, 80, true);           	// timer 0, prescalar: 80, UP counting
  timerAttachInterrupt(timer, &onTimer, true); 	// Attach interrupt
  timerAlarmWrite(timer, 1000, true);  		// Match value= 1000 for 1ms, 100 for 0.1ms. delay.
  timerAlarmEnable(timer);           			// Enable Timer with interrupt (Alarm Enable)


}

void  tracker::loop( crsf_telemetrie &crsf )
{

  (void)readNorth();

#ifndef SIMULATE
  if( crsf.getChannel(ARM_CHANNEL) < 1200 )
  {
    // overwrite the pwm values:
    //i16panpwm = crsf.getChannel(PAN_CHANNEL);
    //i16panpwm = getPanPwm( i16panzero );
    //i16panpwm = crsf.getChannel(PAN_CHANNEL);
    i16tiltpwm = crsf.getChannel(TILT_CHANNEL);
    setServos( i16panpwm, i16tiltpwm );
    //setStepper( ((float)i16panpwm-1500.0)*(1800/500) );
  setStepper( i16panzero );

    if( crsf.getChannel(OK_CHANNEL) > 1900 )
    {
        //setZero( i16panpwm, i16tiltpwm, true );
    }
    if( crsf.getChannel(OK_CHANNEL) < 1200 )
    {
        HomeIsSet = false;
    }
  }
  else 
#endif  
  if( updateCalculation( crsf ) )
  {
    //i16pan = getPan();
    //i16tilt = getTilt();
    setServos( i16panpwm, i16tiltpwm );
    setStepper( i16pan );
  }

}

bool tracker::updateCalculation( crsf_telemetrie &crsf )
{
    bool result = false;
    float distance = 0.0;
    static int16_t i16GpsPacketCount = 11;

#ifdef SIMULATE
    static float ang=0.0;
    static int16_t height=0;
    home.set(510000000, 67000000, 5, 0 );
    HomeIsSet = true;
    plane.simulate( home, 500, ang, height );
    ang+=0.5;
    height++;
    if( ang > 180 ) ang -= 360;
    else if( ang < -180 ) ang += 360;
    if( true )
#else    
    if( crsf.getLatestGps( plane ) && plane.getSatelites() >= MINSATELITES )
#endif
{
        result = true;
        if( !HomeIsSet )
        {
            setHome( plane );
        }
        else
        {
            i16pan = (int16_t)(home.degree( plane ) * (1800.0/M_PI)); /* range is [-1800;+1800] */
            i16pan += i16panzero; /* add offset, i16panzero is in range [-1800;1800] -> [-3600;+3600]*/
            if( i16pan < -1800 ) i16pan += 3600; /* set range to [-1800..1800] */
            if( i16pan > +1800 ) i16pan -= 3600; 

            i16tilt = home.tilt( plane ) * (1800.0/M_PI); /* range is [0;900]*/
            i16tilt += i16tiltzero;
            
            // limit angles to the hardware range
            if( i16tilt < LOWTILT )  i16tilt = LOWTILT;
            if( i16tilt > HIGHTILT )  i16tilt = HIGHTILT;
            if( i16pan < LOWPAN )  i16pan = LOWPAN;
            if( i16pan > HIGHPAN )  i16pan = HIGHPAN;

#if 0
            Serial.println( "home: " + String(home.getLat()) + "/" + String(home.getLon()) );
            Serial.println( "plane: " + String(plane.getLat()) + "/" + String(plane.getLon()) );
#endif
            distance = home.dist( plane );
//            Serial.println(" Dist:" + String(distance) + " Ang:" + String(i16pan) + " Tilt:" + String(i16tilt));

            // calculate the servo output values
            i16panpwm = getPanPwm( i16pan );
            i16tiltpwm = getTiltPwm( i16tilt );
        }
    }   
    return result;
}

int16_t tracker::getPanPwm( int16_t i16Angle )
{
    int16_t i16result;
    i16result = map( i16Angle, LOWPAN, HIGHPAN, LOWPAN_PWM, HIGHPAN_PWM);  /* swap to change direction? */
    return i16result;
}

int16_t tracker::getTiltPwm( int16_t i16Angle )
{
    int16_t i16result;
    i16result = map( i16Angle, LOWTILT, HIGHTILT, LOWTILT_PWM, HIGHTILT_PWM );
    return i16result;
}

void tracker::setHome( gps &h )
{
  home = h;
  HomeIsSet = (home.getLat() != 0 ) && (home.getLon() != 0 );
  if( HomeIsSet )
  {
    preferences.begin("tracker", false); 
    preferences.putInt("pos_lat", home.getLat());
    preferences.putInt("pos_lon", home.getLon());
    preferences.end();
    Serial.println( "Home: " + String(home.getLat()) + "/" + String(home.getLon()) );
  }
}

void tracker::setPlane( gps &p )
{

}

void tracker::setZero( int16_t i16pan, int16_t i16tilt, bool bStore )
{
    /* parameter range is 1000 .. 2000 [us] */
    i16panzero = map( i16pan, LOWPAN_PWM, HIGHPAN_PWM, LOWPAN, HIGHPAN );
    i16tiltzero = map( i16tilt, LOWTILT_PWM, HIGHTILT_PWM, LOWTILT, HIGHTILT );

    i16panzero -= CENTERPAN;
    i16tiltzero -= CENTERTILT;

    //ToDo: save non volatile
    if( bStore )
    {
      preferences.begin("tracker", false); 
      preferences.putShort("dir_north", i16pan);
      preferences.putShort("dir_horizontal", i16tilt);
      preferences.end();
    }

    Serial.println("Zero (p/t):" + String(i16panzero) + " Ang:" + String(i16tiltzero));
}

void tracker::setServos( int16_t i16pan, int16_t i16tilt )
{
  PWM.setMicroseconds( chPan, i16pan );
  PWM.setMicroseconds( chTilt, i16tilt );
}

int16_t tracker::readNorth( void )
{
      //I cannot use these functions:
    //analogSetCycles(255);
    //adcStart(POTIPIN);
    //adcBusy(POTIPIN);
    //resultadcEnd(POTIPIN);
    static int valadc = 1700;
    valadc = ( 2 * valadc + analogRead(POTIPIN)) / 3; // Poti value in in range of, 0..3510 (4092 is not reached)
    i16panzero = map( valadc, 0, 3510, LOWPAN, HIGHPAN ); // das muss an eine bedingung gebunden werden
//i16panzero = 0; // big noise, try median filter?
    AkkuVoltage = AKKUFACTOR * 2.5 / 4096 * analogRead(AKKUPIN);
    //Serial.println( AkkuVoltage );
    //Serial.println(i16panzero); // Wert ausgeben
    return i16panzero;
}






#define MIN_INTERVAL  10000   // 100 Hz call frequency
#define MIN_ISR_TIME  200     // 5000 Hz interrupt
#define MAX_ISR_TIME  10000   // 100 Hz interrupt

void tracker::setStepper( int16_t i16AngValue )
{
  int ipos, iset, idiff;
  int isrtime;
  static unsigned long last = 0;
  unsigned long now;
  unsigned long interval;
  
  static int16_t Value = i16AngValue;
  Value = (3 * Value + i16AngValue ) / 4; // filter i16AngValue

  timerAlarmDisable(timer);

  ipos = iStepperPos;
  iset = map(Value, -1800,1800,-STEP_LIMIT,+STEP_LIMIT);
  idiff = iset - ipos;
  if( idiff < 0 )
  { 
    idiff = -idiff;
  }

  now = micros();
  interval = now - last;
  last = now;
  if( interval < MIN_INTERVAL ){
    interval = MIN_INTERVAL;
  }


  if( idiff > 0 ){
    isrtime = interval / idiff; 
    if( isrtime < MIN_ISR_TIME ){
      isrtime = MIN_ISR_TIME;
    }
    if( isrtime > MAX_ISR_TIME ){
      isrtime = MAX_ISR_TIME;
    }

    iStepperSet = iset;
    timerAlarmWrite(timer, isrtime, true);  		
    timerAlarmEnable(timer);
  }
  
  //Serial.println( String(interval) + " / " + String(isrtime) ); 
  //Serial.println( String(i16pan) + " / " + String(iStepperSet) + " / " + String(iStepperPos) ); 
}


