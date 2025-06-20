// stm.h
 #ifndef STM_H
 #define STM_H

#include "Ifx_Types.h"
#include "asclin.h"
#include "IfxStm.h"
 
#define LEDL                        &MODULE_P02,6               //LED connected to Port 00, Pin 5
#define LEDR                        &MODULE_P02,7               //LED connected to Port 00, Pin 6


typedef struct
{
    Ifx_STM             *stmSfr;            /**< \brief Pointer to Stm register base */
    IfxStm_CompareConfig stmConfig;         /**< \brief Stm Configuration structure */
    volatile uint32      counter;           /**< \brief interrupt counter */
} App_Stm;


// extern App_Stm g_Stm ;

void Driver_Stm_Init(void);
void adb(uint8 posr, uint8 posl);

void allclear();
void allset();









//void func() __attribute__((section(".myfunc")));



// void a() __attribute__((section(".myfunc")));
//void waitns(uint32 ns) __attribute__((section(".otaSection0")));



 
 #endif
