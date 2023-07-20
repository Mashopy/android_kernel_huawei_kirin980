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



#include <mdrv.h>
#include <scm_ind_src.h>
#include <scm_cnf_src.h>
#include <scm_debug.h>
#include <diag_system_debug.h>
#include <scm_init.h>
#include <scm_common.h>
#include <mdrv.h>
#include <scm_ind_src.h>
#include <scm_cnf_src.h>
#include <scm_ind_dst.h>
#include <scm_debug.h>
#include <diag_system_debug.h>
#include <OmCommonPpm.h>
#include <scm_init.h>
#include <scm_ind_dst.h>
#include <diag_port_manager.h>
#include <OmVcomPpm.h>
#include <scm_common.h>
#include <OmSocketPpm.h>
#include <OmUsbPpm.h>
#include <OmPortSwitch.h>
#include "diag_report.h"
#include "diag_service.h"
#include "OmCpPcdevPpm.h"


void mdrv_diag_PTR(DIAG_PTR_ID_ENUM enType, unsigned int paraMark, unsigned int para0, unsigned int para1)
{
    diag_PTR(enType, paraMark, para0, para1);
}

void mdrv_PPM_RegDisconnectCb(PPM_DisconnectTLPortFuc cb)
{
    PPM_RegDisconnectCb(cb);
}
unsigned int mdrv_GetThrputInfo(DIAG_THRPUT_ID_ENUM type)
{
    return DIAG_GetThrputInfo(type);
}

void mdrv_scm_reg_ind_coder_dst_send_fuc(void)
{
    scm_reg_ind_coder_dst_send_fuc();
}
unsigned int mdrv_PPM_LogPortSwitch(unsigned int  ulPhyPort, unsigned int ulEffect)
{
    return PPM_LogPortSwitch(ulPhyPort, (bool)ulEffect);
}
unsigned int mdrv_PPM_QueryLogPort(unsigned int  *pulLogPort)
{
    return PPM_QueryLogPort(pulLogPort);
}
void mdrv_PPM_QueryUsbInfo(void *PpmUsbInfoStru, unsigned int len)
{
    PPM_QueryUsbInfo(PpmUsbInfoStru, len);
}
void mdrv_PPM_ClearUsbTimeInfo(void)
{
    PPM_ClearUsbTimeInfo();
}
unsigned int mdrv_CPM_ComSend(CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort, unsigned char *pucVirData, unsigned char *pucPHYData, unsigned int ulLen)
{
    return CPM_ComSend(enLogicPort, pucVirData, pucPHYData, ulLen);
}
void mdrv_CPM_LogicRcvReg(CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort, CPM_RCV_FUNC pRcvFunc)
{
    PPM_SocketRevFunReg(enLogicPort, pRcvFunc);
}

void  mdrv_scm_set_power_on_log(void)
{
    scm_set_power_on_log();
}

unsigned int mdrv_diag_shared_mem_write(unsigned int eType, unsigned int len, char *pData)
{
    return diag_shared_mem_write(eType, len, pData);
}
unsigned int mdrv_diag_shared_mem_read(unsigned int eType)
{
    return diag_shared_mem_read(eType);
}
/*****************************************************************************
 �� �� ��     : mdrv_diag_debug_file_header
 ��������  : ΪDIAGά������ļ�ͷ
 �������  : void
*****************************************************************************/
unsigned int mdrv_diag_debug_file_header(void *pFile)
{
    return DIAG_DebugFileHeader(pFile);
}
/*****************************************************************************
 �� �� ��     : mdrv_diag_debug_file_tail
 ��������  : ΪDIAGά������ļ�β
 �������  : void
*****************************************************************************/
void mdrv_diag_debug_file_tail(void *pFile, char *FilePath)
{
    return DIAG_DebugFileTail(pFile, FilePath);
}

/*****************************************************************************
 �� �� ��     : mdrv_diag_report_log
 ��������  :
 �������  :
*****************************************************************************/
unsigned int mdrv_diag_report_log(unsigned int ulModuleId, unsigned int ulPid, unsigned int level, char *cFileName, unsigned int ulLineNum, char* pszFmt, va_list arg)
{
    return diag_report_log(ulModuleId, ulPid, level, cFileName, ulLineNum, pszFmt, arg);
}

/*****************************************************************************
 �� �� ��     : mdrv_diag_report_trans
 ��������  : �ṹ�������ϱ��ӿ�(�滻ԭ����DIAG_ReportCommand)
 �������  : DRV_DIAG_TRANS_IND_STRU->ulModule( 31-24:modemid,23-16:modeid )
             DRV_DIAG_TRANS_IND_STRU->ulMsgId(͸������ID)
             DRV_DIAG_TRANS_IND_STRU->ulLength(͸����Ϣ�ĳ���)
             DRV_DIAG_TRANS_IND_STRU->pData(͸����Ϣ)
*****************************************************************************/
unsigned int mdrv_diag_report_trans(DRV_DIAG_TRANS_IND_STRU *pstData)
{
    return diag_report_trans(pstData);
}
/*****************************************************************************
 �� �� ��  : DIAG_EventReport
 ��������  : �¼��ϱ��ӿ�
 �������  : DRV_DIAG_EVENT_IND_STRU->ulModule( 31-24:modemid,23-16:modeid,15-12:level,11-0:pid )
             DRV_DIAG_EVENT_IND_STRU->ulEventId(event ID)
             DRV_DIAG_EVENT_IND_STRU->ulLength(event�ĳ���)
             DRV_DIAG_EVENT_IND_STRU->pData(event��Ϣ)
*****************************************************************************/
unsigned int mdrv_diag_report_event(DRV_DIAG_EVENT_IND_STRU *pstData)
{
    return diag_report_event(pstData);
}
unsigned int mdrv_diag_report_air(DRV_DIAG_AIR_IND_STRU *pstData)
{
    return diag_report_air(pstData);
}

/*****************************************************************************
 �� �� ��     : DIAG_TraceReport
 ��������  : �����Ϣ�ϱ��ӿ�
 �������  : pMsg(��׼��VOS��Ϣ�壬Դģ�顢Ŀ��ģ����Ϣ����Ϣ���л�ȡ)
*****************************************************************************/
unsigned int mdrv_diag_report_trace(void *pstData, unsigned int modemid)
{
    return diag_report_trace(pstData, modemid);
}

/*****************************************************************************
 �� �� ��     : mdrv_diag_reset
 ��������  : ��λdiag�������
 �������  : void
*****************************************************************************/
void mdrv_diag_report_reset(void)
{
    return diag_report_reset();
}
/*****************************************************************************
 �� �� ��     : mdrv_diag_reset_mntn_info
 ��������  : ��λdiagά��ͳ����Ϣ
 �������  : void
*****************************************************************************/
void mdrv_diag_reset_mntn_info(DIAGLOG_MNTN_ENUM  type)
{
    if(DIAGLOG_SRC_MNTN == type)
    {
        return diag_DebugResetSrcMntnInfo();
    }
    else if(DIAGLOG_DST_MNTN == type)
    {
        return diag_reset_dst_mntn_info();
    }
}
/*****************************************************************************
 �� �� ��     : mdrv_diag_get_mntn_info
 ��������  : ��ȡά��ͳ����Ϣ
 �������  : void
*****************************************************************************/
void* mdrv_diag_get_mntn_info(DIAGLOG_MNTN_ENUM  type)
{
    if(DIAGLOG_SRC_MNTN == type)
    {
        return (void *)diag_DebugGetSrcMntnInfo();
    }
    else
    {
        return diag_DebugGetDstMntnInfo();
    }
}
/*****************************************************************************
 �� �� ��     : mdrv_diag_report_msg_trans
 ��������  : ��ȡά��ͳ����Ϣ
 �������  : void
*****************************************************************************/
unsigned int mdrv_diag_report_msg_trans(DRV_DIAG_TRANS_IND_STRU *pstData, unsigned int ulcmdid)
{
    return diag_report_msg_trans(pstData, ulcmdid);
}
/*****************************************************************************
 �� �� ��     : mdrv_diag_report_msg_trans
 ��������  : ��ȡά��ͳ����Ϣ
 �������  : void
*****************************************************************************/
unsigned int mdrv_diag_report_cnf(DRV_DIAG_CNF_INFO_STRU *pstData, void *pData, unsigned int ulLen)
{
    return diag_report_cnf(pstData, pData, ulLen);
}
/*****************************************************************************
 �� �� ��  : diag_report_reset_msg
 ��������  :
 �������  : �ϱ�������λ��Ϣ(CNFͨ��TRNANS��Ϣ)
*****************************************************************************/
unsigned int mdrv_diag_report_reset_msg(DRV_DIAG_TRANS_IND_STRU *pstData)
{
    return diag_report_reset_msg(pstData);
}
/*****************************************************************************
 �� �� ��  : bsp_diag_report_drv_log
 ��������  : �ϱ��ַ�����Ϣ
 �������  : void
*****************************************************************************/
unsigned int bsp_diag_report_drv_log(unsigned int level, char* fmt, va_list arg)
{
    return diag_report_drv_log(level, fmt, arg);
}

void mdrv_diag_ServiceProcReg(DRV_DIAG_SERVICE_FUNC pServiceFn)
{
    return diag_ServiceProcReg(pServiceFn);
}
extern u32 SCM_RegDecoderDestProc(SOCP_DECODER_DST_ENUM_U32 enChanlID, SCM_DECODERDESTFUCN func);
unsigned int mdrv_SCM_RegDecoderDestProc(SOCP_DECODER_DST_ENUM_U32 enChanlID, SCM_DECODERDESTFUCN func)
{
    return SCM_RegDecoderDestProc(enChanlID, func);
}
void mdrv_ppm_pcdev_ready(void)
{
}

unsigned int mdrv_diag_get_usb_type(void)
{
    return diag_get_usb_type();
}




