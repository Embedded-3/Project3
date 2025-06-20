// can.h

#ifndef CAN_H_
#define CAN_H_

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxStm.h"
#include "IfxMultican_Can.h"
#include "asclin.h"
#include "Flash_Programming.h"
#include "main.h"

#define ISR_PRIORITY_CAN_RX   3 // 10


/* 글로벌 핸들 */
extern IfxMultican_Can g_can;             // CAN 모듈 핸들
extern IfxMultican_Can_Node g_canNode;    // CAN 노드 핸들
extern IfxMultican_Can_MsgObj g_txMsgObj; // 송신 메시지 오브젝트
extern IfxMultican_Can_MsgObj g_rxMsgObj; // 수신 메시지 오브젝트

/* 송신 메시지 */
extern IfxMultican_Message g_txMsg;
/* 수신 메시지 */
extern IfxMultican_Message g_rxMsg;


extern volatile uint32 meta_data[2]; // 8byte
extern volatile uint32 firmware_data[256][8];  // 8KB  [i] : page, [j] : idx in a page
extern volatile int fwPage;



void initCan(void);
IfxMultican_Status sendCanMessage(void);
void canReceiveLoop();


#endif /* CAN_H_ */
