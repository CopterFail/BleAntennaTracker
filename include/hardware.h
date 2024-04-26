/*
*/

#ifndef hardwarepins
#define hardwarepins

#define ESP32_PINOUT
//#define ESP32C3_PINOUT

#ifdef ESP32_PINOUT 

//button.cpp:
#define BLUE_BUTTON_PIN     25
#define RED_BUTTON_PIN      26

//led.cpp
#define LED_PIN             23

// servo.cpp
#define PANPIN              2
#define TILTPIN             4

// stepper.cpp, Stepper pin definitions
#define STEP_PIN            14    // step
#define DIR_PIN             12    // driection
#define SPREAD_PIN          13    // pull down for stealth shop
#define MS1_PIN             18    // micro steps, 1-1 for 1/16, 0-0 for 1/8 
#define MS2_PIN             19    // micro steps
#define ENABLE_PIN          17    // pull down to enable
//#define DIAG_PIN            16    // Diag signal from tmc2209
//#define IDX_PIN             21    // Index pin from tmc2209
#define INDEX_PIN           27    // hall sensor input (internal pullup needed)

#endif



#ifdef ESP32C3_PINOUT 

//button.cpp:
#define BLUE_BUTTON_PIN     0
#define RED_BUTTON_PIN      1

//led.cpp
#define LED_PIN             2

// servo.cpp
#define PANPIN              3
#define TILTPIN             4

// stepper.cpp, Stepper pin definitions
#define STEP_PIN            5    // step
#define DIR_PIN             6    // driection
#define SPREAD_PIN          7    // pull down for stealth shop
#define MS1_PIN             8    // micro steps, 1-1 for 1/16, 0-0 for 1/8 
#define MS2_PIN             9    // micro steps
#define ENABLE_PIN          20   // pull down to enable
//#define DIAG_PIN            16    // Diag signal from tmc2209
//#define IDX_PIN             21    // Index pin from tmc2209
#define INDEX_PIN           10   // hall sensor input (internal pullup needed)

#endif


#endif
