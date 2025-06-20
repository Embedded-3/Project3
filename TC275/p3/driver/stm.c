// stm.c
#include "stm.h"

#define WHITE   0x40,0x40,0x40 //0x18,0x18,0x18 //0x40,0x40,0x40
#define OFF     0x00,0x00,0x00


void ws2812_send_color(Ifx_P *port, uint8 pinIndex, uint8 g, uint8 r, uint8 b);
void ws2812_send_byte(Ifx_P *port, uint8 pinIndex, uint8 data);




// void ws2812_send_color(Ifx_P *port, uint8 pinIndex, uint8 g, uint8 r, uint8 b) __attribute__((section(".otaSection0")));
// void ws2812_send_byte(Ifx_P *port, uint8 pinIndex, uint8 data) __attribute__((section(".otaSection0")));
// void allclear() __attribute__((section(".otaSection0")));


volatile int* ptrr;
App_Stm g_Stm __attribute__((section(".shared")));
void Driver_Stm_Init(void)
{
    ptrr = &g_Stm;
    /* disable interrupts */
    boolean interruptState = IfxCpu_disableInterrupts();

    IfxStm_enableOcdsSuspend(&MODULE_STM0);

    g_Stm.stmSfr = &MODULE_STM0;
    IfxStm_initCompareConfig(&g_Stm.stmConfig);

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);
}




void waitns(uint32 ns)
{
    IfxStm_waitTicks(g_Stm.stmSfr, ns);
}

void allset()
{
    // 전체 클리어
    for(int i = 0; i < 8;i++){
        ws2812_send_color(LEDR, WHITE);
    }
    for(int i = 0; i < 8;i++){
        ws2812_send_color(LEDL, WHITE);  
    }
}

void allclear()
{
    // 전체 클리어
    for(int i = 0; i < 8;i++){
        ws2812_send_color(LEDR, OFF);
    }
    for(int i = 0; i < 8;i++){
        ws2812_send_color(LEDL, OFF);  
    }
}

void adb(uint8 posr, uint8 posl)
{
    allclear();

    for (int i = 0; i < 8; i++)
    {
        if (posr & (1 << (7 - i)))  // MSB부터 확인
        {
            ws2812_send_color(LEDR, WHITE);  // on
        }
        else
        {
            ws2812_send_color(LEDR, OFF);  // off
        }
    }

    for (int i = 0; i < 8; i++)
    {
        if (posl & (1 << (7 - i)))  // MSB부터 확인
        {
            ws2812_send_color(LEDL, WHITE);  // on
        }
        else
        {
            ws2812_send_color(LEDL, OFF);  // off
        }
    }
}



void ws2812_send_byte(Ifx_P *port, uint8 pinIndex, uint8 data)
{

    for (int i = 7; i >= 0; i--) {
        if (data & (1 << i)) {
            // 1비트 전송: High 0.8us + Low 0.45us
            IfxPort_setPinHigh(port, pinIndex);
            //waitns(70);
            waitns(70);
            IfxPort_setPinLow(port, pinIndex);
            //waitns(12);
            waitns(5);
        } else {
            // 0비트 전송: High 0.4us + Low 0.85us
            IfxPort_setPinHigh(port, pinIndex);
            //waitns(12);
            waitns(5);
            IfxPort_setPinLow(port, pinIndex);
            //waitns(70);
            waitns(70);
        }
    }
    // 18 : 500
    // 80 : 1us

    //try1 : 70, 12
    //try2 : 70, 5

}

void ws2812_send_color(Ifx_P *port, uint8 pinIndex, uint8 g, uint8 r, uint8 b)
{
    IfxPort_setPinLow(port, pinIndex);
    //waitns(3000);
    //waitns(4500);
    waitns(4300);
    //waitns(1700);
    //waitns(1000);

    ws2812_send_byte(port, pinIndex, g);
    ws2812_send_byte(port, pinIndex, r);
    ws2812_send_byte(port, pinIndex, b);

    
}








/*-------------------------------------------------------------------------------------------*/





// void waitns(uint32 ns)__attribute__((section(".otaSection0")))
// {
//     IfxStm_waitTicks(g_Stm.stmSfr, ns);
// }

// void allclear()__attribute__((section(".otaSection0")))
// {
//     // 전체 클리어
//     for(int i = 0; i < 8;i++){
//         ws2812_send_color(LEDR, OFF);
//     }
//     for(int i = 0; i < 8;i++){
//         ws2812_send_color(LEDL, OFF);  
//     }
// }


// void ws2812_send_byte(Ifx_P *port, uint8 pinIndex, uint8 data) __attribute__((section(".otaSection0")))
// {

//     for (int i = 7; i >= 0; i--) {
//         if (data & (1 << i)) {
//             // 1비트 전송: High 0.8us + Low 0.45us
//             IfxPort_setPinHigh(port, pinIndex);
//             //waitns(70);
//             waitns(70);
//             IfxPort_setPinLow(port, pinIndex);
//             //waitns(12);
//             waitns(5);
//         } else {
//             // 0비트 전송: High 0.4us + Low 0.85us
//             IfxPort_setPinHigh(port, pinIndex);
//             //waitns(12);
//             waitns(5);
//             IfxPort_setPinLow(port, pinIndex);
//             //waitns(70);
//             waitns(70);
//         }
//     }
//     // 18 : 500
//     // 80 : 1us

//     //try1 : 70, 12
//     //try2 : 70, 5

// }

// void ws2812_send_color(Ifx_P *port, uint8 pinIndex, uint8 g, uint8 r, uint8 b) __attribute__((section(".otaSection0")))
// {
//     IfxPort_setPinLow(port, pinIndex);
//     //waitns(3000);
//     //waitns(4500);
//     waitns(4300);
//     //waitns(1700);
//     //waitns(1000);

//     ws2812_send_byte(port, pinIndex, g);
//     ws2812_send_byte(port, pinIndex, r);
//     ws2812_send_byte(port, pinIndex, b);

    
// }


// void func() __attribute__((section(".myfunc")))       // 현재 사용 섹션
// {
//     volatile int i = 0;
//     volatile int j = 0;

//     IfxPort_setPinLow(LEDL);
//     IfxPort_setPinLow(LEDR);
//     waitns(4300);

//     allclear();

//     int num = 2;
//     while(num--) {
//         for (i = 0; i < 8; i++)
//         {
//             for (j = 0; j < 8; j++) // 한 세트 8개
//             {
//                 if (i <= j) {
//                     ws2812_send_color(LEDR, OFF);  // off
//                 }
//                 else {
//                     ws2812_send_color(LEDR, WHITE);  // on
//                 }
//             }

//             for (j = 0; j < 8; j++) // 한 세트 8개
//             {
//                 if (i <= j) {
//                     ws2812_send_color(LEDL, OFF);  // off
//                 }
//                 else {
//                     ws2812_send_color(LEDL, WHITE);  // on
//                 }
//             }
//             for(volatile int t=0;t<1500000;t++);
//         }
//         waitns(4300);

//         for (i = 0; i < 8; i++)
//         {
//             for (j = 0; j < 8; j++) // 한 세트 8개
//             {
//                 if (i >= j) {
//                     ws2812_send_color(LEDR, OFF);  // off
//                 }
//                 else {
//                     ws2812_send_color(LEDR, WHITE);  // on
//                 }
//             }

//             for (j = 0; j < 8; j++) // 한 세트 8개
//             {
//                 if (i >= j) {
//                     ws2812_send_color(LEDL, OFF);  // off
//                 }
//                 else {
//                     ws2812_send_color(LEDL, WHITE);  // on
//                 }
//             }
//             for(volatile int t=0;t<1500000;t++);
//         }
//         waitns(4300);
//     }
// }








