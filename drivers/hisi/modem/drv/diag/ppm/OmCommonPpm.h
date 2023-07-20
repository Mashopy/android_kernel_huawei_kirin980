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


#ifndef __OM_COMMON_PPM_H__
#define __OM_COMMON_PPM_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include <mdrv.h>
#include <mdrv_diag_system.h>
#include <osl_types.h>
#include <bsp_dump.h>
#include <bsp_print.h>
#include <nv_stru_drv.h>
#include "OmUsbPpm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#pragma pack(push)
#pragma pack(4)

extern u32                          g_ulOmAcpuDbgFlag ;

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define USB_MAX_DATA_LEN            (60*1024)   /*USB���͵�������ݳ���*/

#define PPM_PCDEV_ICC_MSG_LEN_MAX   (16)

#define BIT_N(num)          (0x01 << (num))

#define OM_ACPU_RECV_USB        BIT_N(0)
#define OM_ACPU_DISPATCH_MSG    BIT_N(1)
#define OM_ACPU_SEND_SOCP       BIT_N(2)
#define OM_ACPU_SEND_USB        BIT_N(3)
#define OM_ACPU_USB_CB          BIT_N(4)
#define OM_ACPU_SEND_USB_IND    BIT_N(5)
#define OM_ACPU_ERRLOG_SEND     BIT_N(6)
#define OM_ACPU_ERRLOG_RCV      BIT_N(7)
#define OM_ACPU_ERRLOG_PRINT    BIT_N(8)
#define OM_ACPU_RECV_SOCKET     BIT_N(9)
#define OM_ACPU_SEND_SOCKET     BIT_N(10)
#define OM_ACPU_DATA            BIT_N(11)
#define OM_ACPU_READ_DONE       BIT_N(12)
#define OM_ACPU_CP_PCDEV_CB     BIT_N(13)
#define OM_ACPU_CFG_AP2MDM      BIT_N(14)
#define OM_ACPU_CFG_PCDEV_CB    BIT_N(15)
#define OM_ACPU_PCDEV_CB        BIT_N(16)


#define OM_ACPU_DEBUG_CHANNEL_TRACE(enChanID, pucData, ulDataLen, ulSwitch, ulDataSwitch) \
        if(false != (g_ulOmAcpuDbgFlag&ulSwitch)) \
        { \
            diag_crit("%s, channal ID: = %d, Data Len: = %d\n", __FUNCTION__, enChanID, ulDataLen); \
            if(false != (g_ulOmAcpuDbgFlag&ulDataSwitch) )\
            {\
                if (NULL != pucData)\
                {\
                    u32 ulOmDbgIndex;\
                    for (ulOmDbgIndex = 0 ; ulOmDbgIndex < ulDataLen; ulOmDbgIndex++) \
                    { \
                        diag_crit("%02x ", *((u8*)pucData + ulOmDbgIndex));\
                    } \
                }\
            }\
        }


/*����ͨ������ö��*/
enum
{
    CPM_OM_PORT_TYPE_USB    = 0,
    CPM_OM_PORT_TYPE_VCOM   = 1,
    CPM_OM_PORT_TYPE_WIFI   = 2,
    CPM_OM_PORT_TYPE_SD     = 3,
    CPM_OM_PORT_TYPE_FS     = 4,
#ifdef DIAG_SYSTEM_A_PLUS_B_CP
    CPM_OM_PORT_TYPE_PCDEV  = 6,
#endif
    CBP_OM_PORT_TYPE_BUTT
};

enum
{
    CP_AP_REQ_PORT_SWITCH       = 0x10, /* CP->AP �ж˿����� */
    CP_AP_REQ_CFG_PORT_STATE    = 0x11,
    CP_AP_REQ_IND_PORT_STATE    = 0x12,
    AP_CP_CNF_PORT_SWTICH       = 0x20, /* AP->CP �ж˿ڻظ� */
    AP_CP_REQ_CFG_PORT_STATE    = 0x21,
    AP_CP_REQ_IND_PORT_STATE    = 0x22,
    AP_CP_MSG_BUTT
};
typedef u32 AP_CP_MSG_ENUM_UINT32;

/* �豸�¼����� ����AP��CP_AGENT_DEV_STATE_E����һ��*/
typedef enum tagCPAGENT_DEV_STATE_E {
    PPM_PCDEV_DEV_LINK_DOWN = 0,        /* �豸�����Խ��ж�д(��Ҫ�����¼��ص�������״̬) */
    PPM_PCDEV_DEV_LINK_UP = 1,          /* �豸���Խ��ж�д(��Ҫ�����¼��ص�������״̬) */
    PPM_PCDEV_DEV_BOTTOM
} PPM_PCDEV_DEV_STATE_E;

/*******************************************************************************
  3 ö�ٶ���
*******************************************************************************/
/* UDI�豸��� */
enum OM_PROT_HANDLE_ENUM
{
    OM_USB_IND_PORT_HANDLE      =   0,
    OM_USB_CFG_PORT_HANDLE      =   1,
    OM_USB_CBT_PORT_HANDLE      =   2,
    OM_HSIC_IND_PORT_HANDLE     =   3,
    OM_HSIC_CFG_PORT_HANDLE     =   4,
    OM_PCDEV_IND_PORT_HANDLE    =   5,
    OM_PCDEV_CFG_PORT_HANDLE    =   6,
    OM_PORT_HANDLE_BUTT             /*OM_PORT_HANDLE_NUM = OM_PORT_HANDLE_BUTT��������Ĵ�ö��ע�����OM_PORT_HANDLE_NUM*/
};

typedef u32  OM_PROT_HANDLE_ENUM_UINT32;

enum OM_LOGIC_CHANNEL_ENUM
{
     OM_LOGIC_CHANNEL_CBT = 0,
     OM_LOGIC_CHANNEL_CNF,
     OM_LOGIC_CHANNEL_IND,
     OM_LOGIC_CHANNEL_BUTT
};

typedef u32     OM_LOGIC_CHANNEL_ENUM_UINT32;


/*****************************************************************************
  4 �ṹ�嶨��
*****************************************************************************/

typedef struct
{
    u8                          *pucAsyncCBData;      /* DRV_UDI_IOCTL�ӿ��첽���غ��������ָ�� */
    u32                          ulLen;               /* DRV_UDI_IOCTL�ӿ��첽���غ󷵻ص�ʵ�ʴ������ݳ��� */
    u32                          ulRsv;               /* Reserve */
}OM_PSEUDO_SYNC_STRU;

/*****************************************************************************
 ö����    : AT_PHY_PORT_ENUM
 ö��˵��  : ����˿ں�ö��ֵ ��omnvinterface.h��������
*****************************************************************************/
enum AT_PHY_PORT_ENUM
{
    AT_UART_PORT = 0,
    AT_PCUI_PORT,
    AT_CTRL_PORT,
    AT_HSUART_PORT,
    AT_PCUI2_PORT,
    AT_PORT_BUTT
};
typedef u32  AT_PHY_PORT_ENUM_UINT32;

enum
{
    CPM_IND_PORT = AT_PORT_BUTT,    /* OM�����ϱ��˿� */
    CPM_CFG_PORT,                   /* OM���ö˿� */
    CPM_SD_PORT,
    CPM_WIFI_OM_IND_PORT,           /* WIFI��OM�����ϱ��˿� */
    CPM_WIFI_OM_CFG_PORT,           /* WIFI��OM�����·��˿� */
    CPM_WIFI_AT_PORT,               /* WIFI��AT�˿� */
    CPM_HSIC_IND_PORT,
    CPM_HSIC_CFG_PORT,
    CPM_VCOM_IND_PORT,              /* VCOM��OM�����ϱ��ӿ� */
    CPM_VCOM_CFG_PORT,              /* VCOM��OM���ýӿ� */
    CPM_FS_PORT,
    CPM_PCDEV_IND_PORT,
    CPM_PCDEV_CFG_PORT,
    CPM_PORT_BUTT
};//����˿�ö��
typedef u32  CPM_PHY_PORT_ENUM_UINT32;

/* AP CPͨ����Ϣ�ṹ */
typedef struct
{
    u32 ulMsgId;    /* ��ϢID */
    u32 ulLen;      /* ��Ϣ���� */
    u8  pMsg[PPM_PCDEV_ICC_MSG_LEN_MAX];    /* ��Ϣ���� */
}AP_CP_MSG_STRU;

typedef struct
{
    u32 ulMsgId;    /* ��ϢID */
    u32 ulLen;      /* ��Ϣ���� */
    u32 ulState;    /* ״̬ */
}AP_CP_STATE_MSG_STRU;

/*****************************************************************************
  4 ��������
*****************************************************************************/

extern u32 PPM_ReadPortData(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, UDI_HANDLE UdiHandle, OM_PROT_HANDLE_ENUM_UINT32 enHandle);


extern void   PPM_PortWriteAsyCB(OM_PROT_HANDLE_ENUM_UINT32 enHandle, u8* pucData, s32 lLen);

extern void   PPM_ReadPortDataInit(CPM_PHY_PORT_ENUM_UINT32        enPhyPort,
                                    OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                    void                            *pReadCB,
                                    void                            *pWriteCB,
                                    void                            *pStateCB);

extern u32 PPM_UdiRegCallBackFun(UDI_HANDLE enHandle, u32 ulCmdType, void* pFunc);

extern void   PPM_PortCloseProc(OM_PROT_HANDLE_ENUM_UINT32  enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort);

extern u32 PPM_PortSend(OM_PROT_HANDLE_ENUM_UINT32 enHandle, u8 *pucVirAddr, u8 *pucPhyAddr, u32 ulDataLen);

extern void   PPM_PortStatus(OM_PROT_HANDLE_ENUM_UINT32 enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort,ACM_EVT_E enPortState);

extern void   PPM_GetSendDataLen(SOCP_CODER_DST_ENUM_U32 enChanID, u32 ulDataLen, u32 *pulSendDataLen, CPM_PHY_PORT_ENUM_UINT32 *penPhyport);

extern u32 PPM_PortInit(void);

extern int PPM_InitPhyPort(void);

void PPM_OmPortInfoShow(OM_PROT_HANDLE_ENUM_UINT32  enHandle);
void PPM_OmPortDebugInfoShow(void);
void PPM_PortSwitchInfoShow(void);

extern OM_PSEUDO_SYNC_STRU * PPM_ComPpmGetSyncInfo(void);
extern OM_ACPU_DEBUG_INFO * PPM_ComPpmGetDebugInfo(void);

/*****************************************************************************
  5 ȫ�ֱ�������
*****************************************************************************/
#ifdef DIAG_SYSTEM_A_PLUS_B_AP
extern void *       g_astOMPortUDIHandle[OM_PORT_HANDLE_BUTT];
#else
extern UDI_HANDLE   g_astOMPortUDIHandle[OM_PORT_HANDLE_BUTT];
#endif
/* USB���ص�OM IND�˿��У�α��Ϊͬ���ӿ�ʹ�õ����ݽṹ�� */
extern OM_PSEUDO_SYNC_STRU                     g_stUsbIndPseudoSync;

/* USB���ص�OM CNF�˿��У�α��Ϊͬ���ӿ�ʹ�õ����ݽṹ�� */
extern OM_PSEUDO_SYNC_STRU                     g_stUsbCfgPseudoSync;

/*****************************************************************************
  6 OTHERS����
*****************************************************************************/




#pragma pack(pop)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of OmCommonPpm.h*/

