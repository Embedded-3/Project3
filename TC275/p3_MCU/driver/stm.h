// stm.h

#ifndef STM_H
#define STM_H

/***********************************************************************/
/*Include*/
/***********************************************************************/
#include "Ifx_Types.h"
#include "IfxStm.h"
//#include "IfxCpu_Irq.h"
#include "Libraries\iLLD\TC27D\Tricore\Cpu\Irq\IfxCpu_Irq.h"

#include "main.h"


/***********************************************************************/
/*Typedef*/
/***********************************************************************/

typedef struct
{
    uint8 u8nuScheduling1msFlag;
    uint8 u8nuScheduling10msFlag;
    uint8 u8nuScheduling100msFlag;
    uint8 u8nuScheduling1000msFlag;
}SchedulingFlag;

// main에서 쓸거
typedef struct
{
    uint32 u32nuCnt1ms;
    uint32 u32nuCnt10ms;
    uint32 u32nuCnt100ms;
    uint32 u32nuCnt1000ms;

}TestCnt;
/***********************************************************************/
/*Define*/
/***********************************************************************/

/***********************************************************************/
/*External Variable*/
/***********************************************************************/
extern SchedulingFlag stSchedulingInfo;


extern TestCnt stTestCnt;   // main에서 사용할 전역변수
/***********************************************************************/
/*Global Function Prototype*/
/***********************************************************************/
void Driver_Stm_Init(void);
void AppScheduling(void);

#endif // STM_H