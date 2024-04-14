

#include <Arduino.h>
#include "stepper.h"

// Stepper definitions
#define STEP_PIN  14
#define DIR_PIN   12
#define FAST_PIN  13
#define STEP_LIMIT (100*16*4)   // counting micro steps (1/16) and 1:4 gear for one direction
#define FAST_FACTOR (4)       // quarter steps instead of 1/16
#define MIN_INTERVAL  10000   // 100 Hz call frequency for new stepper values
#define MIN_ISR_TIME  200     // 1000 Hz interrupt
#define MAX_ISR_TIME  10000   // 100 Hz interrupt

volatile int iStepperPos=0;
volatile int iStepperSet=0;
volatile int iStepperFactor=1;

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
      iStepperPos+= iStepperFactor;
    }
    else if( (diff < 0) && (iStepperPos > -STEP_LIMIT) ) 
    {
      digitalWrite(DIR_PIN, LOW );
      digitalWrite(STEP_PIN, HIGH);
      iStepperPos-= iStepperFactor;
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
    pinMode( STEP_PIN, OUTPUT );
    pinMode( DIR_PIN, OUTPUT );
    pinMode( FAST_PIN, OUTPUT );

    timer = timerBegin(0, 80, true);           	// timer 0, prescalar: 80, UP counting
    timerAttachInterrupt(timer, &onTimer, true); 	// Attach interrupt
    timerAlarmWrite(timer, 1000, true);  		// Match value= 1000 for 1ms, 100 for 0.1ms. delay.
    timerAlarmEnable(timer);           			// Enable Timer with interrupt (Alarm Enable)
}

void stepper::setStepper( uint16_t i16AngValue )
{
  int ipos, iset, idiff;
  int isrtime;
  static unsigned long last = 0;
  unsigned long now;
  unsigned long interval;
  bool bFast = false;
  
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
      isrtime *= FAST_FACTOR;
      bFast = true;
      if( isrtime < (MIN_ISR_TIME) ){   // use a longer isrtime with faster steps?
        isrtime = MIN_ISR_TIME;
      }
    }
    if( isrtime > MAX_ISR_TIME ){
      isrtime = MAX_ISR_TIME;
    }

    if( bFast ){ 
      digitalWrite(FAST_PIN, LOW);
      iStepperFactor = FAST_FACTOR; // set full steps
    }else{
      digitalWrite(FAST_PIN, HIGH); // set quarter steps
      iStepperFactor = 1;
    }
    iStepperSet = iset;
    timerAlarmWrite(timer, isrtime, true);  		

    if( bSim ){
      Serial.println( String(interval/idiff) + " / " + String(isrtime) + " / " + String(iStepperFactor) ); 
      Serial.println( String(i16AngValue) + " / " + String(iStepperSet) + " / " + String(iStepperPos) ); 
    }
    else
    {
      timerAlarmEnable(timer);
    }
    
  }
}