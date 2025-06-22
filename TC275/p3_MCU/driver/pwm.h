// pwm.h
#ifndef PWM_H
#define PWM_H

#include "Ifx_Types.h"
#include "IfxGtm_Tom_Pwm.h"
#include "IfxGtm_Cmu.h"
#include "IfxGtm_Atom_Pwm.h"


#define CLK_FREQ                     100000000.0f // 1000000.0f


// ATOM 모듈 0
#define RearRL           IfxGtm_ATOM0_3_TOUT3_P02_3_OUT // D5 pin
#define Motor_DIR_PIN       &MODULE_P02,4               // D6 pin

#define PWM_PERIOD                   4000
#define DEFAULT_DUTY_REAR      0 //STOP                

#define NUM_WHEELS 1


typedef enum WHEEL{
    REAR = 0,
} e_Wheel_t;


void initPwm(void);
void startPwm(void);
void setPwm(const e_Wheel_t whichWheel, uint32 dutyCycle);
void setAllMotor(uint32 dutyCycle);

#endif
/*
무부하 엔코더 값 기록

속도(듀티) | 엔코더 값
1700    680?    // 진짜 최저 속도 거북이임
MIN     1380
MAX     3100
3000    2400
2500    1760

*/