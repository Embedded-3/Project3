// start.c

#include "start.h"
#include "IfxPort.h"

#define PFLASH_BASE_ADDRESS         (0x80100000)
#define PFLASH_PARTITION0           (PFLASH_BASE_ADDRESS + 0x00000)
#define PFLASH_PARTITION1           (PFLASH_BASE_ADDRESS + 0x80000)

uint32 PFLASH_STARTING_ADDRESS =    PFLASH_BASE_ADDRESS;

FuncPtr g_funcptr;
volatile uint32 boot_flag __attribute__((section(".otaDataSection")));       // 현재 사용 섹션
volatile uint32* ptr;

uint32 DATA_TO_WRITE = 0x00000000;

#define LED1                        &MODULE_P10,1               //LED connected to Port 00, Pin 5
#define LED2                        &MODULE_P10,2               //LED connected to Port 00, Pin 6

// void ota0() __attribute__((section(".otaSection0")))
// {
//     //IfxPort_setPinState(LED2, IfxPort_State_high);
// }

// void ota1() __attribute__((section(".otaSection1")))
// {
//     //IfxPort_setPinState(LED2, IfxPort_State_high);
// }

void software_init_hook() {
    ptr = &boot_flag;   // debug

    core_mode = OPERATING;

    switch(boot_flag) {
        case 0x01000000:
            g_funcptr.ota = (void (*)(void))PFLASH_PARTITION0;  // 실행할 함수 주소

            PFLASH_STARTING_ADDRESS = (uint32)(PFLASH_PARTITION1 & 0x0FFFFFFF | 0xA0000000);    // ota 위치
            DATA_TO_WRITE = 0x10000000;
            break;

        case 0x10000000:
            g_funcptr.ota = (void (*)(void))PFLASH_PARTITION1;  // 실행할 함수 주소

            PFLASH_STARTING_ADDRESS = (uint32)(PFLASH_PARTITION0 & 0x0FFFFFFF | 0xA0000000);    // ota 위치
            DATA_TO_WRITE = 0x01000000;
            break;

        default:    // 기본 설정은 실행 주소 : partition0, 업데이트 받을 주소 : partition1
            g_funcptr.ota = (void (*)(void))PFLASH_PARTITION0;  // 실행할 함수 주소

            PFLASH_STARTING_ADDRESS = (uint32)(PFLASH_PARTITION1 & 0x0FFFFFFF | 0xA0000000);    // ota 위치
            DATA_TO_WRITE = 0x10000000;
            break;
    }
}


/*
            // g_funcptr.ota = (void *)ota1;
            // g_funcptr.ota = (void *)((uint32)(g_funcptr.ota) & 0x0FFFFFFF | 0xA0000000);
*/
