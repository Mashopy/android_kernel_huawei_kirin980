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


#ifndef __DIAG_CONNECT_COMM_H__
#define __DIAG_CONNECT_COMM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "vos.h"
#include "VosPidDef.h"
#include "nv_stru_msp.h"
#include "msp_diag.h"
#include "diag_cmdid_def.h"
#include "diag_frame.h"

#pragma pack(push)
#pragma pack(4)


/* ������Ϣ��ʱ��ʱ�� 5s��ʱ */
#define     DIAG_CONNECT_TIMER_LEN          (3*1000)

#define     DIAG_CONNECT_TIMER_NAME         (0x00006006)

#define     DIAG_CONNECT_TIMER_PARA         (DIAG_CMD_HOST_CONNECT)

#define     DIAG_CHANN_DEFAULT              (0x5C5C5C5C)
#define     DIAG_CHANN_NOT_IN_USE           (0x5A5A5A5A)
#define     DIAG_CHANN_REGISTER             (0x5B5B5B5B)
#define     DIAG_CHANN_OWNER_NOT_INIT       (0x5E5E5E5E)

#ifdef DIAG_SYSTEM_5G
#define     DIAG_SRC_CHANNLE_NUM            (64)
#else
#define     DIAG_SRC_CHANNLE_NUM            (32)
#endif

#define     DIAG_ATUH_DATA_SIZE             (32)


typedef struct
{
    VOS_UINT32 ulAuid;          /* ԭAUID*/
    VOS_UINT32 ulSn;            /* HSO�ַ�������������*/

    VOS_UINT32 ulMsgTransId;       /* Trans ID */
    VOS_UINT32 ulMsgId;
} DIAG_CMD_CONNECT_HEAD_STRU;


struct CONNECT_CTRL_STRU
{
    HTIMER                      stTimer;
    VOS_UINT32                  ulChannelNum;
    VOS_UINT32                  ulState;
    VOS_UINT32                  ulSn;
    DIAG_CMD_CONNECT_HEAD_STRU  stConnectCmd;
    union
    {
        VOS_UINT32          ulID;           /* �ṹ��ID */
        MSP_DIAG_STID_STRU  stID;
    }connectCommandId;
};

typedef VOS_UINT32 (*DIAG_SEND_PROC_FUNC)(VOS_UINT8 * pData);

typedef struct
{
    VOS_UINT32              ulConnId;       /* DIAG_CONN_ID_TYPE_E */
    VOS_CHAR                cName[16];   /* ��ϵͳ����  */
    DIAG_SEND_PROC_FUNC     pSendFunc;      /* ���պ��� */
    VOS_UINT32              ulResult;       /* connect id�Ĵ����� */
    VOS_UINT32              ulChannelCount;
    VOS_UINT32              ulHasCnf;
}DIAG_CONNECT_PROC_STRU;

typedef struct{
    VOS_UINT32  ulChannelId;
    VOS_UINT32  ulOwner;
    VOS_UINT32  ulResult;
}DIAG_CONN_CHANN_TBL;

typedef struct
{
    VOS_UINT32  ulMsgId;    /* ID_MSG_DIAG_CMD_CONNECT_REQ ID_MSG_DIAG_CMD_DISCONNECT_REQ */
    VOS_UINT32  ulSn;
}DIAG_CONN_MSG_SEND_T;

/*****************************************************************************
 ���� : HSO����UE�豸
ID   : DIAG_CMD_HOST_CONNECT
REQ  : DIAG_CMD_HOST_CONNECT_REQ_STRU
CNF  : DIAG_CMD_HOST_CONNECT_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulMajorMinorVersion; /* ���汾��.�ΰ汾��*/
    VOS_UINT32 ulRevisionVersion;   /* �����汾��*/
    VOS_UINT32 ulBuildVersion;      /* �ڲ��汾��*/
} DIAG_CMD_UE_SOFT_VERSION_STRU;

typedef struct
{
    VOS_UINT16 usVVerNo;           /* V����*/
    VOS_UINT16 usRVerNo;           /* R����*/
    VOS_UINT16 usCVerNo;           /* C����*/
    VOS_UINT16 usBVerNo;           /* B����*/
    VOS_UINT16 usSpcNo;            /* SPC����*/
    VOS_UINT16 usHardwareVerNo;    /* Ӳ��PCB�ź�ӡ�ư�汾��*/
    VOS_UINT32 ulProductNo;        /* ��Ʒ���ͱ�ţ�����ͬ������ϵ�Ӳ��ƽ̨*/
} DIAG_CMD_UE_BUILD_VER_STRU; /* �ڲ��汾*/

typedef struct
{
    /* 010: OMͨ���ںϵİ汾        */
    /* 110: OM�ں�GUδ�ںϵİ汾    */
    /* 100: OM��ȫ�ںϵİ汾        */
    VOS_UINT32  ulDrxControlFlag:1; /* DRX����*/
    VOS_UINT32  ulPortFlag      :1; /* OM Port flag 0:old,1:new */
    VOS_UINT32  ulOmUnifyFlag   :1; /* OM */
    VOS_UINT32  ul5GFrameFlag   :1;
    VOS_UINT32  ulAuthFlag      :1; /* �Ƿ���а�ȫУ�� */
    VOS_UINT32  ulReserved      :27;
}DIAG_CONTROLFLAG_STRU;

/* �����ɹ�ʱ�Ļظ�  */
typedef struct
{
    VOS_UINT32 ulAuid;          /* ԭAUID*/
    VOS_UINT32 ulSn;            /* HSO�ַ�������������*/

    VOS_UINT32 ulRc;            /* �����*/
    VOS_CHAR szImei[16];
    DIAG_CMD_UE_SOFT_VERSION_STRU stUeSoftVersion;
    DIAG_CMD_UE_BUILD_VER_STRU stBuildVersion;
    VOS_UINT32 ulChipBaseAddr;
    union
    {
        VOS_UINT32              UintValue;
        DIAG_CONTROLFLAG_STRU   CtrlFlag;
    } diag_cfg;/* B135��������ʾ�͹������԰汾: 1:�͹��İ汾��0�������汾��0xFFFFFFFF:MSP��ȡNV��ʧ�ܣ�HSO����Ϊ���Ӳ��ɹ���������ʾ��Ҫ�����½�������*/
    VOS_UINT32 ulLpdMode;
    NV_ITEM_AGENT_FLAG_STRU stAgentFlag;
    VOS_CHAR szProduct[64];
#ifdef DIAG_SYSTEM_5G
    VOS_CHAR ucTimeStamp[8];
    VOS_UINT32 ulCpuFrequcy;
    VOS_UINT32 ulUsbType;
#endif

} DIAG_CMD_HOST_CONNECT_CNF_STRU;

/* ��ȡ������Ϣ */
typedef struct
{
    VOS_UINT32 ulAuid;          /* ԭAUID */
    VOS_UINT32 ulSn;            /* HSO�ַ������������� */
    VOS_UINT32 ulInfo;          /* ��ѯ��Ϣ */
} DIAG_CMD_GET_MDM_INFO_REQ_STRU;

typedef struct
{
    DIAG_CMD_GET_MDM_INFO_REQ_STRU  stMdmInfo;
    VOS_UINT8                       u8AuthData[0];
}DAIG_AUTH_REQ_STRU;

typedef struct
{
#ifdef DIAG_SYSTEM_5G
    DIAG_CMD_HOST_CONNECT_CNF_STRU  stConnCnf;
#endif
    DAIG_AUTH_REQ_STRU              stAuthReq;
}DIAG_AUTH_STRU;

typedef struct
{
    DIAG_CMD_GET_MDM_INFO_REQ_STRU  stMdmInfo;
    VOS_UINT32                      ulRet;
}DIAG_AUTH_CNF_STRU;

/* ��Ȩ����ظ� */
typedef struct
{
    VOS_UINT32 ulAuid;          /* ԭAUID*/
    VOS_UINT32 ulSn;            /* HSO�ַ�������������*/
    VOS_UINT32 ulRc;            /* �����*/
} DIAG_CMD_AUTH_CNF_STRU;

/* �����˼�ͨ�Žṹ�� */
typedef struct
{
     VOS_MSG_HEADER                     /*VOSͷ */
     VOS_UINT32                     ulMsgId;
     VOS_UINT32                     ulCmdId;
     DIAG_CMD_HOST_CONNECT_CNF_STRU stConnInfo;
}DIAG_MSG_MSP_CONN_STRU;
/* �Ͽ����� */
typedef struct
{
    VOS_UINT32 ulAuid;          /* ԭAUID*/
    VOS_UINT32 ulSn;            /* HSO�ַ�������������*/
    VOS_UINT32 ulRc;            /* �����*/
} DIAG_CMD_HOST_DISCONNECT_CNF_STRU;




#pragma pack(pop)

#ifdef __cplusplus
    #if __cplusplus
            }
    #endif
#endif

#endif
