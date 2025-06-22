// can.c
#include "can.h"



static uint32 swapEndian(uint32 value) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8)  |
           ((value & 0x00FF0000) >> 8)  |
           ((value & 0xFF000000) >> 24);
}

IFX_INTERRUPT(canRxInterruptHandler, 0, ISR_PRIORITY_CAN_RX);
void canRxInterruptHandler(void)
{
    canReceiveLoop();
    IfxCpu_enableInterrupts();
}

/* 초기화 함수 */
void initCan(void)
{
  /* 1. CAN 모듈 초기화 */
  IfxMultican_Can_Config canConfig;
  IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);
  IfxMultican_Can_initModule(&g_can, &canConfig);

  /* 2. CAN 노드 초기화 */
  IfxMultican_Can_NodeConfig nodeConfig;
  IfxMultican_Can_Node_initConfig(&nodeConfig, &g_can);

  nodeConfig.nodeId = IfxMultican_NodeId_0;       // Node0 사용
  nodeConfig.baudrate = 500000; //250000;                   // 250kbps 설정
  nodeConfig.samplePoint = 8000;                  // 샘플 포인트 80%
  nodeConfig.rxPin = &IfxMultican_RXD0B_P20_7_IN; // RX 핀 (P20.7)
  nodeConfig.txPin = &IfxMultican_TXD0_P20_8_OUT; // TX 핀 (P20.8)
  nodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
  nodeConfig.txPinMode = IfxPort_OutputMode_pushPull;
  nodeConfig.pinDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1; // 패드 드라이버 설정

  IfxMultican_Can_Node_init(&g_canNode, &nodeConfig);

  /* 3. 송신 메시지 오브젝트 초기화 */
  IfxMultican_Can_MsgObjConfig txMsgConfig;
  IfxMultican_Can_MsgObj_initConfig(&txMsgConfig, &g_canNode);

  txMsgConfig.msgObjId = 0;      // 오브젝트 ID 0
  txMsgConfig.messageId = 0x100; // 송신 ID: 0x100
  txMsgConfig.frame = IfxMultican_Frame_transmit;
  txMsgConfig.control.messageLen = IfxMultican_DataLengthCode_8;

  IfxMultican_Can_MsgObj_init(&g_txMsgObj, &txMsgConfig);

  /* 4. 수신 메시지 오브젝트 초기화 */
  IfxMultican_Can_MsgObjConfig rxMsgConfig;
  IfxMultican_Can_MsgObj_initConfig(&rxMsgConfig, &g_canNode);

  rxMsgConfig.msgObjId = 1;      // 오브젝트 ID 1
  rxMsgConfig.messageId = 0; // 수신 ID는 의미 없음
  rxMsgConfig.acceptanceMask = 0; // 0이면 '모든 ID' 허용
  rxMsgConfig.frame = IfxMultican_Frame_receive;
  rxMsgConfig.control.messageLen = IfxMultican_DataLengthCode_8;

  /* 5. 수신 인터럽트 활성화 */
  rxMsgConfig.rxInterrupt.enabled = TRUE;
  rxMsgConfig.rxInterrupt.srcId = IfxMultican_SrcId_0;


  IfxMultican_Can_MsgObj_init(&g_rxMsgObj, &rxMsgConfig);

  /* 6. 송신 메시지 데이터 준비 */
  IfxMultican_Message_init(&g_txMsg, 0x00a, 0x11223344, 0x55667788, 8); // ID 0x100, 데이터 8 bytes

  /* 6. 인터럽트 서비스 등록 */
  IfxSrc_init(IfxMultican_getSrcPointer(&MODULE_CAN, IfxMultican_SrcId_0), IfxSrc_Tos_cpu0, ISR_PRIORITY_CAN_RX);
  IfxSrc_enable(IfxMultican_getSrcPointer(&MODULE_CAN, IfxMultican_SrcId_0));
}

/* 송신 함수 */
IfxMultican_Status sendCanMessage(void)
{
    IfxMultican_Status status = IfxMultican_Can_MsgObj_sendMessage(&g_txMsgObj, &g_txMsg);
    return status;
}

/* 수신 대기 및 출력 함수 */
void canReceiveLoop()
{
    Ifx_CAN_MO *hwObj = IfxMultican_MsgObj_getPointer(g_rxMsgObj.node->mcan, g_rxMsgObj.msgObjId);

    // 메시지가 수신되었는지 확인
    if (IfxMultican_MsgObj_isRxPending(hwObj))
    {
        // 수신 메시지 읽기
        IfxMultican_Message_init(&g_rxMsg, 0, 0, 0, 8); // ID, data 초기화
        IfxMultican_Status status = IfxMultican_MsgObj_readMessage(hwObj, &g_rxMsg);

        // 수신 성공
        if (status & IfxMultican_Status_newData)
        {
            print("[CAN 수신] -> ID: 0x%X  ---  \n\r", g_rxMsg.id);
            print("raw data : [0] : 0x%X | [1]: 0x%X\n\r", g_rxMsg.data[0], g_rxMsg.data[1]);

            switch(g_rxMsg.id) {
                // case 0x555:
                //     print("[CAN 수신] -> ID: 0x%X  ---  \n\r", g_rxMsg.id);
                //     print("raw data : [0] : 0x%X | [1]: 0x%X\n\r", g_rxMsg.data[0], g_rxMsg.data[1]);
                //     break;

                /*----------------------- 모터 제어 수신 -------------------------*/
                case 0x11: // 모터 제어
                // 0부터 받나? 1이 나중?
                    print("raw data : [0] : 0x%X | [1]: 0x%X\n\r", g_rxMsg.data[0], g_rxMsg.data[1]);

                    // 모터 pwm
                    g_motor.pwm = (g_rxMsg.data[0] >> 0) & 0xFFFF; // 16 비트

                    // 모터 방향 by 기어
                    switch((g_rxMsg.data[0] >> 16) & 0x3) { // 3bit
                        case 0x0: case 0x2: case 0x3: // 전진 : P, N, D
                            g_motor.dir = GO_FRONT;
                            break;
                        case 0x1:                     // 후진 : R
                            g_motor.dir = GO_BACK;
                            break;
                        default:
                            g_motor.dir = GO_FRONT; // 기본값
                            print("Unknown direction: %d\n\r", (g_rxMsg.data[0] >> 16) & 0xFFFF);
                            break;
                    }
                    break;
            }
        }

        // 수신 Pending Clear
        IfxMultican_MsgObj_clearRxPending(hwObj);
    }
}

