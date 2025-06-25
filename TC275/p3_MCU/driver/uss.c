#include "Ifx_Types.h"
#include "IfxGtm_Tim_In.h"
#include "IfxStm.h"
#include "asclin.h"
#include "uss.h"

#define DELAY_STM    (&MODULE_STM0)
#define TRIG_PORT      &MODULE_P02
#define TRIG_PIN       1                    // d3
#define ECHO_INPUT     IfxGtm_TIM0_0_TIN0_P02_0_IN  // d2


IfxGtm_Tim_In g_driverTIM;
float32 g_distanceCm = 0.0f;

void delay_us(uint32 microseconds);


// USS를 사용하기 위해 main에서 초기 호출해야 하는 init함수
void ultrasonic_init(void)
{
    /* GTM enable */
    IfxGtm_enable(&MODULE_GTM);
    IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_CLK0);

    /* TRIG 핀 설정 (출력) */
    IfxPort_setPinModeOutput(TRIG_PORT, TRIG_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinLow(TRIG_PORT, TRIG_PIN);

    /* TIM 입력 설정 */
    IfxGtm_Tim_In_Config timConfig;
    IfxGtm_Tim_In_initConfig(&timConfig, &MODULE_GTM);
    timConfig.filter.inputPin = &ECHO_INPUT;
    timConfig.filter.inputPinMode = IfxPort_InputMode_pullDown;
    IfxGtm_Tim_In_init(&g_driverTIM, &timConfig);
}

// trigger 신호 생성 함수
void trigger_ultrasonic_pulse(void)
{
    IfxPort_setPinHigh(TRIG_PORT, TRIG_PIN);
    delay_us(10);  // 10us
    IfxPort_setPinLow(TRIG_PORT, TRIG_PIN);
}


// Echo 측정 함수
float32 getHighTimeSecond(IfxGtm_Tim_In *driver)
{
    sint32 pulseTicks = IfxGtm_Tim_In_getPulseLengthTick(driver);
    float32 freq = driver->captureClockFrequency;
    return pulseTicks / freq;
}

// 실제 초음파 거리 측정 함수 -----> 주기 60ms
int measure_ultrasonic_distance(void)
{
    trigger_ultrasonic_pulse();
    IfxGtm_Tim_In_update(&g_driverTIM);

    float32 highTime = getHighTimeSecond(&g_driverTIM);
    g_distanceCm = (highTime * 34000.0f) / 2.0f;

    //print("거리: %d cm\n\r", (int)g_distanceCm);

    return (int)g_distanceCm;
}

// 주기용 us 함수
void delay_us(uint32 microseconds)
{
    Ifx_TickTime ticks = IfxStm_getTicksFromMicroseconds(DELAY_STM, microseconds);
    IfxStm_waitTicks(DELAY_STM, ticks);
}

// 주기용 ms 함수
void delay_ms(uint32 milliseconds)
{
    Ifx_TickTime ticks = IfxStm_getTicksFromMilliseconds(DELAY_STM, milliseconds);
    IfxStm_waitTicks(DELAY_STM, ticks);
}
