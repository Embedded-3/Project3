// can.c
#include "can.h"


volatile uint32 ota_write_flag = 0;
volatile uint32 meta_data[2] = {0}; // 8byte
volatile uint32 firmware_data[256][8];  // 8KB  [i] : page, [j] : idx in a page
volatile uint32 total_pages;
volatile uint32 repeat;
volatile uint32 turn=0;
volatile int fwPage = 0; //4;
volatile int idx = 0;
// 8byte : can packet

volatile uint8 hbeamOnOff;
volatile uint8 posr;
volatile uint8 posl;

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
  nodeConfig.baudrate = 500000;  //250000;                   // 250kbps 설정
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
                /*--------------------주행 관련 메시지 수신-----------------------*/
                case 0x71: // 기본 주행
                    hbeamOnOff = (g_rxMsg.data[0] >> 29) & 0x01;    // 29번째 비트 (앞에서 3번째비트)
                    if(hbeamOnOff) {
                        g_hbeam.onoff = 1;
                        g_hbeam.posl = 0xC0;
                        g_hbeam.posr = 0xC0;
                    }
                    else {
                        g_hbeam.onoff = 0;
                        g_hbeam.posl = 0x00;
                        g_hbeam.posr = 0x00;
                    }
                    break;

                /*--------------------adb 관련 메시지 수신-----------------------*/
                case 0x23: // adb
                    print("data[0] : 0x%X\n\r", g_rxMsg.data[0]);
                    print("data[1] : 0x%X\n\r", g_rxMsg.data[1]);

                    g_hbeam.posl = (g_rxMsg.data[0] >> 0) & 0xFF;
                    g_hbeam.posr = (g_rxMsg.data[0] >> 8) & 0xFF;
                    if(!g_hbeam.posl && !g_hbeam.posr) g_hbeam.onoff = 0;   // hbeam off
                    else g_hbeam.onoff = 1;                                 // hbeam on

                    print("left : 0x%X\n\r", g_hbeam.posl);
                    print("right : 0x%X\n\r", g_hbeam.posr);
                    break;

                /* ------------------ ota관련 메시지 수신------------------------*/
                case 0x10:  // metadata
                    print("0x%X\n\r", g_rxMsg.data[0]);
                    print("0x%X\n\r", g_rxMsg.data[1]);
                    meta_data[0] = g_rxMsg.data[0];
                    meta_data[1] = g_rxMsg.data[1];
                    repeat = meta_data[0];
                    total_pages = meta_data[0]/8;
                    if(meta_data[0]%8>0) total_pages++;
                    fwPage = 0;
                    idx = 0;

                    core_mode = UPDATING;
                    print("start!\n\r");
                    print("0x%d\n\r", total_pages);
                    print("0x%d\n\r", repeat);
                    break;

                case 0x11:  // firmware data
                    if(fwPage <= total_pages) {

                        g_rxMsg.data[0] = swapEndian(g_rxMsg.data[0]);
                        g_rxMsg.data[1] = swapEndian(g_rxMsg.data[1]);

                        print("Page %d | idx %d\n\r", fwPage, idx);
                        print("data[0] : 0x%X\n\r", g_rxMsg.data[0]);
                        print("data[1] : 0x%X\n\r", g_rxMsg.data[1]);

                        firmware_data[fwPage][idx++] = g_rxMsg.data[0];
                        firmware_data[fwPage][idx++] = g_rxMsg.data[1];
                        turn += 2;
                        if(idx %8 == 0) {
                            idx = 0;
                            fwPage += 1;
                        }
                        print("turn : %d\n\r", turn);
                        print("fwPage : %d\n\r", fwPage);
                        print("total_pages : %d\n\r", total_pages);
                    }
                    
                    if(turn == repeat){
                        if(idx != 0) {  // 한 페이지를 다 못채움
                            // while(idx < 8) {
                            //     firmware_data[fwPage][idx++] = 0;   // padding
                            // }
                            // if(idx >= 8 ){
                            //     fwPage += 1;
                            //     idx = 0;
                            // }
                        }
                        ota_write_flag = 1;

                    }
                    break;
                default:
                    break;
            }

        }

        // 수신 Pending Clear
        IfxMultican_MsgObj_clearRxPending(hwObj);
    }
}

