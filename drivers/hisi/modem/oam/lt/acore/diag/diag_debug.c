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

#include <mdrv.h>
#include "msp_debug.h"
#include "diag_debug.h"
#include "diag_api.h"
#include "diag_cfg.h"
#include "diag_msgmsp.h"
#include "msp.h"
#include "diag_common.h"
#include "nv_stru_drv.h"
#include "acore_nv_stru_drv.h"
#include "nv_stru_pam.h"
#include "nv_stru_sys.h"
#include "soc_socp_adapter.h"
#include "diag_acore_common.h"


#define    THIS_FILE_ID        MSP_FILE_ID_DIAG_DEBUG_C

#define    DIAG_LOG_PATH       MODEM_LOG_ROOT"/drv/DIAG/"

extern VOS_UINT8 g_EventModuleCfg[DIAG_CFG_PID_NUM];
VOS_UINT32 g_DiagDebugCfg = DIAG_CFG_SWT_CLOSE;
#define DIRPAH_LEN  64
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/

/*****************************************************************************
  3 Function
*****************************************************************************/

/*****************************************************************************
 Function Name   : CBT : Count Branch Timestamp (��������֧��ʱ�����λ����)
 Description     : ����ͳ�ƴ��������߷�֧�����ⶨλ
*****************************************************************************/

DIAG_CBT_INFO_TBL_STRU g_astCBTInfoTbl[EN_DIAG_DEBUG_INFO_MAX] = {{0}};

DIAG_CBT_INFO_TBL_STRU* diag_DebugGetInfo(VOS_VOID)
{
   return g_astCBTInfoTbl;
}

/*****************************************************************************
 Function Name   : diag_CBT
 Description     :
 Input           :DIAG_CBT_ID_ENUM ulType
                VOS_UINT32 ulRserved1
                VOS_UINT32 ulRserved2
                VOS_UINT32 ulRserved3
*****************************************************************************/
VOS_VOID diag_CBT(DIAG_CBT_ID_ENUM ulType,
                     VOS_UINT32 ulRserved1, VOS_UINT32 ulRserved2, VOS_UINT32 ulRserved3)
{
    g_astCBTInfoTbl[ulType].ulCalledNum += 1;
    g_astCBTInfoTbl[ulType].ulRserved1 = ulRserved1;
    g_astCBTInfoTbl[ulType].ulRserved2 = ulRserved2;
    g_astCBTInfoTbl[ulType].ulRserved3 = ulRserved3;
    g_astCBTInfoTbl[ulType].ulRtcTime  = VOS_GetTick();
}


/*****************************************************************************
 Function Name   : LNR : Last N Ring buffer store (���N����Ϣѭ���洢����)
 Description     : ���������N����Ϣ
*****************************************************************************/

DIAG_LNR_INFO_TBL_STRU g_astLNRInfoTbl[EN_DIAG_LNR_INFO_MAX] = {{0}};


VOS_VOID diag_LNR(DIAG_LNR_ID_ENUM ulType, VOS_UINT32 ulRserved1, VOS_UINT32 ulRserved2)
{
    g_astLNRInfoTbl[ulType].ulRserved1[g_astLNRInfoTbl[ulType].ulCur] = ulRserved1;
    g_astLNRInfoTbl[ulType].ulRserved2[g_astLNRInfoTbl[ulType].ulCur] = ulRserved2;
    g_astLNRInfoTbl[ulType].ulCur = (g_astLNRInfoTbl[ulType].ulCur+1)%DIAG_LNR_NUMBER;
}

VOS_VOID DIAG_ShowLNR(DIAG_LNR_ID_ENUM ulType, VOS_UINT32 n)
{
    VOS_UINT32 i;
    VOS_UINT32 cur;

    cur = (g_astLNRInfoTbl[ulType].ulCur + DIAG_LNR_NUMBER - n)%DIAG_LNR_NUMBER;

    for(i = 0; i <n; i++)
    {
        diag_crit("0x%x, 0x%x.\n", g_astLNRInfoTbl[ulType].ulRserved1[cur], g_astLNRInfoTbl[ulType].ulRserved2[cur]);
        cur = (cur + 1)%DIAG_LNR_NUMBER;
    }
}
/*****************************************************************************
 Function Name   : DIAG_ShowLogCfg
 Description     : �������������PID��ѯlog���ÿ�����Ϣ
*****************************************************************************/
VOS_VOID DIAG_ShowLogCfg(VOS_UINT32 ulModuleId)
{
    VOS_UINT32 level;

    /*���DIAG�Ƿ��ʼ����HSO�Ƿ�������*/
    if(!DIAG_IS_CONN_ON)
    {
        diag_crit("do not connect tool\n");
    }

    if(DIAG_CFG_MODULE_IS_INVALID((VOS_INT32)ulModuleId))
    {
        diag_crit("invalid PID. PID range(%d -- %d)\n", VOS_PID_DOPRAEND, VOS_PID_BUTT);
        return;
    }

    diag_crit("open global switch 0x%x(0xffffffff = invalid).\n", g_PrintTotalCfg);

    /* level�д洢��ֵ(0|ERROR|WARNING|NORMAL|INFO|0|0|0) bit 6-3 �ֱ��ʾERROR-INFO */
    level = g_PrintModuleCfg[ulModuleId - VOS_PID_DOPRAEND];
    if(level & 0x08)
    {
        diag_crit("PID %d print level: info.\n", ulModuleId);
    }
    else if(level & 0x10)
    {
        diag_crit("PID %d print level: normal.\n", ulModuleId);
    }
    else if(level & 0x20)
    {
        diag_crit("PID %d print level: warning.\n", ulModuleId);
    }
    else if(level & 0x40)
    {
        diag_crit("PID %d print level: error.\n", ulModuleId);
    }
    else
    {
        diag_crit("PID %d print level: off.\n", ulModuleId);
    }

    diag_crit("PRINT failed cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_PRINTFV_ERR].ulCalledNum);
    diag_crit("PRINT success cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_PRINTFV_OK].ulCalledNum);
}

/*****************************************************************************
 Function Name   : DIAG_ShowEventCfg
 Description     : ��ѯEVENT���ÿ�����Ϣ
*****************************************************************************/
VOS_VOID DIAG_ShowEventCfg(VOS_UINT32 ulpid)
{
    /*���DIAG�Ƿ��ʼ����HSO�Ƿ�������*/
    if(!DIAG_IS_CONN_ON)
    {
        diag_crit("do not connect tool\n");
    }
    else if(!DIAG_IS_EVENT_ON)
    {
        diag_crit("event global switch closed\n");
    }
    else
    {
        if(DIAG_CFG_MODULE_IS_INVALID(ulpid))
        {
            diag_crit("Invalid PID: A(%d -- %d), C(%d -- %d)!\n",
                VOS_PID_CPU_ID_1_DOPRAEND, VOS_CPU_ID_1_PID_BUTT,
                VOS_PID_CPU_ID_0_DOPRAEND, VOS_CPU_ID_0_PID_BUTT);
        }
        else
        {
            if(g_EventModuleCfg[ulpid - VOS_PID_DOPRAEND])
            {
                diag_crit("ulpid %d event switch opened\n", ulpid);
            }
            else
            {
                diag_crit("ulpid %d event switch closed\n", ulpid);
            }
        }
    }

    diag_crit("Event report failed cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_EVENT_ERR].ulCalledNum);
    diag_crit("Event report success cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_EVENT_OK].ulCalledNum);
}

/*****************************************************************************
 Function Name   : DIAG_ShowAirCfg
 Description     : ��ѯ�տ����ÿ�����Ϣ
*****************************************************************************/
VOS_VOID DIAG_ShowAirCfg(VOS_VOID)
{
    /*���DIAG�Ƿ��ʼ����HSO�Ƿ�������*/
    if(!DIAG_IS_CONN_ON)
    {
        diag_crit("do not connect tool\n");
    }
    else if(!DIAG_IS_LT_AIR_ON)
    {
        diag_crit("air global switch closed\n");
    }
    else
    {
        diag_crit("air global switch opend\n");
    }
    diag_crit("AIR report failed cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_AIR_ERR].ulCalledNum);
    diag_crit("AIR report success cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_AIR_OK].ulCalledNum);
}


/*****************************************************************************
 Function Name   : DIAG_ShowLayerCfg
 Description     : �������������PID��ѯ�����Ϣ���ÿ�����Ϣ
*****************************************************************************/
VOS_VOID DIAG_ShowLayerCfg(VOS_UINT32 ulModuleId, VOS_UINT32 ulSrcDst)
{
    VOS_UINT32 ulOffset = 0;
    VOS_UINT32 ulState = 0;

    /*���DIAG�Ƿ��ʼ����HSO�Ƿ�������*/
    if(!DIAG_IS_CONN_ON)
    {
        diag_crit("do not connect tool\n");
    }

    /* 0��ʾԴģ�� */
    if(DIAG_CMD_LAYER_MOD_SRC == ulSrcDst)
    {
        if(DIAG_CFG_LAYER_MODULE_IS_ACORE(ulModuleId))
        {
            ulOffset = DIAG_CFG_LAYER_MODULE_ACORE_OFFSET(ulModuleId);

            if(g_ALayerSrcModuleCfg[ulOffset])
            {
                ulState = 1;
            }
        }
        else if(DIAG_CFG_LAYER_MODULE_IS_CCORE(ulModuleId))
        {
            ulOffset = DIAG_CFG_LAYER_MODULE_CCORE_OFFSET(ulModuleId);

            if(g_CLayerSrcModuleCfg[ulOffset])
            {
                ulState = 1;
            }
        }
        else
        {
            diag_crit("Invalid PID: A(%d -- %d), C(%d -- %d)!\n",
                VOS_PID_CPU_ID_1_DOPRAEND, VOS_CPU_ID_1_PID_BUTT,
                VOS_PID_CPU_ID_0_DOPRAEND, VOS_CPU_ID_0_PID_BUTT);

            return ;
        }

        diag_crit("SRC module %d switch status is %s .\n", ulModuleId, ulState ? "Opend":"closed");
    }
    else
    {
        if(DIAG_CFG_LAYER_MODULE_IS_ACORE(ulModuleId))
        {
            ulOffset = DIAG_CFG_LAYER_MODULE_ACORE_OFFSET(ulModuleId);

            if(g_ALayerDstModuleCfg[ulOffset])
            {
                ulState = 1;
            }
        }
        else if(DIAG_CFG_LAYER_MODULE_IS_CCORE(ulModuleId))
        {
            ulOffset = DIAG_CFG_LAYER_MODULE_CCORE_OFFSET(ulModuleId);

            if(g_CLayerDstModuleCfg[ulOffset])
            {
                ulState = 1;
            }
        }
        else
        {
            diag_crit("Invalid PID: A(%d -- %d), C(%d -- %d)!\n",
                VOS_PID_CPU_ID_1_DOPRAEND, VOS_CPU_ID_1_PID_BUTT,
                VOS_PID_CPU_ID_0_DOPRAEND, VOS_CPU_ID_0_PID_BUTT);

            return ;
        }

        diag_crit("DST module %d switch status is %s .\n", ulModuleId, ulState ? "opened":"closed");
    }

    diag_crit("Layer report failed cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_TRACE_ERR].ulCalledNum);
    diag_crit("Layer report success cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_TRACE_OK].ulCalledNum);

    diag_crit("vos hook layer report failed cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_API_MSG_LAYER_ERR].ulCalledNum);
    diag_crit("vos hook layer report success cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_API_MSG_LAYER_OK].ulCalledNum);
    diag_crit("voshook layer report filtered cnt: %d: srcid 0x%x, dstid 0x%x, msgid 0x%x.\n",
        g_astCBTInfoTbl[EN_DIAG_API_MSG_LAYER_MATCH].ulCalledNum,
        g_astCBTInfoTbl[EN_DIAG_API_MSG_LAYER_MATCH].ulRserved1,
        g_astCBTInfoTbl[EN_DIAG_API_MSG_LAYER_MATCH].ulRserved2,
        g_astCBTInfoTbl[EN_DIAG_API_MSG_LAYER_MATCH].ulRserved3);
}

/*****************************************************************************
 Function Name   : DIAG_ShowUsrCfg
 Description     : ��ѯ�û������ÿ�����Ϣ
*****************************************************************************/
VOS_VOID DIAG_ShowUsrCfg(VOS_VOID)
{
    diag_crit("User Plane report failed cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_USER_ERR].ulCalledNum);
    diag_crit("User Plane report success cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_USER_OK].ulCalledNum);
}

/*****************************************************************************
 Function Name   : DIAG_ShowTrans
 Description     : ��ѯ���n��͸���ϱ������Ϣ
*****************************************************************************/
VOS_VOID DIAG_ShowTrans(VOS_UINT32 n)
{
    diag_crit("Trance report failed cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_TRANS_ERR].ulCalledNum);
    diag_crit("Trance report success cnt: %d.\n", g_astCBTInfoTbl[EN_DIAG_CBT_API_TRANS_OK].ulCalledNum);

    diag_crit("The last %d trance messages:\n", n);

    DIAG_ShowLNR(EN_DIAG_LNR_TRANS_IND, n);

}

/*****************************************************************************
 Function Name   : DIAG_ShowTrans
 Description     : ��ѯ���n��͸���ϱ������Ϣ
*****************************************************************************/
VOS_VOID DIAG_ShowPsTransCmd(VOS_UINT32 n)
{
    diag_crit("The last %d trance messages:\n", n);
    DIAG_ShowLNR(EN_DIAG_LNR_PS_TRANS, n);
}

extern HTIMER g_DebugTimer;

/*****************************************************************************
 Function Name   : diag_ReportMntn
 Description     : ͨ������ͨ����ʱ�ϱ���ά�ɲ���Ϣ
*****************************************************************************/
DIAG_MNTN_API_OK_STRU g_ind_src_mntn_info = {};
/*****************************************************************************
 Function Name   : diag_ReportMntn
 Description     : ͨ������ͨ����ʱ�ϱ���ά�ɲ���Ϣ
*****************************************************************************/
VOS_VOID diag_ReportSrcMntn(VOS_VOID)
{
    DIAG_DEBUG_SRC_MNTN_STRU    stDiagInfo = {};
    DIAG_DRV_DEBUG_INFO_STRU*   pstDebugInfo = NULL;
    VOS_UINT32          ulRet;
    static VOS_UINT32   last_slice = 0;
    VOS_UINT32          current_slice;

    stDiagInfo.pstMntnHead.ulModule       = DIAG_GEN_MODULE_EX(DIAG_MODEM_0, DIAG_MODE_LTE, DIAG_MSG_TYPE_BSP);
    stDiagInfo.pstMntnHead.ulPid          = DIAG_AGENT_PID;
    stDiagInfo.pstMntnHead.ulMsgId        = DIAG_DEBUG_AP_SRC_MNTN_CMDID;
    stDiagInfo.pstMntnHead.ulReserve      = 0;
    stDiagInfo.pstMntnHead.ulLength       = sizeof(stDiagInfo.pstMntnInfo);

    stDiagInfo.pstMntnInfo.ulChannelId  = SOCP_CODER_SRC_PS_IND;
    (VOS_VOID)VOS_MemCpy_s(stDiagInfo.pstMntnInfo.chanName, sizeof(stDiagInfo.pstMntnInfo.chanName), DIAG_SRC_NAME, sizeof(DIAG_SRC_NAME));
    current_slice = mdrv_timer_get_normal_timestamp();
    stDiagInfo.pstMntnInfo.ulDeltaTime  = DIAG_SLICE_DELTA(last_slice, current_slice);

    pstDebugInfo = mdrv_diag_get_mntn_info(DIAGLOG_SRC_MNTN);

    stDiagInfo.pstMntnInfo.ulPackageLen = pstDebugInfo->ulPackageLen;
    stDiagInfo.pstMntnInfo.ulPackageNum = pstDebugInfo->ulPackageNum;

    stDiagInfo.pstMntnInfo.ulAbandonLen = pstDebugInfo->ulAbandonLen;
    stDiagInfo.pstMntnInfo.ulAbandonNum = pstDebugInfo->ulAbandonNum;


	stDiagInfo.pstMntnInfo.ulThrputEnc  = ((((stDiagInfo.pstMntnInfo.ulPackageLen - stDiagInfo.pstMntnInfo.ulAbandonLen)/1024) * 32768)/stDiagInfo.pstMntnInfo.ulDeltaTime)*1024;

    stDiagInfo.pstMntnInfo.ulOverFlow50Num  = pstDebugInfo->ulOverFlow50Num;
    stDiagInfo.pstMntnInfo.ulOverFlow80Num  = pstDebugInfo->ulOverFlow80Num;

    /* ������Ϣ�ϱ����� */
    stDiagInfo.pstMntnInfo.ulTraceNum       = g_astCBTInfoTbl[EN_DIAG_CBT_API_TRACE_OK].ulCalledNum - g_ind_src_mntn_info.ulTraceNum;
    stDiagInfo.pstMntnInfo.ulLayerNum       = g_astCBTInfoTbl[EN_DIAG_API_MSG_LAYER_OK].ulCalledNum - g_ind_src_mntn_info.ulLayerNum;
    stDiagInfo.pstMntnInfo.ulEventNum       = g_astCBTInfoTbl[EN_DIAG_CBT_API_EVENT_OK].ulCalledNum - g_ind_src_mntn_info.ulEventNum;
    stDiagInfo.pstMntnInfo.ulLogNum         = g_astCBTInfoTbl[EN_DIAG_CBT_API_PRINTFV_OK].ulCalledNum - g_ind_src_mntn_info.ulLogNum;
    stDiagInfo.pstMntnInfo.ulAirNum         = g_astCBTInfoTbl[EN_DIAG_CBT_API_AIR_OK].ulCalledNum - g_ind_src_mntn_info.ulAirNum;
    stDiagInfo.pstMntnInfo.ulTransNum       = g_astCBTInfoTbl[EN_DIAG_CBT_API_TRANS_OK].ulCalledNum - g_ind_src_mntn_info.ulTransNum;
    stDiagInfo.pstMntnInfo.ulVolteNum       = g_astCBTInfoTbl[EN_DIAG_CBT_API_VOLTE_OK].ulCalledNum - g_ind_src_mntn_info.ulVolteNum;
    stDiagInfo.pstMntnInfo.ulUserNum        = g_astCBTInfoTbl[EN_DIAG_CBT_API_USER_OK].ulCalledNum - g_ind_src_mntn_info.ulUserNum;
    stDiagInfo.pstMntnInfo.ulLayerMatchNum  = g_astCBTInfoTbl[EN_DIAG_API_MSG_LAYER_MATCH].ulCalledNum - g_ind_src_mntn_info.ulLayerMatchNum;

    g_ind_src_mntn_info.ulTraceNum       = stDiagInfo.pstMntnInfo.ulTraceNum;
    g_ind_src_mntn_info.ulLayerNum       = stDiagInfo.pstMntnInfo.ulLayerNum;
    g_ind_src_mntn_info.ulEventNum       = stDiagInfo.pstMntnInfo.ulEventNum;
    g_ind_src_mntn_info.ulLogNum         = stDiagInfo.pstMntnInfo.ulLogNum;
    g_ind_src_mntn_info.ulAirNum         = stDiagInfo.pstMntnInfo.ulAirNum;
    g_ind_src_mntn_info.ulTransNum       = stDiagInfo.pstMntnInfo.ulTransNum;
    g_ind_src_mntn_info.ulVolteNum       = stDiagInfo.pstMntnInfo.ulVolteNum;
    g_ind_src_mntn_info.ulUserNum        = stDiagInfo.pstMntnInfo.ulUserNum;
    g_ind_src_mntn_info.ulLayerMatchNum  = stDiagInfo.pstMntnInfo.ulLayerMatchNum;

    stDiagInfo.pstMntnHead.pData        = (void *)(&stDiagInfo.pstMntnInfo);
    ulRet = mdrv_diag_report_trans((DRV_DIAG_TRANS_IND_STRU *)&stDiagInfo);
    if(!ulRet)
    {
        last_slice = current_slice;
        /*���ͳɹ���������ؼ�¼*/
        mdrv_diag_reset_mntn_info(DIAGLOG_SRC_MNTN);
    }
    return;
}


VOS_VOID diag_ReportDstMntn(VOS_VOID)
{
    DIAG_DEBUG_DST_LOST_STRU    stDiagInfo = {};
    SOCP_BUFFER_RW_STRU         stSocpBuff = {NULL};
    DIAG_MNTN_DST_INFO_STRU     *pstDstInfo;
    VOS_UINT32                  ulRet;
    VOS_UINT32                  current_slice = 0;
    static  VOS_UINT32          last_slice = 0;

    pstDstInfo = (DIAG_MNTN_DST_INFO_STRU *)mdrv_diag_get_mntn_info(DIAGLOG_DST_MNTN);

    mdrv_SocpEncDstQueryIntInfo(&(pstDstInfo->aulSocpTrfInfo), &(pstDstInfo->aulSocpThrOvfInfo));
	mdrv_PPM_QueryUsbInfo(&(pstDstInfo->aulPpmUsbInfo), sizeof(OM_PPM_USB_DEBUG_INFO));
    (VOS_VOID)VOS_MemCpy_s(&stDiagInfo.pstMntnInfo, sizeof(stDiagInfo.pstMntnInfo), pstDstInfo, sizeof(*pstDstInfo));

    stDiagInfo.pstMntnInfo.ulChannelId = SOCP_CODER_DST_OM_IND;
    (VOS_VOID)VOS_MemCpy_s(stDiagInfo.pstMntnInfo.chanName, sizeof(stDiagInfo.pstMntnInfo.chanName), "ind_dst", sizeof("ind_dst"));

    (void)DRV_SOCP_GET_READ_BUFF(SOCP_CODER_DST_OM_IND, &stSocpBuff);
    stDiagInfo.pstMntnInfo.ulUseSize = stSocpBuff.u32RbSize + stSocpBuff.u32Size;

    current_slice = mdrv_timer_get_normal_timestamp();
    stDiagInfo.pstMntnInfo.ulDeltaTime = DIAG_SLICE_DELTA(last_slice, current_slice);

    stDiagInfo.pstMntnInfo.ulThrputPhy      = ((mdrv_GetThrputInfo(EN_DIAG_THRPUT_DATA_CHN_PHY)/1024)*1000)/((stDiagInfo.pstMntnInfo.ulDeltaTime*1000)/32768);
    stDiagInfo.pstMntnInfo.ulThrputCb       = ((mdrv_GetThrputInfo(EN_DIAG_THRPUT_DATA_CHN_CB)/1024)*1000)/((stDiagInfo.pstMntnInfo.ulDeltaTime*1000)/32768);

    stDiagInfo.pstMntnHead.ulModule       = DIAG_GEN_MODULE_EX(DIAG_MODEM_0, DIAG_MODE_LTE, DIAG_MSG_TYPE_BSP);
    stDiagInfo.pstMntnHead.ulPid          = DIAG_AGENT_PID;
    stDiagInfo.pstMntnHead.ulMsgId        = DIAG_DEBUG_DST_MNTN_CMDID;
    stDiagInfo.pstMntnHead.ulLength       = sizeof(stDiagInfo.pstMntnInfo);

    stDiagInfo.pstMntnHead.pData          = (void *)(&stDiagInfo.pstMntnInfo);

    ulRet = mdrv_diag_report_trans((DRV_DIAG_TRANS_IND_STRU *)&stDiagInfo);
    if(!ulRet)
    {
        /*���ͳɹ���������ؼ�¼*/
        mdrv_diag_reset_mntn_info(DIAGLOG_DST_MNTN);
	    last_slice = current_slice;
        mdrv_clear_socp_encdst_int_info();
		mdrv_PPM_ClearUsbTimeInfo();
    }
    return;
}

VOS_VOID diag_ReportMntn(VOS_VOID)
{
    /* ����log */
    if(!DIAG_IS_POLOG_ON)
    {
        /* HIDSδ���� */
        if(!DIAG_IS_CONN_ON)
        {
            return;
        }
    }

    /*Դ��ά����Ϣ�ϱ�*/
    diag_ReportSrcMntn();

    /*Ŀ�Ķ�ά����Ϣ�ϱ�*/
    diag_ReportDstMntn();

    return;
}

VOS_VOID diag_StopMntnTimer(VOS_VOID)
{
    /* ɾ����ʱ�� */
    if(DIAG_CFG_SWT_CLOSE == g_DiagDebugCfg)
    {
        diag_info("mntn is not active\n");
        return;
    }

    g_DiagDebugCfg = DIAG_CFG_SWT_CLOSE;

    (VOS_VOID)VOS_StopRelTimer(&g_DebugTimer);
}

VOS_VOID diag_StartMntnTimer(VOS_UINT32 ulMntnReportTime)
{
    VOS_UINT32          ulCnfRst = 0;

    if(DIAG_CFG_SWT_OPEN == g_DiagDebugCfg)
    {
		diag_StopMntnTimer();
    }

    g_DiagDebugCfg = DIAG_CFG_SWT_OPEN;

    ulCnfRst = VOS_StartRelTimer(&g_DebugTimer, MSP_PID_DIAG_APP_AGENT, ulMntnReportTime, DIAG_DEBUG_TIMER_NAME, \
                            DIAG_DEBUG_TIMER_PARA, VOS_RELTIMER_LOOP, VOS_TIMER_NO_PRECISION);
    if(ulCnfRst != ERR_MSP_SUCCESS)
    {
        diag_error("start dbug timer fail\n");
    }
}

/*****************************************************************************
 Function Name   : diag_UserPlaneCfgProc
 Description     : deal witch user plane msg cfg
 Input           : pstReq ����������
 Output          : None
 Return          : VOS_UINT32
*****************************************************************************/
VOS_UINT32 diag_MntnCfgProc(VOS_UINT8* pstReq)
{
    DIAG_CMD_LOG_CAT_DIAG_MNTN_CNF_STRU stDebugCnf = {0};
    DIAG_CMD_LOG_DIAG_MNTN_REQ_STRU*    pstDebugReq = NULL;
    DIAG_FRAME_INFO_STRU*               pstDiagHead = NULL;
    MSP_DIAG_CNF_INFO_STRU              stDiagInfo = {0};
    DIAG_MSG_A_TRANS_C_STRU*            pstInfo;
    VOS_UINT32  ret;
    VOS_UINT32  ulLen;

    pstDiagHead = (DIAG_FRAME_INFO_STRU*)(pstReq);

    if(pstDiagHead->ulMsgLen < sizeof(MSP_DIAG_DATA_REQ_STRU) + sizeof(DIAG_CMD_LOG_DIAG_MNTN_REQ_STRU))
    {
        diag_error("len error:0x%x\n", pstDiagHead->ulMsgLen);
        ret = ERR_MSP_INALID_LEN_ERROR;
        goto DIAG_ERROR;
    }

    pstDebugReq = (DIAG_CMD_LOG_DIAG_MNTN_REQ_STRU*)(pstReq + DIAG_MESSAGE_DATA_HEADER_LEN);

    if(DIAG_CFG_SWT_CLOSE == pstDebugReq->usMntnSwitch)
    {
        diag_info("switch to close\n");
        /* ɾ����ʱ�� */
        diag_StopMntnTimer();
    }
    else
    {
        if(pstDebugReq->usMntnTime == 0)
        {
            pstDebugReq->usMntnTime = 0x1388;  // ��������·�ά������Ϊ0��MSP�޸�ΪĬ��ֵ = 5s
        }
        diag_info("switch to open, Mntn Cycle = %d ms\n", pstDebugReq->usMntnTime);
        diag_StartMntnTimer(pstDebugReq->usMntnTime);
    }

    DIAG_MSG_ACORE_CFG_PROC(ulLen, pstDiagHead, pstInfo, ret);
    DIAG_MSG_SEND_CFG_TO_NRM(ulLen, pstDiagHead, pstInfo, ret);

    ret = ERR_MSP_SUCCESS;
DIAG_ERROR:
    DIAG_MSG_COMMON_PROC(stDiagInfo, stDebugCnf, pstDiagHead);
    stDiagInfo.ulMsgType = DIAG_MSG_TYPE_MSP;
    stDebugCnf.ulRc = ret;

    /*�����FW�ظ�*/
    ret = DIAG_MsgReport(&stDiagInfo, &stDebugCnf, (VOS_UINT32)sizeof(stDebugCnf));
    return (VOS_UINT32)ret;
}


/* EVENT�ϱ�����ӿ� */
VOS_VOID DIAG_DebugEventReport(VOS_UINT32 ulpid)
{
    DIAG_EVENT_IND_STRU stEvent = {0};
    char aulData[16] = "event";

    stEvent.ulModule    = DIAG_GEN_MODULE(1, 2);
    stEvent.ulPid       = ulpid;
    stEvent.ulEventId   = 0x13579;
    stEvent.ulLength    = 16;
    stEvent.pData       = aulData;

    (VOS_VOID)DIAG_EventReport(&stEvent);
}


/* �����Ϣ�ϱ�����ӿ� */
VOS_VOID DIAG_DebugLayerReport(VOS_UINT32 ulsndpid, VOS_UINT32 ulrcvpid, VOS_UINT32 ulMsg)
{
    DIAG_DATA_MSG_STRU *pDataMsg;
    char aulData[16] = "layer";

    diag_crit("pid %d send to %d, msgid 0x%x.\n", ulsndpid, ulsndpid, ulMsg);

    pDataMsg = (DIAG_DATA_MSG_STRU*)VOS_AllocMsg(ulsndpid, (sizeof(DIAG_DATA_MSG_STRU)+ 16 - VOS_MSG_HEAD_LENGTH));

    if (pDataMsg != NULL)
    {
        pDataMsg->ulReceiverPid = ulrcvpid;
        pDataMsg->ulMsgId       = ulMsg;
        pDataMsg->ulLen         = 16;
        (VOS_VOID)VOS_MemCpy_s(pDataMsg->pContext,pDataMsg->ulLen,aulData,sizeof(aulData));

        DIAG_TraceReport(pDataMsg);

        (VOS_VOID)VOS_FreeMsg(ulsndpid, pDataMsg);
    }
}

/* �����Ϣ�ϱ�����ӿ� */
VOS_VOID DIAG_DebugVosLayerReport(VOS_UINT32 ulsndpid, VOS_UINT32 ulrcvpid, VOS_UINT32 ulMsg)
{
    DIAG_DATA_MSG_STRU *pDataMsg;
    char aulData[16] = "vos layer";

    diag_crit("pid %d send to %d, msgid 0x%x.\n", ulsndpid, ulsndpid, ulMsg);

    pDataMsg = (DIAG_DATA_MSG_STRU*)VOS_AllocMsg(ulsndpid, (sizeof(DIAG_DATA_MSG_STRU)+ 16 - VOS_MSG_HEAD_LENGTH));

    if (pDataMsg != NULL)
    {
        pDataMsg->ulReceiverPid = ulrcvpid;
        pDataMsg->ulMsgId       = ulMsg;
        pDataMsg->ulLen         = 16;
        (VOS_VOID)VOS_MemCpy_s(pDataMsg->pContext,pDataMsg->ulLen,aulData,sizeof(aulData));

        DIAG_LayerMsgReport(pDataMsg);

        (VOS_VOID)VOS_FreeMsg(ulsndpid, pDataMsg);
    }
}


/* log�ϱ�����ӿ� */
VOS_VOID DIAG_DebugLogReport(VOS_UINT32 ulpid, VOS_UINT32 level)
{
    VOS_UINT32 ulMod = DIAG_GEN_LOG_MODULE(1, 2, level);
    (VOS_VOID)DIAG_LogReport(ulMod, ulpid, __FILE__, __LINE__, "DIAG TEST.\n");
}


/* ͸���ϱ�����ӿ� */
VOS_VOID DIAG_DebugTransReport(VOS_UINT32 ulpid)
{
    DIAG_TRANS_IND_STRU std;
    char aulData[80] = "trans";

    std.ulModule    = DIAG_GEN_MODULE(1, 2);;
    std.ulPid       = ulpid;
    std.ulMsgId     = 0x9753;
    std.ulLength    = 80;
    std.pData       = aulData;

    (VOS_VOID)DIAG_TransReport(&std);
}


/* ��俪�ص���ӿ� */
VOS_VOID DIAG_DebugLayerCfg(VOS_UINT32 ulModuleId, VOS_UINT8 ucFlag)
{
    VOS_UINT32 ulOffset = 0;

    if(DIAG_CFG_LAYER_MODULE_IS_ACORE(ulModuleId))
    {
        ulOffset = DIAG_CFG_LAYER_MODULE_ACORE_OFFSET(ulModuleId);

        g_ALayerSrcModuleCfg[ulOffset] = ucFlag;
        g_ALayerDstModuleCfg[ulOffset] = ucFlag;
    }
    else if(DIAG_CFG_LAYER_MODULE_IS_CCORE(ulModuleId))
    {
        ulOffset = DIAG_CFG_LAYER_MODULE_CCORE_OFFSET(ulModuleId);

        g_CLayerSrcModuleCfg[ulOffset] = ucFlag;
        g_CLayerDstModuleCfg[ulOffset] = ucFlag;
    }
    else
    {
        diag_crit("Invalid PID: A(%d -- %d), C(%d -- %d)!\n",
            VOS_PID_CPU_ID_1_DOPRAEND, VOS_CPU_ID_1_PID_BUTT,
            VOS_PID_CPU_ID_0_DOPRAEND, VOS_CPU_ID_0_PID_BUTT);

        return ;
    }

}

#define GEN_MODULE_ID(modem_id, modeid, level, pid)     ((modem_id<<24)|(modeid<<16)|(level << 12)|(pid))

VOS_UINT32 DIAG_ApiTest(VOS_UINT8* pstReq)
{
    DIAG_MSG_A_TRANS_C_STRU *pstInfo;
    DIAG_AIR_IND_STRU       airmsg;
    VOS_UINT32              data = 0x1234567;
    VOS_UINT32              ulModuleId = 0;
    VOS_UINT32              ulLen;
    DIAG_FRAME_INFO_STRU    *pstDiagHead = VOS_NULL;
    VOS_VOID                *ptr;

    diag_crit("DIAG_ApiTest start\n");

    ptr = VOS_MemAlloc(MSP_PID_DIAG_APP_AGENT, DYNAMIC_MEM_PT, 9*1024);
    if(!ptr)
    {
        diag_error("alloc fail\n");
    }

    (VOS_VOID)DIAG_DebugLogReport(MSP_PID_DIAG_APP_AGENT, 1);

    (VOS_VOID)DIAG_DebugTransReport(MSP_PID_DIAG_APP_AGENT);
    (VOS_VOID)DIAG_TransReport(VOS_NULL);

    (VOS_VOID)DIAG_DebugEventReport(MSP_PID_DIAG_APP_AGENT);
    (VOS_VOID)DIAG_EventReport(VOS_NULL);

    ulModuleId = GEN_MODULE_ID(0, 1, 1, MSP_PID_DIAG_APP_AGENT);
    airmsg.ulPid = MSP_PID_DIAG_APP_AGENT;
    airmsg.ulDirection = 0x3;
    airmsg.ulLength = sizeof(data);
    airmsg.ulModule = ulModuleId;
    airmsg.ulMsgId = 0x123;
    airmsg.pData = (VOS_VOID *)&data;
    (VOS_VOID)DIAG_AirMsgReport(&airmsg);

    if(ptr)
    {
        airmsg.ulPid = MSP_PID_DIAG_APP_AGENT;
        airmsg.ulDirection = 0x3;
        airmsg.ulLength = 9*1024;
        airmsg.ulModule = ulModuleId;
        airmsg.ulMsgId = 0x123;
        airmsg.pData = (VOS_VOID *)&data;
        (VOS_VOID)DIAG_AirMsgReport(&airmsg);
    }

    (VOS_VOID)DIAG_AirMsgReport(VOS_NULL);

    (VOS_VOID)DIAG_DebugLayerReport(DOPRA_PID_TIMER, MSP_PID_DIAG_APP_AGENT, 0x123);
    (VOS_VOID)DIAG_TraceReport(VOS_NULL);

    (VOS_VOID)DIAG_DebugVosLayerReport(DOPRA_PID_TIMER, MSP_PID_DIAG_APP_AGENT, 0x123);
    (VOS_VOID)DIAG_LayerMsgReport(VOS_NULL);

    (VOS_VOID)DIAG_ErrorLog(VOS_NULL, 0, 0, 0, VOS_NULL, 0);

    VOS_MemFree(MSP_PID_DIAG_APP_AGENT, ptr);

    /* �޸ĺ�ӿ�Ҳ֧���ڴ��ڵ��� */
    if(pstReq)
    {
        pstDiagHead = (DIAG_FRAME_INFO_STRU*)(pstReq);

        DIAG_MSG_ACORE_CFG_PROC(ulLen, pstDiagHead, pstInfo, data);
        DIAG_MSG_SEND_CFG_TO_NRM(ulLen, pstDiagHead, pstInfo, data);
    }

    diag_crit("DIAG_ApiTest end\n");

    return VOS_OK;
DIAG_ERROR:
    return 0;
}

EXPORT_SYMBOL(DIAG_ApiTest);


