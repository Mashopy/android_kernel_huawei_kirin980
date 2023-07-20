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



#ifndef __DIAG_DEBUG_H__
#define __DIAG_DEBUG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "product_config.h"
#include "mdrv.h"
#include "vos.h"
#include "msp_errno.h"
#include "msp_diag_comm.h"
#include "diag_frame.h"
#include "diag_cfg_comm.h"


#pragma pack(4)

typedef enum
{
   DIAG_DEBUG_API = 0,
   DIAG_DEBUG_API_AIR,
   DIAG_DEBUG_API_TRACE,
   DIAG_DEBUG_API_USERPLANE,
   DIAG_DEBUG_API_EVENT,
   DIAG_DEBUG_API_PRINT,
   DIAG_DEBUG_API_VOLTE,
   DIAG_DEBUG_API_TRANS,
   DIAG_DEBUG_API_DEFAULT
}DIAG_DEBUG_API_ENUM;


/* CBT ***********************************************************************/

#define DIAG_DEBUG_SDM_FUN(enType,ulRserved1,ulRserved2,ulRserved3)  \
            diag_CBT(enType,ulRserved1,ulRserved2,ulRserved3)

typedef enum
{
    EN_DIAG_CBT_MSGMSP_CMD_ENTER,
    EN_DIAG_CBT_MSGMSP_TRANS_ENTER,
    EN_DIAG_CBT_MSGMSP_SYS_ENTER,
    EN_DIAG_CBT_MSGMSP_DFLT_ENTER,

    EN_DIAG_CBT_MSGMSP_TRANS_REQ,
    EN_DIAG_CBT_MSGMSP_TRANS_CNF,

    EN_DIAG_CBT_API_PRINTFV_OK,
    EN_DIAG_CBT_API_PRINTFV_ERR,

    EN_DIAG_CBT_API_EVENT_OK,
    EN_DIAG_CBT_API_EVENT_ERR,

    EN_DIAG_CBT_API_AIR_OK,
    EN_DIAG_CBT_API_AIR_ERR,

    EN_DIAG_CBT_API_TRACE_OK,
    EN_DIAG_CBT_API_TRACE_ERR,

    EN_DIAG_API_MSG_LAYER_OK,
    EN_DIAG_API_MSG_LAYER_ERR,
    EN_DIAG_API_MSG_LAYER_MATCH,
    EN_DIAG_API_MSG_LAYER_NOTIFY,

    EN_DIAG_CBT_API_TRACE_FILTER,
    EN_DIAG_CBT_API_TRACE_MATCH,

    EN_DIAG_CBT_API_USER_OK,
    EN_DIAG_CBT_API_USER_ERR,

    EN_DIAG_CBT_API_PACKET_ERR_REQ,

    EN_DIAG_CBT_API_TRANS_OK,
    EN_DIAG_CBT_API_TRANS_ERR,

    EN_DIAG_CBT_API_VOLTE_ERR,
    EN_DIAG_CBT_API_VOLTE_OK,

    EN_DIAG_DEBUG_CODE_PACKET_START,
    EN_DIAG_DEBUG_CODE_PACKET_START_ERROR,

    EN_DIAG_DEBUG_GET_SRC_BUF_START,
    EN_DIAG_DEBUG_GET_SRC_BUF_START_ERROR,

    EN_SDM_DIAG_DOT,
    EN_SDM_DIAG_DOT_ERR,
    EN_SDM_DIAG_DOT_OK,

    EN_DIAG_DEBUG_MSG,
    EN_DIAG_DEBUG_MSG_ERR,
    EN_DIAG_DEBUG_CONN_CFG,
    EN_DIAG_DEBUG_DIS_CONN_CFG,
    EN_DIAG_DEBUG_LAYER_CFG,
    EN_DIAG_DEBUG_PRINT_CFG,
    EN_DIAG_DEBUG_EVENT_CFG,
    EN_DIAG_DEBUG_AIR_CFG,
	EN_DIAG_DEBUG_USEPLANE_CFG,

    EN_DIAG_DEBUG_AGENT_INIT,
    EN_DIAG_DEBUG_AGENT_INIT_ERROR,
    EN_DIAG_DEBUG_AGENT_DISPATCH_CMD,
    EN_DIAG_DEBUG_REG_RD,
    EN_DIAG_DEBUG_REG_WR,
    EN_DIAG_DEBUG_BBP_LOG,
    EN_DIAG_DEBUG_BBP_SAMPLE,
    EN_DIAG_DEBUG_LTE_DSP_CNF,
    EN_DIAG_DEBUG_TDS_DSP_CNF,

    EN_DIAG_DEBUG_BBP_AGENT_TIME_OUT_ENTRY,
    /* DIAG AGENT��LDSP���佻�� */
    EN_DIAG_AGENT_LDSP_MB_MSG,

    EN_DIAG_CBT_API_DT_ERR,
    EN_DIAG_CBT_API_DT_OK,

    EN_DIAG_DEBUG_INFO_MAX
} DIAG_CBT_ID_ENUM;

/*����������λʱMSP����Ĳ���ID��СֵΪ:0xB000000*/
#define DIAG_BASE_ERROR_NUMBER          (0xB0000000)

enum MSP_SYSTEMERROR_MODID_ENUM
{
    DIAG_ERROR_MODID_BASE       = (0xB0000000),
    DIAG_CALLED_IN_IRQ          = DIAG_ERROR_MODID_BASE,
    DIAG_ERROR_MODID_OVERFLOW,
    DIAG_ERROR_MODID_BUTT       = (0xB0000010)
};

typedef struct
{
    VOS_UINT32 ulCalledNum;  /* ���ô���������Ϣ����,���߱����ú����Ƿ񱻵���*/
    VOS_UINT32 ulRserved1;
    VOS_UINT32 ulRserved2;
    VOS_UINT32 ulRserved3;
    VOS_UINT32 ulRtcTime;
} DIAG_CBT_INFO_TBL_STRU;

typedef struct
{
    VOS_CHAR                pucApiName[8];
    DIAG_CBT_ID_ENUM        ulOk;
    DIAG_CBT_ID_ENUM        ulErr;
    VOS_UINT32              ulSlice;
    VOS_UINT32              ulOkNum;
    VOS_UINT32              ulErrNum;
}DIAG_DEBUG_API_INFO_STRU;


extern VOS_VOID diag_CBT(DIAG_CBT_ID_ENUM ulType, VOS_UINT32 ulRserved1,
                        VOS_UINT32 ulRserved2, VOS_UINT32 ulRserved3);


/* LNR ***********************************************************************/

#define DIAG_LNR_NUMBER             (100)

typedef enum
{
    EN_DIAG_LNR_CMDID = 0,  /* ���N�����յ���������� */

    EN_DIAG_LNR_LOG_LOST,   /* ���N��log��ʧ�ļ�¼ */

    EN_DIAG_LNR_PS_TRANS,   /* ���N��PS͸������ļ�¼ */

    EN_DIAG_LNR_CCORE_MSG,  /* ���N��C�˴�A���յ�����ϢID�ļ�¼ */

    EN_DIAG_LNR_MESSAGE_RPT,/* ���N��ͨ��messageģ��report�ϱ���cmdid�ļ�¼ */

    EN_DIAG_LNR_TRANS_IND,  /* ���N��͸���ϱ��ļ�¼ */

    EN_DIAG_LNR_INFO_MAX
} DIAG_LNR_ID_ENUM;

typedef struct
{
    VOS_UINT32 ulCur;
    VOS_UINT32 ulRserved1[DIAG_LNR_NUMBER];
    VOS_UINT32 ulRserved2[DIAG_LNR_NUMBER];
} DIAG_LNR_INFO_TBL_STRU;


extern VOS_VOID diag_LNR(DIAG_LNR_ID_ENUM ulType, VOS_UINT32 ulRserved1, VOS_UINT32 ulRserved2);

/* DIAG lost ����Ϊ������ǰ�Ľṹ����*******************************************/

typedef struct
{
    VOS_UINT32      ulModuleId;
    VOS_UINT32      ulNo;                   /* ��� */

    VOS_UINT32      ulLostTotalNum;         /* �����ܴ��� */

    VOS_UINT32      aulCurFailNum[6];    /* ��ǰʱ����ڶ����ĸ������ */

    VOS_UINT32      ulSrcChnSize;       /* ����Դͨ��ʣ���ֽ��� */
    VOS_UINT32      ulDstChnSize;       /* ����Ŀ��ͨ��ʣ���ֽ��� */
    VOS_UINT32      ulDSPChnSize;       /* DSP����Դͨ��ʣ���ֽ��� */

    VOS_UINT32      ulThrputPhy;        /* ����ͨ�������� */
    VOS_UINT32      ulThrputCb;         /* �ص������� */

    VOS_UINT32      ulTotalSocpDstLen;  /* ��SOCP����Ŀ��buffer�ж�ȡ�����ֽ��� */

    VOS_UINT32      ulTotalUSBLen;      /* ��USBд������ֽ��� */
    VOS_UINT32      ulTotalUSBFreeLen;  /* USB�ص��ͷŵ����ֽ��� */
    VOS_UINT32      ulTotalVCOMLen;     /* ��VCOMд������ֽ��� */
    VOS_UINT32      ulTotalVCOMErrLen;  /* ��VCOMд��ʧ�ܵ����ֽ��� */
    VOS_UINT32      ulVCOMMaxTimeLen;   /* ����VCOMд�ӿڻ��ѵ����ʱ�� */

    VOS_UINT32      ulTotalSOCKETLen;   /* ��SOCKETд������ֽ��� */

    VOS_UINT32      aulRreserve[4];     /* Ԥ�� */

    VOS_UINT32      aulToolreserve[4];  /* ������Ԥ����16���ֽڣ������ڹ�������ʾ���ߵ�ά����Ϣ */
}DIAG_LOST_INFO_STRU;

typedef struct
{
    VOS_UINT32      ulTraceNum;         /* �����Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulEventNum;         /* event�ɹ��ϱ����� */
    VOS_UINT32      ulLogNum;           /* Log�ɹ��ϱ����� */
    VOS_UINT32      ulAirNum;           /* �տڳɹ��ϱ����� */
    VOS_UINT32      ulTransNum;         /* ͸���ɹ��ϱ����� */

    VOS_UINT32      ulThrputEnc;        /* ����Դ������ */

    VOS_UINT32      ulThrputPhy;        /* ����ͨ�������� */
    VOS_UINT32      ulThrputCb;         /* �ص������� */

    VOS_UINT32      ulSrcChnSize;       /* ����Դͨ��ʣ���ֽ��� */
    VOS_UINT32      ulDstChnSize;       /* ����Ŀ��ͨ��ʣ���ֽ��� */

    VOS_UINT32      ulTotalLostTimes;   /* �������� */

    VOS_UINT32      ulTotalUSBLen;      /* ��USBд������ֽ��� */
    VOS_UINT32      ulTotalUSBFreeLen;  /* USB�ص��ͷŵ����ֽ��� */
    VOS_UINT32      ulTotalVCOMLen;     /* ��VCOMд������ֽ��� */
    VOS_UINT32      ulTotalVCOMErrLen;  /* ��VCOMд��ʧ�ܵ����ֽ��� */
    VOS_UINT32      ulVCOMMaxTimeLen;   /* ����VCOMд�ӿڻ��ѵ����ʱ�� */

    VOS_UINT32      ulTotalSOCKETLen;   /* ��SOCKETд������ֽ��� */

    VOS_UINT32      aulReserve[4];      /* Ԥ�� */

    VOS_UINT32      aulToolreserve[12]; /* ������Ԥ����64���ֽڣ������ڹ�������ʾ���ߵ�ά����Ϣ */
}DIAG_MNTN_INFO_STRU;


/* DIAG MNTN *******************************************/

#define DIAG_DEBUG_LOST_LEN             (5*32768)

#define DIAG_DEBUG_LOST_MINUTE          (60*32768)

#define DIAG_SLICE_DELTA(begin,end) (((end)>(begin))?((end)-(begin)):((0xFFFFFFFF-(begin))+(end)))

#if defined(__KERNEL__)
#define DIAG_SRC_NAME   "ap_diag"
#elif defined(__OS_LRCCPU__)
#define DIAG_SRC_NAME   "lr_diag"
#elif defined(__OS_NRCCPU__)
#define DIAG_SRC_NAME   "nr_diag"
#endif

typedef struct
{
    VOS_UINT32      ulModuleId;
    VOS_UINT32      ulNo;               /* ��� */
}DIAG_MNTN_HEAD_STRU;

typedef struct
{
    DIAGLOG_MNTN_SRC_INFO
     /* acore diag mntn info */
    VOS_UINT32      ulTraceNum;         /* �����Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulLayerNum;         /* OSA���������Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulEventNum;         /* event�ɹ��ϱ����� */
    VOS_UINT32      ulLogNum;           /* Log�ɹ��ϱ����� */
    VOS_UINT32      ulAirNum;           /* �տڳɹ��ϱ����� */
    VOS_UINT32      ulTransNum;         /* ͸���ɹ��ϱ����� */
    VOS_UINT32      ulVolteNum;         /* Volte��Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulUserNum;          /* Volte��Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulLayerMatchNum;    /* �������������Ϣ�����˴��� */
}DIAG_MNTN_SRC_INFO_STRU;


typedef struct
{
    VOS_UINT32      ulTraceNum;         /* �����Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulLayerNum;         /* OSA���������Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulEventNum;         /* event�ɹ��ϱ����� */
    VOS_UINT32      ulLogNum;           /* Log�ɹ��ϱ����� */
    VOS_UINT32      ulAirNum;           /* �տڳɹ��ϱ����� */
    VOS_UINT32      ulTransNum;         /* ͸���ɹ��ϱ����� */
    VOS_UINT32      ulVolteNum;         /* Volte��Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulUserNum;          /* Volte��Ϣ�ɹ��ϱ����� */
    VOS_UINT32      ulLayerMatchNum;    /* �������������Ϣ�����˴��� */
}DIAG_MNTN_API_OK_STRU;


typedef struct
{
    DIAG_TRANS_IND_STRU                 pstMntnHead;
    DIAG_MNTN_SRC_INFO_STRU             pstMntnInfo;
}DIAG_DEBUG_SRC_MNTN_STRU;

#if (VOS_OS_VER == VOS_LINUX)

typedef struct
{
    DIAG_TRANS_IND_STRU         pstMntnHead;
    DIAG_MNTN_DST_INFO_STRU     pstMntnInfo;
}DIAG_DEBUG_DST_LOST_STRU;
#endif
/* DIAG_TRACE *******************************************/

#define DIAG_DEBUG_TRACE_DELAY          (1*32768)
#define DIAG_DEBUG_TXT_LOG_DELAY        (10*32768)
#define DIAG_DEBUG_TXT_LOG_LENGTH       256
typedef struct
{
    VOS_UINT32      ulModuleId;
    VOS_UINT32      ulLevel;
    VOS_UINT32      ulSn;
    VOS_CHAR        pucData[DIAG_DEBUG_TXT_LOG_LENGTH];
}DIAG_DEBUG_INFO_STRU;

typedef struct
{
    DIAG_FRAME_INFO_STRU                pstFrameInfo;
    DIAG_DEBUG_INFO_STRU                pstInfo;
}DIAG_DEBUG_FRAME_STRU;

/* A����C�˷��͵�debug��Ϣ *******************************************/

#define DIAG_DEBUG_DFR_BIT          0x00000001      /* �������� */
#define DIAG_DEBUG_NIL_BIT          0x00000002      /* ����log���ϱ��Ķ�λ��Ϣ */

typedef struct
{
    VOS_UINT32    ulSenderCpuId;
    VOS_UINT32    ulSenderPid;
    VOS_UINT32    ulReceiverCpuid;
    VOS_UINT32    ulReceiverPid;
    VOS_UINT32    ulLength;
    VOS_UINT32    ulMsgId;

    /* bit0 : DFR */
    /* bit1 : no ind log */
    VOS_UINT32      ulFlag;
} DIAG_A_DEBUG_C_REQ_STRU;


/* ���⺯���ӿ� *******************************************/
DIAG_CBT_INFO_TBL_STRU* diag_DebugGetInfo(VOS_VOID);
VOS_VOID DIAG_ShowLNR(DIAG_LNR_ID_ENUM ulType, VOS_UINT32 n);
VOS_VOID DIAG_ShowLogCfg(VOS_UINT32 ulModuleId);
VOS_VOID DIAG_ShowEventCfg(VOS_UINT32 ulpid);
VOS_VOID DIAG_ShowAirCfg(VOS_VOID);
VOS_VOID DIAG_ShowLayerCfg(VOS_UINT32 ulModuleId, VOS_UINT32 ulSrcDst);
VOS_VOID DIAG_ShowUsrCfg(VOS_VOID);
VOS_VOID DIAG_ShowLost(VOS_VOID);
VOS_VOID DIAG_ShowTrans(VOS_UINT32 n);
VOS_VOID DIAG_ShowPsTransCmd(VOS_UINT32 n);
VOS_UINT32 diag_DebugMsgProc(MsgBlock* pMsgBlock);
VOS_VOID diag_ThroughputSave(DIAG_THRPUT_ID_ENUM enChn, VOS_UINT32 bytes);
VOS_VOID DIAG_ShowThroughput(VOS_UINT32 ulIndex);
VOS_VOID DIAG_DebugEventReport(VOS_UINT32 ulpid);
VOS_VOID DIAG_DebugLayerReport(VOS_UINT32 ulsndpid, VOS_UINT32 ulrcvpid, VOS_UINT32 ulMsg);
VOS_VOID DIAG_DebugVosLayerReport(VOS_UINT32 ulsndpid, VOS_UINT32 ulrcvpid, VOS_UINT32 ulMsg);
VOS_VOID DIAG_DebugLogReport(VOS_UINT32 ulpid, VOS_UINT32 level);
VOS_VOID DIAG_DebugTransReport(VOS_UINT32 ulpid);
VOS_VOID DIAG_DebugLayerCfg(VOS_UINT32 ulModuleId, VOS_UINT8 ucFlag);
VOS_VOID diag_ReportMntn(VOS_VOID);
void diag_debug_ind_src_lost(VOS_UINT32 len);
void diag_reset_src_mntn_info(void);
VOS_VOID diag_ReportSrcMntn(VOS_VOID);
VOS_VOID diag_ReportDstMntn(VOS_VOID);
VOS_UINT32 DIAG_ApiTest(VOS_UINT8* pstReq);
VOS_UINT32 diag_MntnCfgProc(VOS_UINT8* pstReq);
VOS_VOID diag_StopMntnTimer(VOS_VOID);
VOS_VOID diag_StartMntnTimer(VOS_UINT32 ulMntnReportTime);
VOS_VOID diag_DebugPackageRecord(VOS_UINT32 ulPackageLen);
VOS_VOID diag_DebugOverFlowRecord(VOS_UINT32 ulPackageLen);
VOS_VOID diag_NotifyOthersMntnInfo(DIAG_CMD_LOG_DIAG_MNTN_REQ_STRU *pstDebugReq);
VOS_VOID DIAG_DebugDtReport(VOS_UINT32 ulpid);

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of diag_Debug.h */

