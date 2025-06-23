// stm.h
 #ifndef STM_H
 #define STM_H

#include "Ifx_Types.h"
#include "asclin.h"
#include "IfxStm.h"
#include "main.h"
 
#define LEDL                        &MODULE_P02,6               //d8
#define LEDR                        &MODULE_P02,7               //d9


typedef struct
{
    Ifx_STM             *stmSfr;            /**< \brief Pointer to Stm register base */
    IfxStm_CompareConfig stmConfig;         /**< \brief Stm Configuration structure */
    volatile uint32      counter;           /**< \brief interrupt counter */
} App_Stm;


// extern App_Stm g_Stm ;

void Driver_Stm_Init(void);


void allclear(void);
void allset(void);


void led8Ctl(void);






 
 #endif
