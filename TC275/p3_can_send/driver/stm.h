// stm.h
 #ifndef STM_H
 #define STM_H

#include "Ifx_Types.h"
#include "asclin.h"
#include "IfxStm.h"
 
typedef struct
{
    Ifx_STM             *stmSfr;            /**< \brief Pointer to Stm register base */
    IfxStm_CompareConfig stmConfig;         /**< \brief Stm Configuration structure */
    volatile uint32      counter;           /**< \brief interrupt counter */
} App_Stm;


extern App_Stm g_Stm;

void Driver_Stm_Init(void);


void waitwaitns(uint32 ns) __attribute__((section(".otaSection0")));       // 현재 사용 섹션



 
 #endif
