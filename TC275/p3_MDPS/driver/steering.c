#include "driver/steering.h"
// STEERING용
IfxGtm_Tom_Pwm_Config g_steeringConfig;
IfxGtm_Tom_Pwm_Driver g_steeringDriver;

sint8 angle=0;
sint8 before_angle=0;

void initsteeringPWM(void)
{
    IfxGtm_enable(&MODULE_GTM);                                     /* Enable GTM                                   */
    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_FXCLK);   /* Enable the FXU clock*/

    IfxGtm_Tom_Pwm_initConfig(&g_steeringConfig, &MODULE_GTM);

    g_steeringConfig.tom = STEERING.tom;                                      /* Select the TOM depending on the LED          */
    g_steeringConfig.tomChannel = STEERING.channel;                           /* Select the channel depending on the LED      */
    g_steeringConfig.clock = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk2;
    g_steeringConfig.period = STEERING_PWM;                                /* Set the timer period                         */
    g_steeringConfig.dutyCycle = STEERING_PWM_CENTER;
    g_steeringConfig.pin.outputPin = &STEERING;                               /* Set the LED port pin as output               */
    g_steeringConfig.synchronousUpdateEnabled = TRUE;                    /* Enable synchronous update                    */

    IfxGtm_Tom_Pwm_init(&g_steeringDriver, &g_steeringConfig);                /* Initialize the GTM TOM                       */
    IfxGtm_Tom_Pwm_start(&g_steeringDriver, TRUE);                       /* Start the PWM                                */
}


void setSteeringDutyCycle(uint16 dutyCycle)
{
    g_steeringConfig.dutyCycle = dutyCycle;                              /* Change the value of the duty cycle           */
    IfxGtm_Tom_Pwm_init(&g_steeringDriver, &g_steeringConfig);                /* Re-initialize the PWM                        */

}

void mapSteeringAngleToPwm(sint8 angle)
{
    if(angle != before_angle){
        if (angle < -30.0f) angle = -30;
        if (angle >  30.0f) angle =  30;
        int pwm = STEERING_PWM_CENTER - (angle *(STEERING_LEFT_MAX - STEERING_PWM_CENTER) / 30);
        setSteeringDutyCycle(pwm);
        before_angle = angle;
        print("PWM SUCCESS\n\r");
    }
}
