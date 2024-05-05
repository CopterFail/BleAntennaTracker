/**
 * GPS class
 * by CopterFail 2024
*/


#include <Arduino.h>
#include "led.h"
#include "stepper.h"
#include "hardware.h"

#define STEP_LIMIT (100*16*4)   // counting micro steps (1/16) and 1:4 gear for one direction
//#define FAST_FACTOR (2)       // 1/8 steps instead of 1/16 - works well
#define FAST_FACTOR (2)       // 1/8 steps instead of 1/16 - test
#define MIN_INTERVAL  10000   // 100 Hz call frequency for new stepper values
#define MAX_INTERVAL  2000000 // upper limit for new stepper values
#define MIN_ISR_TIME  125     // 8000 Hz interrupt should be fast enougth
#define MAX_ISR_TIME  10000   // 100 Hz interrupt
#define INDEX_SIZE    (110)   // size is 230 ... 210 not symetrically

volatile int iStepperPos = 0;
volatile int iStepperSet = 0;
volatile int iStepperFactor = 1;
volatile bool bIndexFound = false;
volatile int iDebug;

 hw_timer_t * timer = NULL;      //H/W timer defining (Pointer to the Structure)

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {      //Defining Inerrupt function with IRAM_ATTR for faster access
  static bool state = true;
  static int ilastDir = 0;
  static bool bIndex;
  int diff = 0;

  if( LOW == digitalRead(INDEX_PIN) ) // light-bridge replaced by hall sensor (not symmetric)
  {
    if( !bIndex )
    {
      bIndex = true;
      bIndexFound = true;
      if( ilastDir > 0)
      {
        iStepperPos = -INDEX_SIZE; 
      }
      else
      {
        iStepperPos = +INDEX_SIZE;
      }
    }
  }
  else
  {
    if(bIndex)iDebug = iStepperPos;
    bIndex = false;
  }

  if( state )
  {
    diff = iStepperSet - iStepperPos;
    portENTER_CRITICAL_ISR(&timerMux);
    if( (diff > 0) && (iStepperPos < +STEP_LIMIT) )
    {
      digitalWrite(DIR_PIN, HIGH );
      digitalWrite(STEP_PIN, HIGH);
      iStepperPos+= iStepperFactor;
      ilastDir = +iStepperFactor;
    }
    else if( (diff < 0) && (iStepperPos > -STEP_LIMIT) ) 
    {
      digitalWrite(DIR_PIN, LOW );
      digitalWrite(STEP_PIN, HIGH);
      iStepperPos-= iStepperFactor;
      ilastDir = -iStepperFactor;
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

void stepper::setup( bool bSimulation )
{
    bSim = bSimulation;
    if( bSim ){
      Serial.println("Simulation for stepper is active ");
    }

    pinMode( STEP_PIN, OUTPUT );
    pinMode( DIR_PIN, OUTPUT );
    pinMode( MS1_PIN, OUTPUT );
    #ifdef MS2_PIN
    pinMode( MS2_PIN, OUTPUT );
    #endif

    #ifdef SPREAD_PIN
    pinMode( SPREAD_PIN, OUTPUT );
    digitalWrite(SPREAD_PIN, LOW );
    #endif

    #ifdef ENABLE_PIN
    digitalWrite(ENABLE_PIN, HIGH );
    pinMode( ENABLE_PIN, OUTPUT );
    #endif

    #ifdef DIAG_PIN
    pinMode( DIAG_PIN, INPUT );
    #endif
    //pinMode( IDX_PIN, INPUT );
    pinMode( INDEX_PIN, INPUT_PULLUP );

    iStepperFactor = 1;
    setMicroStep(iStepperFactor);

    iMinIsrTime = MIN_ISR_TIME;
    u32setpointInterval = MAX_ISR_TIME;

    timer = timerBegin(0, 80, true);           	// timer 0, prescalar: 80, UP counting
    timerAttachInterrupt(timer, &onTimer, true); 	// Attach interrupt
    timerAlarmWrite(timer, 1000, true);  		// Match value= 1000 for 1ms, 100 for 0.1ms. delay.
    timerAlarmEnable(timer);           			// Enable Timer with interrupt (Alarm Enable)

    findIndex();
}

bool stepper::findIndex( void )
{
    //iMinIsrTime = 4 * MIN_ISR_TIME;
    for( int i=0; (i<(STEP_LIMIT*2)) && (bIndexFound == false); i+=60 ){
      iStepperPos = 0;
      setStepper( 60 );
      loop();
      delay( 50 );
    }
    setStepper( 0 );
    loop();
    delay( 1000 );
    iMinIsrTime = MIN_ISR_TIME;
    return bIndexFound;
}
uint8_t stepper::getState( void )
{
  uint8_t result = STATUS_FAIL;
  if( bIndexFound ) result = STATUS_OK;
  return result;
}

void stepper::setMicroStep( int ifactor )
{
  switch( ifactor )
  {
    case 2: // 1/8
      digitalWrite(MS1_PIN, LOW );
      #ifdef MS2_PIN
        digitalWrite(MS2_PIN, LOW );
      #endif
      break;
    default:
    case 1: // 1/16
      digitalWrite(MS1_PIN, HIGH );
      #ifdef MS2_PIN
      digitalWrite(MS2_PIN, HIGH );
      #endif
      break;
  }
}

void stepper::loop( void )
{
  filter();
  update();
}

void stepper::update( void )
{
  int ipos, iset, idiff;
  int isrtime;
  bool bFast = false;
  
  timerAlarmDisable(timer);

  ipos = iStepperPos;
  iset = map( i16AngFiltered, -1800,1800,-STEP_LIMIT,+STEP_LIMIT );
  idiff = iset - ipos;
  if( idiff < 0 )
  { 
    idiff = -idiff;
  }

  if( idiff > 0 ){

    // isrtime is time per step 125 ... 10000 -> 8000 ... 100 Steps/s
    isrtime = u32setpointInterval / idiff; // isrtime is time per step -> 8000 ... 100 Steps/s (for 1/16)

    if( isrtime < iMinIsrTime ){
      isrtime *= FAST_FACTOR;
      bFast = true;
      if( isrtime < (iMinIsrTime) ){   // use a longer isrtime with faster steps?
        isrtime = iMinIsrTime;
      }
    }
    if( isrtime > MAX_ISR_TIME ){
      isrtime = MAX_ISR_TIME;
    }

    if( bFast ){ 
      iStepperFactor = FAST_FACTOR; // set faster steps
    }else{
      iStepperFactor = 1; // set 1/16 steps
    }
    setMicroStep(iStepperFactor);
    iStepperSet = iset;
    #ifdef ENABLE_PIN
    digitalWrite(ENABLE_PIN, LOW );
    #endif
    timerAlarmWrite(timer, isrtime, true);  		
    timerAlarmEnable(timer);

    if( bSim ){
      Serial.println( String(u32setpointInterval) + " / " + String(isrtime) + " / " + String(iStepperFactor) ); 
      Serial.println( String(i16AngFiltered) + " / " + String(iStepperSet) + " / " + String(iStepperPos) ); 
      //Serial.println( String(iDebug));
    }
  }
}

void stepper::filter( void )
{
  const int16_t i16FilterLevel = 2;
  i16AngFiltered = (i16FilterLevel * i16AngFiltered + i16AngSetpoint ) / (i16FilterLevel+1); 
}

void stepper::setStepper( int16_t i16NewAngValue )
{
  static uint32_t u32last = 0;
  uint32_t  u32now;

  u32now = micros();  // 
  u32setpointInterval = (u32now - u32last) / 2u; // we need 2 interrupts for 1 step
  u32last = u32now;
  if( u32setpointInterval < MIN_INTERVAL ){
      u32setpointInterval = MIN_INTERVAL;
  }
  if( u32setpointInterval > MAX_INTERVAL ){
      u32setpointInterval = MAX_INTERVAL;
  }

  i16AngSetpoint = i16NewAngValue;
  //loop();
}
