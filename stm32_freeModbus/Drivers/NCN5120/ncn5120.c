
#include <string.h>
#include <stdlib.h>
#include "ncn5120.h"


uint8_t charRx;
uint8_t NCN5120RecBuf[MAX_NCN5120_BUF];
NCN5120_REC *pNcn5120RecBuf[10];
DEVICE_INFO Device_Info;

uint8_t pbufin = 0;
uint8_t pbufout = 0;

static uint8_t cindex = 0;

void NCN5120_TimeoutCallback(void)
{
    /* Prevent unused argument(s) compilation warning */
    /* NOTE : This function Should not be modified, when the callback is needed,
              the __HAL_TIM_PeriodElapsedCallback could be implemented in the user file
     */
    NCN5120_REC *p;

    p = malloc(sizeof(NCN5120_REC));

    p->datacnt = cindex;
    memcpy(p->RxBuf, NCN5120RecBuf, cindex);

    pNcn5120RecBuf[pbufin++] = p;

    if (pbufin >= 10)
    {
        pbufin = 0;
    }

    cindex = 0;
}


uint8_t NCN5120_UART_RxCallback(void)
{
    /* Prevent unused argument(s) compilation warning */
    /* NOTE: This function Should not be modified, when the callback is needed,
             the HAL_UART_RxCpltCallback could be implemented in the user file
     */

    NCN5120RecBuf[cindex] =  charRx;
    cindex++;

    return charRx;
}


void NCN5120_SendFrame_RET_Handler(DEVICE_INFO *devfo)
{
    uint8_t byte;
    byte = devfo->RxBuf[devfo->RxBufByteCtr - 1];
    if ((byte == 0x0b) || (byte == 0x8b))
    {
        devfo->TxRepetFlag = 1;
        Device_Info.ControlState = FREE;

    }
    else
    {
        devfo->TxRepetFlag = 0;
        Device_Info.ControlState = WAIT_SENDFRAME_CMD_RET;
    }
}


__weak void KNX_Frame_Receive_CallBack(uint8_t *buf, uint8_t cnt)
{

}

void ReceiveFrame_Handler(DEVICE_INFO *devfo)
{
    uint8_t cnt;
    if (devfo->RxBuf[devfo->RxBufByteCtr - 3] == 0xCB)
    {
        cnt = devfo->RxBufByteCtr - 3;
    }
    else
    {
        cnt = devfo->RxBufByteCtr - 2;
    }

    KNX_Frame_Receive_CallBack(devfo->RxBuf, cnt);
}



void Ncn5120_Rec_Handler(void)
{
    uint8_t  Cmdret = Device_Info.ControlState;

    switch (Cmdret)
    {
    case WAIT_REST_CMD_RET:
        if (Device_Info.RxBuf[0] == 0x03)
        {
//                 osSemaphoreRelease(Device_Info.RXHandlerOK);
            Device_Info.ControlState = FREE;
        }
        //        NCN5120_Reset_Handler(&Device_Info);
        break;
    case WAIT_RDSTATE_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_Rdstate_Handler(&Device_Info);
        break;
    case WAIT_SETBUSY_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_SetBusy_Handler(&Device_Info);
        break;
    case WAIT_QUITBUSY_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_QuitBusy_Handler(&Device_Info);
        break;
    case WAIT_SETBUSMON_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_SetBusmon_Handler(&Device_Info);
        break;
    case WAIT_SETADDRESS_CMD_RET:
        Device_Info.Configure_Ind.cfig_ind = Device_Info.RxBuf[0];
        Device_Info.ControlState = FREE;
//            Driver_USART1.Send(Device_Info.RxBuf,Device_Info.RxBufByteCtr);
//            NCN5120_SetAddress_Handler(&Device_Info);
        break;
    case WAIT_SETREPETITION_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_SetReptition_Handler(&Device_Info);
        break;
    case WAIT_RDSYSTEMSTATE_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_RdSystemState_Handler(&Device_Info);
        break;
    case WAIT_SETSTOPMODE_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_SetStopMode_Handler(&Device_Info);
        break;
    case WAIT_EXITSTOPMODE_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_ExitStopMode_Handler(&Device_Info);
        break;
    case WAIT_ACKN_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_ACKN_Handler(&Device_Info);
        break;
    case WAIT_CONFIGURE_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_Configure_Handler(&Device_Info);
        break;
    case WAIT_INTREGWR_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_IntRegWr_Handler(&Device_Info);
        break;
    case WAIT_INTREGRD_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_IntRegRd_Handler(&Device_Info);
        break;
    case WAIT_POLLINGSTATE_CMD_RET:
        Device_Info.ControlState = FREE;
//            NCN5120_PollingState_Handler(&Device_Info);
        break;
    case WAIT_SENDFRAME_CMD_RET:
        NCN5120_SendFrame_RET_Handler(&Device_Info);

        break;
    case FREE :
        ReceiveFrame_Handler(&Device_Info);
        break;
    default :
        break;
    }
}

void NCN5120_RxDrive_Monitor(void)
{
    NCN5120_REC *p;
    if (pbufin != pbufout)
    {
        p = pNcn5120RecBuf[pbufout++];
        if (pbufout >= 10)
        {
            pbufout = 0;
        }
        Device_Info.RxBufByteCtr = p->datacnt;
        memcpy(Device_Info.RxBuf, p->RxBuf, p->datacnt);
        free(p);
        Ncn5120_Rec_Handler();
    }
}

void NCN5120_Send_Handler(uint8_t *buf, uint8_t cnt)
{
    uint8_t  Cmd = Device_Info.ControlState;
    switch (Cmd)
    {
    case SEND_REST_CMD:
        Device_Info.ControlState = WAIT_REST_CMD_RET;
        break;
    case SEND_RDSTATE_CMD:
        Device_Info.TxBuf[0] = RdState_cmd ;
        Device_Info.ControlState = WAIT_RDSTATE_CMD_RET;
        break;
    case SEND_SETBUSY_CMD:
        Device_Info.TxBuf[0] = SetBusy_cmd ;
        Device_Info.ControlState = WAIT_SETBUSY_CMD_RET;
        break;
    case SEND_QUITBUSY_CMD:
        Device_Info.TxBuf[0] = QuitBusy_cmd ;
        Device_Info.ControlState = WAIT_QUITBUSY_CMD_RET;
        break;
    case SEND_SETBUSMON_CMD:
        Device_Info.TxBuf[0] = SetBusmon_cmd ;
        Device_Info.ControlState = WAIT_SETBUSMON_CMD_RET;
        break;
    case SEND_SETADDRESS_CMD:
        Device_Info.ControlState = WAIT_SETADDRESS_CMD_RET;
        break;
    case SEND_SETREPETITION_CMD:
        // Device_Info.TxBuf[0] = SetRepetition_cmd ;
        Device_Info.ControlState = FREE;
        break;
    case SEND_RDSYSTEMSTATE_CMD:
        Device_Info.TxBuf[0] = RdSystemState_cmd ;
        Device_Info.ControlState = WAIT_RDSYSTEMSTATE_CMD_RET;
        break;
    case SEND_SETSTOPMODE_CMD:
        Device_Info.TxBuf[0] = SetStopMode_cmd ;
        Device_Info.ControlState = WAIT_SETSTOPMODE_CMD_RET;
        break;
    case SEND_EXITSTOPMODE_CMD:
        Device_Info.TxBuf[0] = ExitStopMode_cmd ;
        Device_Info.ControlState = WAIT_EXITSTOPMODE_CMD_RET;
        break;
    case SEND_ACKN_CMD:
        Device_Info.TxBuf[0] = Ackn_cmd ;
        Device_Info.ControlState = WAIT_ACKN_CMD_RET;
        break;
    case SEND_CONFIGURE_CMD:
        //Device_Info.TxBuf[0] = Configure_cmd ;
        Device_Info.ControlState = WAIT_CONFIGURE_CMD_RET;
        break;
    case SEND_INTREGWR_CMD:
        Device_Info.TxBuf[0] = IntRegWr_cmd ;
        Device_Info.ControlState = WAIT_INTREGWR_CMD_RET;
        break;
    case SEND_INTREGRD_CMD:
        Device_Info.TxBuf[0] = IntRegRd_cmd ;
        Device_Info.ControlState = WAIT_INTREGRD_CMD_RET;
        break;
    case SEND_POLLINGSTATE_CM:
        Device_Info.TxBuf[0] = PollingState_cmd ;
        Device_Info.ControlState = WAIT_POLLINGSTATE_CMD_RET;
        break;
    case SEND_SENDFRAME_CMD:
        Device_Info.ControlState = WAIT_SENDFRAME_CMD_RET;
        break;
    default :
        return;
    }
    HAL_UART_Transmit(Device_Info.huart, buf, cnt, 1000);
//    HAL_UART_Receive_IT(Device_Info.huart,&Device_Info.UratRxbuf,1);
}

void Reset_Service(void)
{

    Device_Info.TxBuf[0] = Rest_cmd;
    Device_Info.TxBufByteCtr = 1;
    Device_Info.ControlState = SEND_REST_CMD;

    NCN5120_Send_Handler(Device_Info.TxBuf, Device_Info.TxBufByteCtr);
//    Device_Info.RxBufPtr = Device_Info.RxBuf;
//    Device_Info.RxBufByteCtr = 0;

}


void SetAddress_Service(uint16_t add)
{
    Device_Info.Device_Address = add;
    Device_Info.TxBuf[0] = SetAddress_cmd;
    Device_Info.TxBuf[1] = HIBYTE(add);
    Device_Info.TxBuf[2] = LOBYTE(add);
    Device_Info.TxBuf[3] = DUMMY;
    Device_Info.TxBufByteCtr = 4;
    Device_Info.TxFlag = 1;
    Device_Info.ControlState = SEND_SETADDRESS_CMD;

    NCN5120_Send_Handler(Device_Info.TxBuf, Device_Info.TxBufByteCtr);
//    Device_Info.RxBufPtr = Device_Info.RxBuf;
//    Device_Info.RxBufByteCtr=0;
}

void Configure_Service(void)
{
    Device_Info.TxBuf[0] = 0x19;
    Device_Info.TxBufByteCtr = 1;
    Device_Info.ControlState = SEND_CONFIGURE_CMD;

    NCN5120_Send_Handler(Device_Info.TxBuf, Device_Info.TxBufByteCtr);

//    Device_Info.RxBufPtr = Device_Info.RxBuf;
//    Device_Info.RxBufByteCtr=0;
}

void SetRepetition_service(uint8_t Nackcnt, uint8_t Busycnt)
{
    Device_Info.Repet_Cnt.cnt.Nack = Nackcnt;
    Device_Info.Repet_Cnt.cnt.busy = Busycnt;
    Device_Info.TxBuf[0] = SetRepetition_cmd;
    Device_Info.TxBuf[1] = Device_Info.Repet_Cnt.repet_cnt;
    Device_Info.TxBuf[2] = DUMMY;
    Device_Info.TxBuf[3] = DUMMY;
    Device_Info.TxBufByteCtr = 4;
    Device_Info.TxFlag = 1;
    Device_Info.ControlState = SEND_SETREPETITION_CMD;

    NCN5120_Send_Handler(Device_Info.TxBuf, Device_Info.TxBufByteCtr);

}



uint8_t NCN5120_Send_Frame_Service(uint8_t *buf, uint8_t cnt)
{
    uint8_t i;
    Device_Info.TxBuf[0] = Sendframe_cmd;
    for (i = 0; i < cnt - 1; i++)
    {
        Device_Info.TxBuf[2 * i] = 0x80 ^ i;
        Device_Info.TxBuf[2 * i + 1] = buf[i];
    }
    Device_Info.TxBuf[2 * i] = 0x40 ^ i;
    Device_Info.TxBuf[2 * i + 1] = buf[i];
    Device_Info.TxBufByteCtr = 2 * cnt;
    Device_Info.TxFlag = 1;
    Device_Info.ControlState = SEND_SENDFRAME_CMD;
    NCN5120_Send_Handler(Device_Info.TxBuf, Device_Info.TxBufByteCtr);

    return 0;
}

void NCN5120_Init(UART_HandleTypeDef *huart)
{
    Device_Info.Timectrl.TimeoutEn = 0;
    Device_Info.Timectrl.TimeoutCnts = 20;
    Device_Info.huart = huart;
    Device_Info.RxBufPtr = Device_Info.RxBuf;
    Device_Info.RxBufByteCtr = 0;

    HAL_UART_Receive_IT(huart, &charRx, 1);
    Reset_Service();
    SetAddress_Service(0x1801);
    Configure_Service();
    SetRepetition_service(NACKREPET, BUSYREPET);

}




















