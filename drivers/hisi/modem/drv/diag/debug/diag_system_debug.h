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


#ifndef __DIAG_SYSTEM_DEBUG_H__
#define __DIAG_SYSTEM_DEBUG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include <product_config.h>
#include <mdrv.h>
#include <mdrv_diag_system.h>
#include <osl_types.h>
#include <osl_sem.h>
#include <bsp_slice.h>
#include <bsp_diag_frame.h>
#include <bsp_print.h>
#include <bsp_socp.h>

#pragma pack(push)
#pragma pack(4)

#undef  THIS_MODU
#define THIS_MODU mod_diag

#define DIAG_DEBUG_START        (0xaaaa5555)
#define DIAG_DEBUG_END          (0x5555aaaa)

/* debug�汾 V1.00 */
#define DIAG_DEBUG_VERSION      (0x00010000)

/* debug��ʾ��Ϣ�ĳ��� */
#define DIAG_DEBUG_INFO_LEN     (32)

/* �������ݲɼ���ʱ���� */
#define DIAG_DEBUG_DEALAY       (5000)

/* ���ӵ����ݽṹ���洢��Ϣǰ�ȴ洢��Ϣ���ȣ���8λ��0xa5����24λ�����ݳ��� */
#define DIAG_DEBUG_SIZE_FLAG    (0xa5000000)

#define diag_crit(fmt,...)       printk(KERN_ERR"[%s]:"fmt, BSP_MOD(THIS_MODU), ##__VA_ARGS__)
#define diag_error(fmt,...)      printk(KERN_ERR"[%s]:<%s %d>"fmt, BSP_MOD(THIS_MODU), __FUNCTION__, __LINE__,##__VA_ARGS__)
//#define diag_debug(fmt,...)      printk(KERN_ERR"[%s]:<%s %d>"fmt, BSP_MOD(THIS_MODU), __FUNCTION__, __LINE__,##__VA_ARGS__)
#define diag_debug(fmt,...)

/* OverFlow (Data Flow Record �������ͳ��)*******************************************/
#define DIAG_SRC_50_OVERFLOW_THR    (SCM_CODER_SRC_IND_BUFFER_SIZE >> 1)/* 50%������ֵ */
#define DIAG_SRC_80_OVERFLOW_THR    (SCM_CODER_SRC_IND_BUFFER_SIZE - (SCM_CODER_SRC_IND_BUFFER_SIZE >> 3)) /* 80%������ֵ */

/* DFR (Data Flow Record������¼��)*******************************************/

#define     DIAG_DFR_NAME_MAX       (8)
#define     DIAG_DFR_BUFFER_MAX     (8*1024)
#define     DIAG_DFR_MAGIC_NUM      (0x44494147)    /* DIAG */

#define     DIAG_DFR_START_NUM      (0xabcd4321)

#define DFR_ALIGN_WITH_4BYTE(len)      (((len) + 3)&(~3))

typedef struct
{
    u32  ulMagicNum;                 /* ħ���� */
    osl_sem_id     semid;                      /* �洢�ռ���ʵĻ����ź��� */
    char    name[DIAG_DFR_NAME_MAX];    /* �洢���������� */
    u32  ulCur;                      /* �洢�ռ�ĵ�ǰ��ַ */
    u32  ulLen;                      /* �洢�ռ���ܳ��� */
    u8   *pData;                     /* �洢�ռ���׵�ַ */
} DIAG_DFR_INFO_STRU;


typedef struct
{
    u32      ulStart;
    u32      ulTime;
}DIAG_DFR_HEADER_STRU;

/* PTR ***********************************************************************/
/*
��������ÿ�����������10-20�����ܸ��������������̣�
1000�����ܻ���50-100���������Ĵ�������
*/
#define DIAG_PTR_NUMBER             (2000)

typedef struct
{
    u16     enStep;     /*�¼�ö��*/
    u16     paraMark;   /*�Ƿ�Я������, 1Я������ 0��Я������*/
    u32     ulTime;     /*��ǰʱ��*/
    u32     para[2];    /*Я���Ĳ�������֧��0������2������֧������*//*lint !e43*/
} DIAG_PTR_ELEMENT_STRU;

typedef struct
{
    u32              ulCur;
    DIAG_PTR_ELEMENT_STRU   stPtr[DIAG_PTR_NUMBER];
} DIAG_PTR_INFO_STRU;

/* ������ͳ�� ***********************************************************************/
#define     DIAG_THRPUT_DEBUG_NUM       (100)       /* ����100���� */

#define     DIAG_THRPUT_DEBUG_LEN       (5*32768)   /* ÿ��ͳ�����������ټ��5������ */

typedef struct
{
    unsigned int      ulSlice;            /* ��ǰʱ���ʱ��� */
    unsigned int      ulThroughput;       /* ������(Bytes/s) */
}DIAG_THRPUT_NODE_STRU;

typedef struct
{
    unsigned int              ulSlice;            /* ���һ��ͳ�Ƶ�ʱ��� */
    unsigned int              ulBytes;            /* ��ǰ���ۼƵ��ֽ��� */
    unsigned int              ulMax;              /* �����ʷ�ֵ */
    unsigned int              ulPtr;              /* ��ǰָ�� */
    DIAG_THRPUT_NODE_STRU   stNode[DIAG_THRPUT_DEBUG_NUM];
}DIAG_THRPUT_INFO_STRU;


extern DIAG_MNTN_DST_INFO_STRU g_ind_dst_mntn_info;
extern u32 g_astThroughput[EN_DIAG_THRPUT_MAX];
extern u32              g_ulSendUSBStartSlice;
extern u32              g_ulSendPcdevStartSlice;
/******************LNR********************************/
#define DIAG_DRV_LNR_NUMBER             (100)

typedef enum
{
    EN_DIAG_DRV_LNR_CMDID = 0,  /* ���N�����յ���������� */

    EN_DIAG_DRV_LNR_LOG_LOST,   /* ���N��log��ʧ�ļ�¼ */

    EN_DIAG_DRV_LNR_PS_TRANS,   /* ���N��PS͸������ļ�¼ */

    EN_DIAG_DRV_LNR_CCORE_MSG,  /* ���N��C�˴�A���յ�����ϢID�ļ�¼ */

    EN_DIAG_DRV_LNR_MESSAGE_RPT,/* ���N��ͨ��messageģ��report�ϱ���cmdid�ļ�¼ */

    EN_DIAG_DRV_LNR_TRANS_IND,  /* ���N��͸���ϱ��ļ�¼ */

    EN_DIAG_DRV_LNR_INFO_MAX
} DIAG_DRV_LNR_ID_ENUM;

typedef enum
{
    NOT_SUPPORT_QUERY = 0,   /*DIAG��֧��USB����*/
    TYPE_USB_SPEED_HIGH = 1, /*USB 2.0*/
    TYPE_USB_SPEED_SUPER = 2,/*USB 3.0*/
    TYPE_USB_SPEED_SUPER_PLUS = 3, /*USB 3.1*/
}DIAG_USB_TYPE_ENUM;

typedef struct
{
    unsigned int ulCur;
    unsigned int ulRserved1[DIAG_DRV_LNR_NUMBER];
    unsigned int ulRserved2[DIAG_DRV_LNR_NUMBER];
} DRV_DIAG_LNR_INFO_TBL_STRU;

#ifndef DIAG_SYSTEM_A_PLUS_B_AP
static inline void diag_system_debug_ind_dst_lost(u32 type, u32 len)
{
    g_ind_dst_mntn_info.ulDeltaLostTimes++;
    g_ind_dst_mntn_info.ulDeltaLostLen += len;
    g_ind_dst_mntn_info.aulCurFailNum[type]++;
    g_ind_dst_mntn_info.aulCurFailLen[type] += len;
}

static inline void diag_system_debug_rev_socp_data(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaSocpLen += len;
}

static inline void  diag_system_debug_usb_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaUSBLen += len;

}
static inline void  diag_system_debug_usb_ok_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaUSBOKLen += len;

}

static inline void  diag_system_debug_usb_free_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaUSBFreeLen += len;

}
static inline void  diag_system_debug_usb_fail_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaUSBFailLen += len;
}

static inline void  diag_system_debug_pcdev_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaPcdevLen += len;

}
static inline void  diag_system_debug_pcdev_ok_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaPcdevOKLen += len;

}

static inline void  diag_system_debug_pcdev_free_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaPcdevFreeLen += len;

}
static inline void  diag_system_debug_pcdev_fail_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaPcdevFailLen += len;
}

static inline void diag_system_debug_vcom_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaVcomLen += len;

}
static inline void diag_system_debug_vcom_fail_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaVcomFailLen += len;

}
static inline void diag_system_debug_vcom_sucess_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaVcomSucessLen += len;

}
static inline void diag_system_debug_socket_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaSocketLen += len;
}
static inline void diag_system_debug_socket_sucess_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaSocketSucessLen += len;
}
static inline void diag_system_debug_socket_fail_len(u32 len)
{
    g_ind_dst_mntn_info.ulDeltaSocketFailLen += len;
}

static inline void diag_system_debug_send_data_end(void)
{
    u32 curent_slice;
    u32 start_slice;
    u32 delta;

    curent_slice = bsp_get_slice_value();
    start_slice = bsp_get_socp_ind_dst_int_slice();
    delta =  get_timer_slice_delta(start_slice, curent_slice);

    g_ind_dst_mntn_info.ulDeltaSocpIntToPortTime += delta;
}
static inline void diag_system_debug_send_usb_end(void)
{
    u32 curent_slice;
    u32 start_slice;
    u32 delta;

    curent_slice    = bsp_get_slice_value();
    start_slice     = g_ulSendUSBStartSlice;

    delta = get_timer_slice_delta(start_slice, curent_slice);
    g_ind_dst_mntn_info.ulDeltaUsbSendTime += delta;
}
static inline void diag_system_debug_send_usb_start(void)
{
    g_ulSendUSBStartSlice = bsp_get_slice_value();
}
static inline void diag_system_debug_send_pcdev_end(void)
{
    u32 curent_slice;
    u32 start_slice;
    u32 delta;

    curent_slice    = bsp_get_slice_value();
    start_slice     = g_ulSendPcdevStartSlice;

    delta = get_timer_slice_delta(start_slice, curent_slice);
    g_ind_dst_mntn_info.ulDeltaPcdevSendTime += delta;
}
static inline void diag_system_debug_send_pcdev_start(void)
{
    g_ulSendPcdevStartSlice = bsp_get_slice_value();
}

static inline void diag_ThroughputSave(DIAG_THRPUT_ID_ENUM enChn, u32 bytes)
{
    g_astThroughput[enChn] += bytes;
}

#else
static inline void diag_system_debug_ind_dst_lost(u32 type, u32 len)
{
    return;
}

static inline void diag_system_debug_rev_socp_data(u32 len)
{
    return;
}

static inline void  diag_system_debug_usb_len(u32 len)
{
    return;
}
static inline void  diag_system_debug_usb_ok_len(u32 len)
{
    return;
}

static inline void  diag_system_debug_usb_free_len(u32 len)
{
    return;
}
static inline void  diag_system_debug_usb_fail_len(u32 len)
{
    return;
}

static inline void diag_system_debug_vcom_len(u32 len)
{
    return;

}
static inline void diag_system_debug_vcom_fail_len(u32 len)
{
    return;

}
static inline void diag_system_debug_vcom_sucess_len(u32 len)
{
    return;
}
static inline void diag_system_debug_socket_len(u32 len)
{
    return;
}
static inline void diag_system_debug_socket_sucess_len(u32 len)
{
    return;
}
static inline void diag_system_debug_socket_fail_len(u32 len)
{
    return;
}

static inline void diag_system_debug_send_data_end(void)
{
    return;
}
static inline void diag_system_debug_send_usb_end(void)
{
    return;
}
static inline void diag_system_debug_send_usb_start(void)
{
    return;
}
static inline void diag_ThroughputSave(DIAG_THRPUT_ID_ENUM enChn, u32 bytes)
{
    return;
}
#endif
/* ��������*******************************************/
void DIAG_DebugPTR(void);
void DIAG_DebugShowPTR(u32 ulnum);
void diag_PTR(DIAG_PTR_ID_ENUM enType, u32 paraMark, u32 para0, u32 para1);
u32 DIAG_SystemDebugFileHeader(u32 pFile);
void diag_ThroughputSave(DIAG_THRPUT_ID_ENUM enChn, u32 bytes);
void DIAG_ShowThroughput(u32 ulIndex);
u32 DIAG_GetThrputInfo(DIAG_THRPUT_ID_ENUM type);
void DIAG_SystemDebugFileTail(u32 pFile, s8 *FilePath);
s32 diag_system_debug_event_cb(unsigned int u32ChanID, SOCP_EVENT_ENUM_UIN32 u32Event, unsigned int u32Param);
DIAG_MNTN_DST_INFO_STRU * diag_DebugGetDstMntnInfo(void);
void diag_reset_dst_mntn_info(void);
u32 diag_debug_log_level(u32 level);
u32 diag_CreateDFR(char *name, u32 ulLen, DIAG_DFR_INFO_STRU *pDfr);
void diag_SaveDFR(DIAG_DFR_INFO_STRU *pDfr, u8 *pData, u32 ulLen);
void diag_GetDFR(DIAG_DFR_INFO_STRU *pDfr);
void diag_DrvLNR(DIAG_DRV_LNR_ID_ENUM ulType, u32 ulRserved1, u32 ulRserved2);
void DIAG_DrvShowLNR(DIAG_DRV_LNR_ID_ENUM ulType, u32 n);
u32 DIAG_DebugFileHeader(void *pFile);
void DIAG_DebugFileTail(void *pFile, char *FilePath);
void diag_DebugResetSrcMntnInfo(void);
DIAG_DRV_DEBUG_INFO_STRU * diag_DebugGetSrcMntnInfo(void);
void diag_DebugOverFlowRecord(u32 ulPackageLen);
void diag_debug_ind_src_lost(u32 len);
void diag_DebugPackageRecord(u32 ulPackageLen);
u32 diag_get_usb_type(void);

#pragma pack(pop)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of diag_Debug.h */

