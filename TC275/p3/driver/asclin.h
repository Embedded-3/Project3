// asclin.h
#ifndef ASCLIN_H
#define ASCLIN_H

#include "IfxStdIf_DPipe.h"

#define ENDLINE     "\n\r"

#define print(...) IfxStdIf_DPipe_print(stdif, __VA_ARGS__)


#define ISR_PRIORITY_ASCLIN_TX      8                                       /* Priority for interrupt ISR Transmit  */
#define ISR_PRIORITY_ASCLIN_RX      4                                       /* Priority for interrupt ISR Receive   */
#define ISR_PRIORITY_ASCLIN_ER      12                                      /* Priority for interrupt ISR Errors    */
#define ASC_TX_BUFFER_SIZE          256                                     /* Define the TX buffer size in byte    */
#define ASC_RX_BUFFER_SIZE          256                                     /* Define the RX buffer size in byte    */
#define ASC_BAUDRATE                115200                                  /* Define the UART baud rate            */

extern IfxStdIf_DPipe* stdif;                                     /* Standard interface object            */

//extern IfxStdIf_DPipe g_ascStandardInterface;

void initShellInterface(void);
void initSerialInterface(void);
void printInfo(IfxStdIf_DPipe*);

#endif // ASCLIN_H
