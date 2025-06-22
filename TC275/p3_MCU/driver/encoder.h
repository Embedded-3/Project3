// encoder.h
#ifndef ENCODER_H
#define ENCODER_H

#include <stdbool.h>
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"


#include "IfxPort_PinMap.h"

#define ISR_PRIORITY_LEFT_ENCODER   5
#define ISR_PRIORITY_RIGHT_ENCODER  7

#define LEFT_ENCODER_PIN_A      &MODULE_P10, 2  // d13
#define LEFT_ENCODER_PIN_B      &MODULE_P10, 1  // d12

#define RIGHT_ENCODER_PIN_A     &MODULE_P10, 3   // d11
#define RIGHT_ENCODER_PIN_B     &MODULE_P10, 5   // d10

#define PPR             1950        // 한바퀴당 펄스
#define DIAMETER        6.6         // 바퀴 지름 (cm)



extern volatile int left_duration;
extern volatile int right_duration;


void initEncoder(void);


#endif