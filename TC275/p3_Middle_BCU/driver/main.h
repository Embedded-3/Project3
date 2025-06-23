// main.h

#ifndef MAIN_H
#define MAIN_H

#include "IfxMultican_Can.h"

typedef enum {
    OPERATING = 0,
    UPDATING = 1,
} Core_Mode_t;

typedef struct {
    uint8 onoff;       // on/off 상태
    uint8 posl;       // 좌측 LED 상태
    uint8 posr;       // 우측 LED 상태
} Hbeam_t;






extern IfxMultican_Can        g_can;         // CAN 모듈 핸들
extern IfxMultican_Can_Node   g_canNode;     // CAN 노드 핸들
extern IfxMultican_Can_MsgObj g_txMsgObj;    // 송신 메시지 오브젝트
extern IfxMultican_Can_MsgObj g_rxMsgObj[];    // 수신 메시지 오브젝트
/* 송신 메시지 */
extern IfxMultican_Message g_txMsg;
/* 수신 메시지 */
extern IfxMultican_Message g_rxMsg;
// CAN bus 상태
extern IfxMultican_Status g_status;



extern Core_Mode_t core_mode;
extern Hbeam_t g_hbeam;




#endif
