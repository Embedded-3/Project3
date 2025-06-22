// User Motor Control
#include "user_MotorCtl.h"
#include "Ifx_Types.h"

#include "pwm.h"
#include "encoder.h"
#include "asclin.h"


volatile Speed_t measured_speed = {0, 0};

void getSpeed(int time) // int time : 측정시간 (ms)
{
    // 속도 (cm/s) = (duration × 바퀴 원주) / (PPR × 측정 시간(ms))
    measured_speed.lspeed = left_duration * DIAMETER * 3.1416 / (PPR * time / 1000);
    measured_speed.rspeed = right_duration * DIAMETER * 3.1416 / (PPR * time / 1000);

    //print("%d | %d\n\r", left_duration, right_duration);
    static int cnt = 0;
    // if (time == 100 && cnt++ % 10 == 0) {
    //     print(RRED"%.3lf [cm/s] | %.3lf [cm/s]\n\r"RESET, measured_speed.lspeed, measured_speed.rspeed);
    // }
    if (time == 10 && cnt++ % 100 == 0) {
        ;
        //print(RRED"%.3lf [cm/s] | %.3lf [cm/s]\n\r"RESET, measured_speed.lspeed, measured_speed.rspeed);
    }

    // duration 초기화
    right_duration = 0;
    left_duration = 0;
}


