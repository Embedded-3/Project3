// encoder.c
#include "encoder.h"


volatile bool LEFT_Direction = TRUE;
volatile int left_duration = 0;
volatile uint8 left_encoderPinALast = 0;

volatile bool RIGHT_Direction = TRUE;
volatile int right_duration = 0;
volatile uint8 right_encoderPinALast = 0;




IFX_INTERRUPT(left_encoderInterruptHandler, 0, ISR_PRIORITY_LEFT_ENCODER);
void left_encoderInterruptHandler(void)
{
    uint8 currentA = IfxPort_getPinState(LEFT_ENCODER_PIN_A);
    
    // rising edge (LOW -> HIGH)
    if (left_encoderPinALast == 0 && currentA == 1)
    {
        uint8 valB = IfxPort_getPinState(LEFT_ENCODER_PIN_B);
        if (valB == 0 && LEFT_Direction)
        {
            LEFT_Direction = FALSE;     // 앞으로
        }
        else if (valB == 1 && !LEFT_Direction)
        {
            LEFT_Direction = TRUE;      // 뒤로
        }
    }

    left_encoderPinALast = currentA;

    if (!LEFT_Direction)
        left_duration++;
    else
        left_duration--;
}

IFX_INTERRUPT(right_encoderInterruptHandler, 0, ISR_PRIORITY_RIGHT_ENCODER);
void right_encoderInterruptHandler(void)
{
    uint8 currentA = IfxPort_getPinState(RIGHT_ENCODER_PIN_A);
    
    // rising edge (LOW -> HIGH)
    if (right_encoderPinALast == 0 && currentA == 1)
    {
        uint8 valB = IfxPort_getPinState(RIGHT_ENCODER_PIN_B);
        if (valB == 0 && RIGHT_Direction)
        {
            RIGHT_Direction = FALSE;    // 뒤로
        }
        else if (valB == 1 && !RIGHT_Direction)
        {
            RIGHT_Direction = TRUE;     // 앞으로
        }
    }

    right_encoderPinALast = currentA;

    if (RIGHT_Direction)      // 왼쪽과 반대임
        right_duration++;
    else
        right_duration--;
}


void initEncoder(void)
{
    
    //인터럽트 관련
#define EXIS0_IDX 4
#define FEN0_IDX 8
#define REN0_IDX 9
#define EIEN0_IDX 11
#define INP0_IDX 12
#define IGP0_IDX 14

#define SRE_IDX 10
#define TOS_IDX 11

    // 1. 입력 핀 설정
    IfxPort_setPinModeInput(LEFT_ENCODER_PIN_A, IfxPort_InputMode_pullUp);
    IfxPort_setPinModeInput(LEFT_ENCODER_PIN_B, IfxPort_InputMode_pullUp);
    left_encoderPinALast = IfxPort_getPinState(LEFT_ENCODER_PIN_A);

    IfxPort_setPinModeInput(RIGHT_ENCODER_PIN_A, IfxPort_InputMode_pullUp);
    IfxPort_setPinModeInput(RIGHT_ENCODER_PIN_B, IfxPort_InputMode_pullUp);
    right_encoderPinALast = IfxPort_getPinState(RIGHT_ENCODER_PIN_A);

#define EXIS1_IDX 20
#define REN1_IDX 25
#define FEN1_IDX 24
#define EIEN1_IDX 27
#define INP1_IDX 28

#define IGP1_IDX 30
    //EICR
    SCU_EICR1.U &= ~(0x7 << EXIS0_IDX | 0x7 << EXIS1_IDX); // 초기화
    //SCU_EICR1.U |= 0x1 << EXIS0_IDX;  // 핀설정

    SCU_EICR1.U |= 1<< REN0_IDX | 1 << REN1_IDX;    // rising edge
    SCU_EICR1.U |= 1<< FEN0_IDX | 1<< FEN1_IDX;    // falling edge
    SCU_EICR1.U |= 1<< EIEN0_IDX | 1<< EIEN1_IDX;

    SCU_EICR1.U &= ~(0x7 << INP0_IDX | 0x7 << INP1_IDX);
    SCU_EICR1.U |= 0x1 << INP1_IDX; //OGU1

    //IGCR
    SCU_IGCR0.U &= ~(0x3 << IGP0_IDX | 0x3 << IGP1_IDX); // 초기화
    SCU_IGCR0.U |= 0x1<<IGP0_IDX | 0x1<<IGP1_IDX;

    //SCU_ERU
    SRC_SCU_SCU_ERU0.U &= ~0xFF;    // 초기화
    SRC_SCU_SCU_ERU0.U |= ISR_PRIORITY_LEFT_ENCODER; // 우선순위
    SRC_SCU_SCU_ERU0.U |= 1<< SRE_IDX;
    SRC_SCU_SCU_ERU0.U &= ~(0x3 << TOS_IDX);

    SRC_SCU_SCU_ERU1.U &= ~0xFF;
    SRC_SCU_SCU_ERU1.U |= ISR_PRIORITY_RIGHT_ENCODER;
    SRC_SCU_SCU_ERU1.U |= 1<< SRE_IDX;
    SRC_SCU_SCU_ERU1.U &= ~(0x3 << TOS_IDX);
}
