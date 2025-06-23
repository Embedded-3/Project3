#ifndef STEERING_H_
#define STEERING_H_

#define ISR_PRIORITY_TOM    20                                      /* Interrupt priority number                    */
#define STEERING               IfxGtm_TOM0_13_TOUT5_P02_5_OUT  // D7 핀
#define PWM_PERIOD          60000                                   /* PWM period for the TOM                       */

#define STEERING_PWM           7812
#define STEERING_PWM_CENTER    500
//#define STEERING_RIGHT_MAX      425
#define STEERING_RIGHT_MAX      500
//#define STEERING_LEFT_MAX       575
#define STEERING_LEFT_MAX       650

#include "Ifx_Types.h"
#include "IfxGtm_Tom_Pwm.h"
#include "asclin.h"
#include "IfxGtm_Cmu.h"
 
void initsteeringPWM(void);
void setSteeringDutyCycle(uint16 dutyCycle);
void mapSteeringAngleToPwm(sint8 angle);

extern sint8 angle;

#endif /* STEERING_H_ */
