// can.c
#include "can.h"
#include "stm.h"
#include "Platform_Types.h"
#include "start.h"


volatile uint32 meta_data[2] = {0}; // 8byte
volatile uint32 firmware_data[256][8];  // 8KB  [i] : page, [j] : idx in a page


static volatile uint8 hbeamOnOff;
static volatile uint8 posr;
static volatile uint8 posl;
// volatile uint32 firmware_data[1][8]={{0xFF004091,0x0B00FFD9,0x1F8601DA,0x9000F16C,0x11111111,0,0,0}};
// volatile int fwPage = 1;


// volatile uint32 firmware_data[4][8]=
// {
//         {0x00DA0820,0x2D3CAF74,0xFF004091,0x0B00FFD9,0x1F8601DA,0x00DAF16C,0x043C0178,0x1FC20158},
//         {0x01580178,0x04B4003B,0x0004C09B,0x7FF80F3F,0xFF004091,0x0B00FFD9,0xF000107B,0xF16C1F86},
//         {0x017800DA,0x0158043C,0x01781FC2,0x003B0158,0xC09B04B4,0x0F3F0004,0xAF547FF8,0xAF741FC2},
//         {0x5FBFAF54,0x90007FD3,0,0,0,0,0,0},
// };
volatile int fwPage = 0; //4;
volatile int idx = 0;

// 8byte : can packet

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
  nodeConfig.baudrate = 250000;                   // 250kbps 설정
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
            print("[CAN 수신] -> ID: 0x%02X  ---  \n\r", g_rxMsg.id);
            switch(g_rxMsg.id) {
                case 0x154: // 주행중 adb
                    hbeamOnOff = (g_rxMsg.data[0] >> 24) & 0xFF;
                    posr = (g_rxMsg.data[0] >> 16) & 0xFF;
                    posl = (g_rxMsg.data[0] >> 8) & 0xFF;
                    adb(posr, posl);
                    break;
                
                // ota
                case 0x10:  // metadata
                    meta_data[0] = g_rxMsg.data[0];
                    meta_data[1] = g_rxMsg.data[1];

                    fwPage = 0;
                    idx = 0;

                    core_mode = UPDATING;
                    print("start!\n\r");
                    print("data[0] : 0x%X\n\r", g_rxMsg.data[0]);
                    print("data[1] : 0x%X\n\r", g_rxMsg.data[1]);



                    // g_txMsg.id = 0x100;
                    // g_txMsg.lengthCode = 8;
                    // g_txMsg.data[0] = (uint32)(PFLASH_STARTING_ADDRESS);
                    // g_txMsg.data[1] = (uint32)(PFLASH_STARTING_ADDRESS);
                    // g_status = sendCanMessage();  // 메시지 전송



                    break;

                case 0x11:  // firmware data
                    if(fwPage < 256) {
                        g_rxMsg.data[0] = swapEndian(g_rxMsg.data[0]);
                        g_rxMsg.data[1] = swapEndian(g_rxMsg.data[1]);

                        print("Page %d | idx %d\n\r", fwPage, idx);
                        print("data[0] : 0x%X\n\r", g_rxMsg.data[0]);
                        print("data[1] : 0x%X\n\r", g_rxMsg.data[1]);

                        firmware_data[fwPage][idx++] = g_rxMsg.data[0];
                        firmware_data[fwPage][idx++] = g_rxMsg.data[1];

                        if(idx >= 8) {
                            idx = 0;
                            fwPage += 1;
                        }
                    }
                    break;

                case 0x12:

                    if(idx != 0) {  // 한 페이지를 다 못채움
                        while(idx < 8) {
                            firmware_data[fwPage][idx++] = 0;   // padding
                        }
                        if(idx >= 8 ){
                            fwPage += 1;
                            idx = 0;
                        }                        
                    }
                    writeProgramFlash();
                    
                    //verify

                    print("write Success\n\r");
                    core_mode = OPERATING;
                    break;  // 큰 switch 문 종료
                default:
                    break;
            }

        }

        // 수신 Pending Clear
        IfxMultican_MsgObj_clearRxPending(hwObj);
    }
}

