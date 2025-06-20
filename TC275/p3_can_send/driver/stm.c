// stm.c
#include "stm.h"

App_Stm g_Stm;
void Driver_Stm_Init(void)
{
    /* disable interrupts */
    boolean interruptState = IfxCpu_disableInterrupts();

    IfxStm_enableOcdsSuspend(&MODULE_STM0);

    g_Stm.stmSfr = &MODULE_STM0;
    IfxStm_initCompareConfig(&g_Stm.stmConfig);

    // g_Stm.stmConfig.triggerPriority = 100u;
    // g_Stm.stmConfig.typeOfService   = IfxSrc_Tos_cpu0;
    // g_Stm.stmConfig.ticks           = 10u; //100000000u; //100000u; //100000000u

    // IfxStm_initCompare(g_Stm.stmSfr, &g_Stm.stmConfig);

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);
}

void waitwaitns(uint32 ns)__attribute__((section(".otaSection0")))       // 현재 사용 섹션
{
    IfxStm_waitTicks(g_Stm.stmSfr, ns);
}

// IFX_INTERRUPT(STM_Int0Handler, 0, 100);
// void STM_Int0Handler(void)
// {
//     IfxCpu_enableInterrupts();

//     IfxStm_clearCompareFlag(g_Stm.stmSfr, g_Stm.stmConfig.comparator);
//     IfxStm_increaseCompare(g_Stm.stmSfr, g_Stm.stmConfig.comparator, 10u);  //100000u
// }









