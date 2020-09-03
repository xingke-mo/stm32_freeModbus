#ifndef __NCN5120_H
#define __NCN5120_H

#include "stm32f1xx_hal.h"

#define MAX_NCN5120_BUF 100

#define NACKREPET 3

#define BUSYREPET 3

#define DUMMY  0xFF

#define LOBYTE(value)           ((unsigned char) ((value) & 0x0ff))
/// returns high byte of integer value
#define HIBYTE(value)           ((unsigned char) ((value) >> 8))
/**
NCN5120
*/
#define Rest_cmd                        0x01
#define RdState_cmd                     0x02
#define SetBusy_cmd                     0x03
#define QuitBusy_cmd                    0x04
#define SetBusmon_cmd                   0x05
#define SetAddress_cmd                  0xF1
#define SetRepetition_cmd               0xF2
#define RdSystemState_cmd               0x0D
#define SetStopMode_cmd                 0x0E
#define ExitStopMode_cmd                0x0F
#define Ackn_cmd                        0x10
#define Configure_cmd                   0x18
#define IntRegWr_cmd                    0x28
#define IntRegRd_cmd                    0x38
#define PollingState_cmd                0xE0
#define Sendframe_cmd                   0x80



typedef enum _NCN5120_CONTROL_STATUS
{
    SEND_REST_CMD,
    SEND_RDSTATE_CMD,
    SEND_SETBUSY_CMD,
    SEND_QUITBUSY_CMD,
    SEND_SETBUSMON_CMD,
    SEND_SETADDRESS_CMD,
    SEND_SETREPETITION_CMD,
    SEND_RDSYSTEMSTATE_CMD,
    SEND_SETSTOPMODE_CMD,
    SEND_EXITSTOPMODE_CMD,
    SEND_ACKN_CMD,
    SEND_CONFIGURE_CMD,
    SEND_INTREGWR_CMD,
    SEND_INTREGRD_CMD,
    SEND_POLLINGSTATE_CM,
    SEND_SENDFRAME_CMD,


    WAIT_REST_CMD_RET,
    WAIT_RDSTATE_CMD_RET,
    WAIT_SETBUSY_CMD_RET,
    WAIT_QUITBUSY_CMD_RET,
    WAIT_SETBUSMON_CMD_RET,
    WAIT_SETADDRESS_CMD_RET,
    WAIT_SETREPETITION_CMD_RET,
    WAIT_RDSYSTEMSTATE_CMD_RET,
    WAIT_SETSTOPMODE_CMD_RET,
    WAIT_EXITSTOPMODE_CMD_RET,
    WAIT_ACKN_CMD_RET,
    WAIT_CONFIGURE_CMD_RET,
    WAIT_INTREGWR_CMD_RET,
    WAIT_INTREGRD_CMD_RET,
    WAIT_POLLINGSTATE_CMD_RET,
    WAIT_SENDFRAME_CMD_RET,
    STALLED,
    PAUSE,
    FREE
} NCN5120_CONTROL_STATUS;




typedef struct _FLAME_TIM_CONTROL
{
    uint16_t TimeoutCtr  ;
    uint8_t  TimeoutEn   ;
    uint16_t TimeoutCnts ;
} FLAME_TIM_CONTROL;


typedef struct _STATE_IND
{
    uint8_t  reserved : 3;
    uint8_t  tw       : 1;
    uint8_t  pe       : 1;
    uint8_t  te       : 1;
    uint8_t  se       : 1;
    uint8_t  sc       : 1;

} STATE_IND;

typedef union
{
    uint8_t cfig_ind;
    struct _CONFIGURE_IND
    {
        uint8_t  reserved1 : 2;
        uint8_t  m         : 1;
        uint8_t  c         : 1;
        uint8_t  ap        : 1;
        uint8_t  aa        : 1;
        uint8_t  b         : 1;
        uint8_t  reserved2 : 1;
    } s_cfig_ind;
} CONFIGURE_IND;

typedef struct _SYSYTEM_STATE_IND
{
    uint8_t  mode      : 2;
    uint8_t  tw        : 1;
    uint8_t  xtal      : 1;
    uint8_t  vfilt     : 1;
    uint8_t  vbus      : 1;
    uint8_t  vdd2      : 1;
    uint8_t  v20v      : 1;

} SYSYTEM_STATE_IND;

//typedef struct _DEVEICE_ADDRESS
//{
//    uint8_t  mode         ;
//    uint8_t  tw        : 1;
//    uint8_t  xtal      : 1;
//    uint8_t  vfilt     : 1;
//    uint8_t  vbus      : 1;
//    uint8_t  vdd2      : 1;
//    uint8_t  v20v      : 1;
//
//}DEVEICE_ADDRESS;


typedef union
{
    uint8_t repet_cnt;
    struct _CNT
    {
        uint8_t  Nack    : 3;
        uint8_t          : 1;
        uint8_t  busy    : 3;
        uint8_t          : 1;

    } cnt;
} REPET_CNT;

typedef struct _DEVICE_INFO
{
    UART_HandleTypeDef *huart;
    NCN5120_CONTROL_STATUS    ControlState ;              /* of type CONTROL_STATE */
    FLAME_TIM_CONTROL             Timectrl ;
    STATE_IND                    State_Ind ;
    CONFIGURE_IND            Configure_Ind ;
    SYSYTEM_STATE_IND     System_State_Ind ;
    REPET_CNT                    Repet_Cnt ;
    uint16_t                Device_Address ;
    uint8_t   UratRxbuf;
    uint8_t   Rxflag ;
    uint8_t   RxBufByteCtr ;
    uint8_t *RxBufPtr ;
    uint8_t   RxBuf[MAX_NCN5120_BUF] ;

    uint8_t   TxFlag;
    uint8_t   TxRepetFlag;
    uint8_t   TxBufByteCtr ;
    uint8_t *TxBufPtr ;
    uint8_t   TxBuf[MAX_NCN5120_BUF];
} DEVICE_INFO;


typedef struct
{
    uint8_t datacnt;
    uint8_t RxBuf[MAX_NCN5120_BUF];
} NCN5120_REC;


void NCN5120_Init(UART_HandleTypeDef *huart);

void cb_event(uint32_t event);

void KNX_MS_RX_Handler(unsigned char *rxbuf);

void NCN5120_Drive(void const *argument);

uint8_t NCN5120_Get_Status(void);

void NCN5120_Status_Reset(void);

uint8_t NCN5120_Send_Frame_Service(uint8_t *buf, uint8_t cnt);

void NCN5120_TimeoutCallback(void);
uint8_t NCN5120_UART_RxCallback(void);

#endif
