/*
*/

#ifndef hardwarepins
#define hardwarepins

//#define PLATFORM_ESP32
#define PLATFORM_ESP32_C3



#ifdef PLATFORM_ESP32

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

#define POTIPIN 34
#define AKKUPIN 35

#endif



#ifdef PLATFORM_ESP32_C3 
// see also: https://www.sudo.is/docs/esphome/boards/esp32c3supermini/
//button.cpp:
#define BLUE_BUTTON_PIN     6
#define RED_BUTTON_PIN      7

//led.cpp
#define LED_PIN             8   // pin 8 is also used by the blue status led

// servo.cpp
//#define PANPIN              
#define TILTPIN             9   // pin 9 is also used by the boot button

// stepper.cpp, Stepper pin definitions
#define STEP_PIN            1    // step
#define DIR_PIN             0    // direction
//#define SPREAD_PIN          7    // pull down for stealth shop, always low, 20 is rx, 21 tx
#define MS1_PIN             2    // micro steps, 1-1 for 1/16, 0-0 for 1/8 , so it can drive both pins
//#define MS2_PIN             5    // micro steps
//#define ENABLE_PIN          20   // pull down to enable, always low
#define DIAG_PIN            5    // Diag signal from tmc2209
//#define IDX_PIN             21    // Index pin from tmc2209
#define INDEX_PIN           10   // hall sensor input (internal pullup needed)

#define POTIPIN             4   // pin 4 is analog input adc1
#define AKKUPIN             3   // pin 3 is analog input adc1, pin 5 is analog input adc2 - does not work

#define RX_PIN              20
#define TX_PIN              21

#endif //platform


#endif // header
