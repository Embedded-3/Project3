#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "driver/asclin.h"
#include "driver/stm.h"
#include "driver/can.h"
#include "driver/steering.h"

void AppScheduling(void);
void AppTask1ms(void);
void AppTask10ms(void);
void AppTask100ms(void);
void AppTask1000ms(void);



IfxCpu_syncEvent g_cpuSyncEvent = 0;
TestCnt stTestCnt;
IfxMultican_Can        g_can;         // CAN 모듈 핸들
IfxMultican_Can_Node   g_canNode;     // CAN 노드 핸들
IfxMultican_Can_MsgObj g_txMsgObj;    // 송신 메시지 오브젝트
IfxMultican_Can_MsgObj g_rxMsgObj;    // 수신 메시지 오브젝트
/* 송신 메시지 */
IfxMultican_Message g_txMsg;
/* 수신 메시지 */
IfxMultican_Message g_rxMsg;
// CAN bus 상태
IfxMultican_Status g_status;

void core0_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    //init
    initShellInterface();
    Driver_Stm_Init();
    initCan();
    initsteeringPWM();
//    mapSteeringAngleToPwm(0);
    while(1)
    {
        AppScheduling();
    }
}

void AppTask1ms(void)
{
    stTestCnt.u32nuCnt1ms++;
}

void AppTask10ms(void)
{
    stTestCnt.u32nuCnt10ms++;
    if(stTestCnt.u32nuCnt10ms %2 ==0){
        mapSteeringAngleToPwm(angle);
        g_txMsg.id = 0x302;
        g_txMsg.lengthCode = 8;
        g_txMsg.data[0] = (uint32)(26);
        g_txMsg.data[1] = (uint32)(2);
        g_status = sendCanMessage();  // 메시지 전송
//        print("angle : %d\n\r", angle);
    }
}

void AppTask100ms(void)
{
    stTestCnt.u32nuCnt100ms++;
}

void AppTask1000ms(void)
{
    stTestCnt.u32nuCnt1000ms++;
}

void AppScheduling(void)
{
    if(stSchedulingInfo.u8nuScheduling1msFlag == 1u)
    {
        stSchedulingInfo.u8nuScheduling1msFlag = 0u;
        AppTask1ms();

        if(stSchedulingInfo.u8nuScheduling10msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling10msFlag = 0u;
            AppTask10ms();
        }

        if(stSchedulingInfo.u8nuScheduling100msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling100msFlag = 0u;
            AppTask100ms();
        }
        if(stSchedulingInfo.u8nuScheduling1000msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling1000msFlag = 0u;
            AppTask1000ms();
        }
    }
}
