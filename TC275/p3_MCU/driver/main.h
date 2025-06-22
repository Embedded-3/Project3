#ifndef MAIN_H
#define MAIN_H

#include "Platform_Types.h"

#define BRAKE_PIN             &MODULE_P11, 6    // 

void AppTask1ms(void);
void AppTask10ms(void);
void AppTask100ms(void);
void AppTask1000ms(void);


typedef enum {
    GO_FRONT = 0,
    GO_BACK = 1,
} e_GoDir_t;

typedef struct {
    uint16 pwm;
    e_GoDir_t dir;  // 방향
    sint16 speed;   // 측정 속도
} Motor_t;

extern Motor_t g_motor;

#endif
