
/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include <product_config.h>
#include <mdrv.h>
#include <mdrv_errno.h>
#include <soc_socp_adapter.h>
#include <securec.h>
#include <bsp_diag.h>
#include <bsp_slice.h>
#include <bsp_dump.h>
#include "diag_system_debug.h"
#include "diag_service.h"
#include "diag_report.h"


#define  THIS_MODU mod_diag
u32 g_ulTransId = 0;
u32 g_ulCnfTransId = 0;
DIAG_LOG_PKT_NUM_ACC_STRU g_DiagLogPktNum ={0};

/*****************************************************************************
 �� �� ��  : diag_GetFileNameFromPath
 ��������  : �õ��ļ�·������ƫ��ֵ
 �������  : char* pcFileName
*****************************************************************************/
char * diag_GetFileNameFromPath(char* pcFileName)
{
    char    *pcPathPos1;
    char    *pcPathPos2;
    char    *pcPathPos;

    /* ����ϵͳ����ʹ��'\'������·�� */
    pcPathPos1 = (char*)strrchr(pcFileName, '\\');
    if(NULL == pcPathPos1)
    {
        pcPathPos1 = pcFileName;
    }

    /* ����ϵͳ����ʹ��'/'������·�� */
    pcPathPos2 = (char*)strrchr(pcFileName, '/');
    if(NULL == pcPathPos2)
    {
        pcPathPos2 = pcFileName;
    }

    pcPathPos = (pcPathPos1 > pcPathPos2) ? pcPathPos1 : pcPathPos2;

    /* ���û�ҵ�'\'��'/'��ʹ��ԭ�����ַ���������ʹ�ýضϺ���ַ��� */
    if (pcFileName != pcPathPos)
    {
        pcPathPos++;
    }

    return pcPathPos;
}

/*****************************************************************************
 �� �� ��  : diag_report_log
 ��������  : ��ӡ�ϱ��ӿ�
 �������  : ulModuleId( 31-24:modemid,23-16:modeid,15-12:level )
                          pcFileName(�ϱ�ʱ����ļ�·��ɾ����ֻ�����ļ���)
                          ulLineNum(�к�)
                          pszFmt(�ɱ����)
ע������   : ���ڴ˽ӿڻᱻЭ��ջƵ�����ã�Ϊ��ߴ���Ч�ʣ����ӿ��ڲ���ʹ��1K�ľֲ����������ϱ����ַ�����Ϣ��
             �Ӷ��˽ӿڶ�Э��ջ���������ƣ�һ�ǵ��ô˽ӿڵ�����ջ�е��ڴ�Ҫ����Ϊ�˽ӿ�Ԥ��1K�ռ䣻
                                           ���ǵ��ô˽ӿ������log��Ҫ����1K���������ֻ��Զ�������
*****************************************************************************/
u32 diag_report_log(u32 ulModuleId, u32 ulPid, u32 ullevel, char *cFileName, u32 ulLineNum, char *pszFmt, va_list arg)
{
    DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU stRptInfo;
    DIAG_SRV_LOG_HEADER_STRU    stLogHeader;
    DIAG_MSG_REPORT_HEAD_STRU   stDiagHead;
    unsigned long  ulLockLevel;
    char *cOffsetName;
    s32 ulParamLen =0;
    u32 usRet = 0;
    u32 ulDataLength =0;

    if(NULL == cFileName)
    {
        cFileName= " ";
    }

    /* �ļ����ض� */
    cOffsetName = diag_GetFileNameFromPath(cFileName);

    /*��HSO�Ĵ�ӡ�ַ�����ʽ����:pszFileName[ulLineNum]data��HSO����������[]ȥ��ȡ��Ӧ����Ϣ*/
    /* coverity[negative_return_fn] */
    ulParamLen = snprintf_s(stRptInfo.szText, DIAG_PRINTF_MAX_LEN, DIAG_PRINTF_MAX_LEN-1, "%s[%d]", cOffsetName, ulLineNum);
    if(ulParamLen < 0)
    {
        diag_error("snprintf_s fail,ulParamLen:0x%x\n", ulParamLen);
        return ERR_MSP_FAILURE;
    }
    else if(ulParamLen > DIAG_PRINTF_MAX_LEN)
    {
        /* �ڴ�Խ�磬������λ */
        system_error(DRV_ERRNO_DIAG_OVERFLOW, __LINE__, ulParamLen, 0, 0);
        return ERR_MSP_FAILURE;
    }

    /* coverity[negative_returns] */
    usRet = vscnprintf((stRptInfo.szText + ulParamLen), (u32)(DIAG_PRINTF_MAX_LEN - ulParamLen), (const char *)pszFmt, arg);
    if(usRet < 0)
    {
        diag_error("vsnprintf fail, usRet:0x%x\n", usRet);
        return ERR_MSP_FAILURE;
    }

    ulParamLen += usRet;
    if(ulParamLen > DIAG_PRINTF_MAX_LEN)
    {
        /* �ڴ�Խ�磬������λ */
        system_error(DRV_ERRNO_DIAG_OVERFLOW, __LINE__, ulParamLen, 0, 0);
    }

    stRptInfo.szText[DIAG_PRINTF_MAX_LEN-1] = '\0';
    ulDataLength = strnlen(stRptInfo.szText, DIAG_PRINTF_MAX_LEN) + 1;

    /*��װDIAG�������*/
    stRptInfo.ulModule = ulPid;
    /* 1:error, 2:warning, 3:normal, 4:info */
    /* (0|ERROR|WARNING|NORMAL|INFO|0|0|0) */
    stRptInfo.ulLevel  = ullevel;

    /* �ַ����ĳ��ȼ�����Ϣ�ĳ��� */
    ulDataLength += (sizeof(DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU) - (DIAG_PRINTF_MAX_LEN + 1));

    spin_lock_irqsave(&g_DiagLogPktNum.ulPrintLock, ulLockLevel);
    stRptInfo.ulNo = (g_DiagLogPktNum.ulPrintNum)++;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulPrintLock, ulLockLevel);

    /* �������ͷ */
    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stLogHeader);
    DIAG_SRV_SET_MODEM_ID(&stLogHeader.frame_header, DIAG_GET_MODEM_ID(ulModuleId));
    DIAG_SRV_SET_TRANS_ID(&stLogHeader.frame_header, g_ulTransId++);
    DIAG_SRV_SET_COMMAND_ID(&stLogHeader.frame_header, DIAG_FRAME_MSG_TYPE_PS, DIAG_GET_MODE_ID(ulModuleId), DIAG_FRAME_MSG_PRINT, 0);
    DIAG_SRV_SET_MSG_LEN(&stLogHeader.frame_header, ulDataLength);

    stDiagHead.ulHeaderSize     = sizeof(stLogHeader);
    stDiagHead.pHeaderData      = &stLogHeader;

    stDiagHead.ulDataSize       = ulDataLength;
    stDiagHead.pData            = (void *)&stRptInfo;

    return diag_ServicePackData(&stDiagHead);
}

/*****************************************************************************
 �� �� ��  : DIAG_TransReport_Ex
 ��������  : �ṹ�������ϱ���չ�ӿ�(��DIAG_TransReport�ഫ����DIAG_MESSAGE_TYPE)
 �������  : DRV_DIAG_TRANS_IND_STRU->ulModule( 31-24:modemid,23-16:modeid,15-12:level,11-8:groupid )
             DRV_DIAG_TRANS_IND_STRU->ulMsgId(͸������ID)
             DRV_DIAG_TRANS_IND_STRU->ulLength(͸����Ϣ�ĳ���)
             DRV_DIAG_TRANS_IND_STRU->pData(͸����Ϣ)
*****************************************************************************/
u32 diag_report_trans(DRV_DIAG_TRANS_IND_STRU *pstData)
{
    DIAG_SRV_TRANS_HEADER_STRU  stTransHeader;
    DIAG_CMD_TRANS_IND_STRU*    pstTransInfo;
    DIAG_MSG_REPORT_HEAD_STRU   stDiagHead;
    unsigned long  ulLockLevel;

    pstTransInfo = &stTransHeader.trans_header;
    pstTransInfo->ulModule = pstData->ulPid;
    pstTransInfo->ulMsgId  = pstData->ulMsgId;

    spin_lock_irqsave(&g_DiagLogPktNum.ulTransLock, ulLockLevel);
    pstTransInfo->ulNo     = (g_DiagLogPktNum.ulTransNum)++;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulTransLock, ulLockLevel);

    /* �������ͷ */
    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stTransHeader);
    DIAG_SRV_SET_MODEM_ID(&stTransHeader.frame_header, DIAG_GET_MODEM_ID(pstData->ulModule));
    DIAG_SRV_SET_TRANS_ID(&stTransHeader.frame_header, g_ulTransId++);
    DIAG_SRV_SET_COMMAND_ID(&stTransHeader.frame_header, DIAG_GET_GROUP_ID(pstData->ulModule), \
                            DIAG_GET_MODE_ID(pstData->ulModule), DIAG_FRAME_MSG_STRUCT, ((pstData->ulMsgId)&0x7ffff));
    DIAG_SRV_SET_MSG_LEN(&stTransHeader.frame_header, sizeof(stTransHeader.trans_header) + pstData->ulLength);

    stDiagHead.ulHeaderSize     = sizeof(stTransHeader);
    stDiagHead.pHeaderData      = &stTransHeader;
    stDiagHead.ulDataSize       = pstData->ulLength;
    stDiagHead.pData            = pstData->pData;

    return diag_ServicePackData(&stDiagHead);
}

/*****************************************************************************
 �� �� ��  : DIAG_EventReport
 ��������  : �¼��ϱ��ӿڣ���PSʹ��(�滻ԭ����DIAG_ReportEventLog)
 �������  : DRV_DIAG_EVENT_IND_STRU->ulModule( 31-24:modemid,23-16:modeid,15-12:level,11-0:pid )
             DRV_DIAG_EVENT_IND_STRU->ulEventId(event ID)
             DRV_DIAG_EVENT_IND_STRU->ulLength(event�ĳ���)
             DRV_DIAG_EVENT_IND_STRU->pData(event��Ϣ)
*****************************************************************************/
u32 diag_report_event(DRV_DIAG_EVENT_IND_STRU *pstEvent)
{
    DIAG_SRV_EVENT_HEADER_STRU  stEventHeader;
    DIAG_CMD_LOG_EVENT_IND_STRU *pstEventIndInfo;
    DIAG_MSG_REPORT_HEAD_STRU   stDiagHead;
    unsigned long  ulLockLevel;

    pstEventIndInfo = &stEventHeader.event_header;
    /*��װDIAG�������*/
    spin_lock_irqsave(&g_DiagLogPktNum.ulEventLock, ulLockLevel);
    pstEventIndInfo->ulNo     = (g_DiagLogPktNum.ulEventNum)++;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulEventLock, ulLockLevel);

    pstEventIndInfo->ulId     = pstEvent->ulEventId;
    pstEventIndInfo->ulModule = pstEvent->ulPid;

    /* �������ͷ */
    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stEventHeader);
    DIAG_SRV_SET_MODEM_ID(&stEventHeader.frame_header, DIAG_GET_MODEM_ID(pstEvent->ulModule));
    DIAG_SRV_SET_TRANS_ID(&stEventHeader.frame_header, g_ulTransId++);
    DIAG_SRV_SET_COMMAND_ID(&stEventHeader.frame_header, DIAG_FRAME_MSG_TYPE_PS, \
                            DIAG_GET_MODE_ID(pstEvent->ulModule), DIAG_FRAME_MSG_EVENT, 0);
    DIAG_SRV_SET_MSG_LEN(&stEventHeader.frame_header, sizeof(stEventHeader.event_header) + pstEvent->ulLength);

    stDiagHead.ulHeaderSize     = sizeof(stEventHeader);
    stDiagHead.pHeaderData      = &stEventHeader;
    stDiagHead.ulDataSize       = pstEvent->ulLength;
    stDiagHead.pData            = pstEvent->pData;

    return diag_ServicePackData(&stDiagHead);
}

/*****************************************************************************
 �� �� ��  : DIAG_AirMsgReport
 ��������  : �տ���Ϣ�ϱ��ӿڣ���PSʹ��(�滻ԭ����DIAG_ReportAirMessageLog)
 �������  : DRV_DIAG_AIR_IND_STRU->ulModule( 31-24:modemid,23-16:modeid,15-12:level,11-0:pid )
             DRV_DIAG_AIR_IND_STRU->ulMsgId(�տ���ϢID)
             DRV_DIAG_AIR_IND_STRU->ulDirection(�տ���Ϣ�ķ���)
             DRV_DIAG_AIR_IND_STRU->ulLength(�տ���Ϣ�ĳ���)
             DRV_DIAG_AIR_IND_STRU->pData(�տ���Ϣ��Ϣ)
*****************************************************************************/
u32 diag_report_air(DRV_DIAG_AIR_IND_STRU *pstAir)
{
    DIAG_MSG_REPORT_HEAD_STRU   stDiagHead;
    DIAG_SRV_AIR_HEADER_STRU    stAirHeader;
    DIAG_CMD_LOG_AIR_IND_STRU   *pstRptInfo = NULL;
    unsigned long  ulLockLevel;

    pstRptInfo = &stAirHeader.air_header;

    pstRptInfo->ulModule  = pstAir->ulPid;
    pstRptInfo->ulId      = pstAir->ulMsgId;
    pstRptInfo->ulSide    = pstAir->ulDirection;

    spin_lock_irqsave(&g_DiagLogPktNum.ulAirLock, ulLockLevel);
    pstRptInfo->ulNo      = (g_DiagLogPktNum.ulAirNum)++;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulAirLock, ulLockLevel);

    /* �������ͷ */
    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stAirHeader);
    DIAG_SRV_SET_MODEM_ID(&stAirHeader.frame_header, DIAG_GET_MODEM_ID(pstAir->ulModule));
    DIAG_SRV_SET_TRANS_ID(&stAirHeader.frame_header, g_ulTransId++);
    DIAG_SRV_SET_COMMAND_ID(&stAirHeader.frame_header, DIAG_FRAME_MSG_TYPE_PS,
                            DIAG_GET_MODE_ID(pstAir->ulModule), DIAG_FRAME_MSG_AIR, 0);
    DIAG_SRV_SET_MSG_LEN(&stAirHeader.frame_header, sizeof(stAirHeader.air_header) + pstAir->ulLength);

    stDiagHead.ulHeaderSize     = sizeof(stAirHeader);
    stDiagHead.pHeaderData      = &stAirHeader;
    stDiagHead.ulDataSize       = pstAir->ulLength;
    stDiagHead.pData            = pstAir->pData;

    return diag_ServicePackData(&stDiagHead);
}

/*****************************************************************************
 �� �� ��  : DIAG_TraceReport
 ��������  : �����Ϣ�ϱ��ӿڣ���PSʹ��(�滻ԭ����DIAG_ReportLayerMessageLog)
 �������  : pMsg(��׼��VOS��Ϣ�壬Դģ�顢Ŀ��ģ����Ϣ����Ϣ���л�ȡ)
*****************************************************************************/
u32 diag_report_trace(void *pMsg, u32 modemid)
{
    DIAG_SRV_TRACE_HEADER_STRU  stTraceHeader;
    DIAG_CMD_LOG_LAYER_IND_STRU *pstTrace;
    DIAG_MSG_REPORT_HEAD_STRU   stDiagHead;
    DIAG_OSA_MSG_STRU           *pDiagMsg = (DIAG_OSA_MSG_STRU *)pMsg;
    unsigned long  ulLockLevel;
    u32  ulMsgLen;

    pstTrace = &stTraceHeader.trace_header;
    pstTrace->ulModule    = pDiagMsg->ulSenderPid;
    pstTrace->ulDestMod   = pDiagMsg->ulReceiverPid;
    pstTrace->ulId        = pDiagMsg->ulMsgId;

    spin_lock_irqsave(&g_DiagLogPktNum.ulTraceLock, ulLockLevel);
    pstTrace->ulNo        = (g_DiagLogPktNum.ulTraceNum++);
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulTraceLock, ulLockLevel);

    /* �������ͷ */
    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stTraceHeader);
    DIAG_SRV_SET_MODEM_ID(&stTraceHeader.frame_header, modemid);
    DIAG_SRV_SET_TRANS_ID(&stTraceHeader.frame_header, g_ulTransId++);
    DIAG_SRV_SET_COMMAND_ID(&stTraceHeader.frame_header, DIAG_FRAME_MSG_TYPE_PS, \
                            DIAG_FRAME_MODE_COMM, DIAG_FRAME_MSG_LAYER, 0);
    ulMsgLen = sizeof(DIAG_OSA_MSG_STRU) - sizeof(pDiagMsg->ulMsgId) + pDiagMsg->ulLength;
    DIAG_SRV_SET_MSG_LEN(&stTraceHeader.frame_header, sizeof(stTraceHeader.trace_header) + ulMsgLen);

    stDiagHead.ulHeaderSize     = sizeof(stTraceHeader);
    stDiagHead.pHeaderData      = &stTraceHeader;
    stDiagHead.ulDataSize       = ulMsgLen;
    stDiagHead.pData            = pDiagMsg;

    return diag_ServicePackData(&stDiagHead);
}

/*****************************************************************************
 �� �� ��     : diag_report_msg_trans
 ��������  : ͨ����Ϣ�ϱ��ӿ�
 �������  : void
*****************************************************************************/
u32 diag_report_msg_trans(DRV_DIAG_TRANS_IND_STRU *pstData, u32 ulcmdid)
{
    DIAG_SRV_COMM_HEADER_STRU  stTransHeader;
    DIAG_CMD_TRANS_IND_STRU*    pstTransInfo;
    DIAG_MSG_REPORT_HEAD_STRU  stDiagHead;
    unsigned long  ulLockLevel;

    pstTransInfo = &stTransHeader.comm_header;
    pstTransInfo->ulModule = pstData->ulPid;

    spin_lock_irqsave(&g_DiagLogPktNum.ulTransLock, ulLockLevel);
    pstTransInfo->ulNo     = (g_DiagLogPktNum.ulTransNum)++;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulTransLock, ulLockLevel);

    /* �������ͷ */
    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stTransHeader);
    DIAG_SRV_SET_MODEM_ID(&stTransHeader.frame_header, DIAG_GET_MODEM_ID(pstData->ulModule));
    DIAG_SRV_SET_TRANS_ID(&stTransHeader.frame_header, g_ulTransId++);
    DIAG_SRV_SET_COMMAND_ID_EX(&stTransHeader.frame_header, ulcmdid);
    DIAG_SRV_SET_MSG_LEN(&stTransHeader.frame_header, sizeof(stTransHeader.comm_header) + pstData->ulLength);

    stDiagHead.ulHeaderSize     = sizeof(stTransHeader);
    stDiagHead.pHeaderData      = &stTransHeader;
    stDiagHead.ulDataSize       = pstData->ulLength;
    stDiagHead.pData            = pstData->pData;

    return diag_ServicePackData(&stDiagHead);
}

/*****************************************************************************
 Function Name   : DIAG_MsgReport
 Description        : DIAG message ���ϱ��ӿ�
*****************************************************************************/
u32 diag_report_cnf(DRV_DIAG_CNF_INFO_STRU *pstDiagInfo, void *pstData, u32 ulLen)
{
    DIAG_SRV_CNF_HEADER_STRU    stCnfHeader;
    DIAG_MSG_REPORT_HEAD_STRU   stDiagHead;

    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stCnfHeader);
    DIAG_SRV_SET_MODEM_ID(&stCnfHeader.frame_header, pstDiagInfo->ulModemid);
    DIAG_SRV_SET_TRANS_ID(&stCnfHeader.frame_header, g_ulCnfTransId++);
    DIAG_SRV_SET_COMMAND_ID(&stCnfHeader.frame_header, (pstDiagInfo->ulMsgType & 0xf), (pstDiagInfo->ulMode & 0xf), (pstDiagInfo->ulSubType & 0x1f), (pstDiagInfo->ulMsgId & 0x7ffff));
    DIAG_SRV_SET_MSG_LEN(&stCnfHeader.frame_header, ulLen);

    stDiagHead.ulHeaderSize     = sizeof(stCnfHeader);
    stDiagHead.pHeaderData      = &stCnfHeader;

    stDiagHead.ulDataSize       = ulLen;
    stDiagHead.pData            = pstData;

    diag_DrvLNR(EN_DIAG_DRV_LNR_MESSAGE_RPT, stCnfHeader.frame_header.u32CmdId, bsp_get_slice_value());

    return diag_ServicePackCnfData(&stDiagHead);
}

/*************************************************************************
 �� �� ��	: diag_report_drv_log
 ��������	: �ϱ��ַ������ݵ����ߣ����ܳ���300�ֽڣ�����300�ֽڲ����Զ��ض�
 �������	: ��
 �� �� ֵ	: �ɹ�����ʶ��
 �޸���ʷ	:
*************************************************************************/
u32 diag_report_drv_log(u32 level, char* fmt, va_list arg)
{
    DIAG_DRV_STRING_HEADER_STRU stStringHeader = {};
    DIAG_MSG_REPORT_HEAD_STRU   stDiagHead;
    diag_print_head_stru *print_head = NULL;
    unsigned long ulLockLevel;
    char string[DIAG_DRV_PRINTLOG_MAX_BUFF_LEN];
    s32 len = 0;
    s32 usRet = 0;

    /* ���ڵ����ӡ�л�����[], HIDSʶ��ʱ����Ϊ�����е�[] Ϊ�кţ���˴˴�Ĭ����д[] */
    usRet = snprintf_s(string, DIAG_DRV_PRINTLOG_MAX_BUFF_LEN, DIAG_DRV_PRINTLOG_MAX_BUFF_LEN - 1, "%s", "ap[0]");
    if(usRet < 0)
    {
        diag_error("snprintf_s return error ret:0x%x\n", usRet);
        return ERR_MSP_INALID_LEN_ERROR;
    }
    len = usRet;

    usRet = vscnprintf(string + sizeof("ap[0]") - 1, DIAG_DRV_PRINTLOG_MAX_BUFF_LEN - sizeof("ap[0]"), fmt, arg);
    if(usRet < 0)
    {
        diag_error("vsnprintf return error ret:0x%x\n", usRet);
        return ERR_MSP_INALID_LEN_ERROR;
    }
    len += usRet;

    if(len > DIAG_DRV_PRINTLOG_MAX_BUFF_LEN)
    {
        /* �ڴ�Խ�磬������λ */
        system_error(DRV_ERRNO_DIAG_OVERFLOW, __LINE__, len, 0, 0);
    }

    print_head = &stStringHeader.print_head;

    /*fill print head*/
    /* 1:error, 2:warning, 3:normal, 4:info */
    /* (0|ERROR|WARNING|NORMAL|INFO|0|0|0) */
    print_head->u32level = (0x80000000) >> level;
    print_head->u32module = 0x8003;//0x8003Ϊ�����PID

    spin_lock_irqsave(&g_DiagLogPktNum.ulPrintLock, ulLockLevel);
    print_head->u32no = (g_DiagLogPktNum.ulPrintNum)++;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulPrintLock, ulLockLevel);

    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stStringHeader);
    DIAG_SRV_SET_MODEM_ID(&stStringHeader.frame_header, 0);
    DIAG_SRV_SET_TRANS_ID(&stStringHeader.frame_header, g_ulTransId++);
    DIAG_SRV_SET_COMMAND_ID(&stStringHeader.frame_header, DIAG_FRAME_MSG_TYPE_BSP, DIAG_FRAME_MODE_COMM, DIAG_FRAME_MSG_PRINT, 0);
    DIAG_SRV_SET_MSG_LEN(&stStringHeader.frame_header, sizeof(stStringHeader.print_head) + len);

    stDiagHead.ulHeaderSize     = sizeof(stStringHeader);
    stDiagHead.pHeaderData      = &stStringHeader;

    stDiagHead.ulDataSize       = len;
    stDiagHead.pData            = (void *)string;

    return diag_ServicePackData(&stDiagHead);
}

void diag_report_init(void)
{
    spin_lock_init(&g_DiagLogPktNum.ulPrintLock);
    spin_lock_init(&g_DiagLogPktNum.ulAirLock);
    spin_lock_init(&g_DiagLogPktNum.ulVoLTELock);
    spin_lock_init(&g_DiagLogPktNum.ulTraceLock);
    spin_lock_init(&g_DiagLogPktNum.ulUserLock);
    spin_lock_init(&g_DiagLogPktNum.ulEventLock);
    spin_lock_init(&g_DiagLogPktNum.ulTransLock);
}

/*****************************************************************************
 �� �� ��  : diag_report_reset_msg
 ��������  :
 �������  : �ϱ�������λ��Ϣ
*****************************************************************************/
u32 diag_report_reset_msg(DRV_DIAG_TRANS_IND_STRU *pstData)
{
    DIAG_SRV_TRANS_HEADER_STRU  stTransHeader;
    DIAG_CMD_TRANS_IND_STRU*    pstTransInfo;
    DIAG_MSG_REPORT_HEAD_STRU   stDiagHead;
    unsigned long  ulLockLevel;

    pstTransInfo = &stTransHeader.trans_header;
    pstTransInfo->ulModule = pstData->ulPid;
    pstTransInfo->ulMsgId  = pstData->ulMsgId;

    spin_lock_irqsave(&g_DiagLogPktNum.ulTransLock, ulLockLevel);
    pstTransInfo->ulNo     = (g_DiagLogPktNum.ulTransNum)++;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulTransLock, ulLockLevel);

    /* �������ͷ */
    diag_SvcFillHeader((DIAG_SRV_HEADER_STRU *)&stTransHeader);
    DIAG_SRV_SET_MODEM_ID(&stTransHeader.frame_header, DIAG_GET_MODEM_ID(pstData->ulModule));
    DIAG_SRV_SET_TRANS_ID(&stTransHeader.frame_header, g_ulTransId++);
    DIAG_SRV_SET_COMMAND_ID(&stTransHeader.frame_header, DIAG_GET_GROUP_ID(pstData->ulModule), \
                            DIAG_GET_MODE_ID(pstData->ulModule), DIAG_FRAME_MSG_CMD, ((pstData->ulMsgId)&0x7ffff));
    DIAG_SRV_SET_MSG_LEN(&stTransHeader.frame_header, sizeof(stTransHeader.trans_header) + pstData->ulLength);

    stDiagHead.ulHeaderSize     = sizeof(stTransHeader);
    stDiagHead.pHeaderData      = &stTransHeader;
    stDiagHead.ulDataSize       = pstData->ulLength;
    stDiagHead.pData            = pstData->pData;

    return diag_ServicePacketResetData(&stDiagHead);
}

void diag_report_reset(void)
{
    unsigned long ulLockLevel;

    g_ulTransId = 0;

    spin_lock_irqsave(&g_DiagLogPktNum.ulPrintLock, ulLockLevel);
    g_DiagLogPktNum.ulPrintNum = 0;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulPrintLock, ulLockLevel);

    spin_lock_irqsave(&g_DiagLogPktNum.ulTransLock, ulLockLevel);
    g_DiagLogPktNum.ulTransNum = 0;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulTransLock, ulLockLevel);

    spin_lock_irqsave(&g_DiagLogPktNum.ulEventLock, ulLockLevel);
    g_DiagLogPktNum.ulEventNum = 0;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulEventLock, ulLockLevel);

    spin_lock_irqsave(&g_DiagLogPktNum.ulAirLock, ulLockLevel);
    g_DiagLogPktNum.ulAirNum = 0;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulAirLock, ulLockLevel);

    spin_lock_irqsave(&g_DiagLogPktNum.ulVoLTELock, ulLockLevel);
    g_DiagLogPktNum.ulVoLTENum = 0;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulVoLTELock, ulLockLevel);

    spin_lock_irqsave(&g_DiagLogPktNum.ulTraceLock, ulLockLevel);
    g_DiagLogPktNum.ulTraceNum = 0;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulTraceLock, ulLockLevel);

    spin_lock_irqsave(&g_DiagLogPktNum.ulUserLock, ulLockLevel);
    g_DiagLogPktNum.ulUserNum = 0;
    spin_unlock_irqrestore(&g_DiagLogPktNum.ulUserLock, ulLockLevel);
}



