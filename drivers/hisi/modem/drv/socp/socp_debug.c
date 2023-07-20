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


#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/debugfs.h>
#include "osl_types.h"
#include "bsp_rfile.h"
#include "bsp_icc.h"
#include "bsp_slice.h"
#include "osl_sem.h"
#include "osl_thread.h"
#include <bsp_module.h>
#include <securec.h>

#include "socp_balong.h"

#define  THIS_MODU mod_socp
#define SOCP_ROOT_PATH          "/data/hisi_logs/modem_log/socp/"


extern SOCP_DEBUG_INFO_S g_stSocpDebugInfo;
extern u32 g_ulSocpDebugTraceCfg;


struct socp_debug_icc_stru
{
    char OpsCmd[32];
};


typedef void (*socp_debug_ops)(void);

struct socp_debug_proc{
    char OpsCmd[32];
    socp_debug_ops OpsFunc;
};

struct socp_debug_ctrl_stru{
    osl_sem_id      task_sem;
    OSL_TASK_ID     task_id;
};
struct socp_debug_ctrl_stru g_stSocpDebugCtrl;


void socp_debug_help(void)
{
    socp_crit("help           :socp_debug_help\n");
    socp_crit("ap_count_clean :socp_debug_ApCountclean\n");
    socp_crit("ap_count_store :socp_debug_ApCountStore\n");
    socp_crit("cp_count_store :socp_debug_CpCountStore\n");
    socp_crit("cp_count_clean :socp_debug_CpCountClean\n");
    socp_crit("count_print    :socp_debug_PrintCount\n");
    socp_crit("reg_store      :socp_debug_RegStore\n");
    socp_crit("all_store      :socp_debug_AllStore\n");
    socp_crit("all_clean      :socp_debug_AllClean\n");
}

/* cov_verified_start */
void socp_debug_sendCmd(char* cmd)
{
    return;
}
void socp_debug_ApCountclean(void)
{
    return;
}

void socp_debug_CountStore(char* p,int len)
{
    return;
}
void socp_debug_ApCountStore(void)
{
    char p[] = "ApCount_";
    socp_debug_CountStore(p,strlen(p));
}
void socp_debug_RegStore(void)
{
    return;
}
void socp_debug_CpCountStore(void)
{
    char p[] = "CpCount_";
    socp_debug_CountStore(p,(int)strlen(p));
}
void socp_debug_CpCountClean(void)
{
    return;
}
void socp_debug_PrintCount(void)
{
    socp_help();
}

void socp_debug_AllStore(void)
{
    socp_debug_ApCountStore();
    socp_debug_CpCountStore();
    socp_debug_RegStore();
}

void socp_debug_AllClean(void)
{
    socp_debug_ApCountclean();
    socp_debug_CpCountClean();
}

struct socp_debug_proc g_strSocpOps[] = {
    {"help"                 ,socp_debug_help},
    {"ap_count_clean"       ,socp_debug_ApCountclean},
    {"ap_count_store"       ,socp_debug_ApCountStore},
    {"reg_store"            ,socp_debug_RegStore},
    {"cp_count_store"       ,socp_debug_CpCountStore},
    {"cp_count_clean"       ,socp_debug_CpCountClean},
    {"count_print"          ,socp_debug_PrintCount},
    {"all_store"            ,socp_debug_AllStore},
    {"all_clean"            ,socp_debug_AllClean},
};

int socp_debug_icc_msg_callback(u32 chanid ,u32 len,void* pdata)
{
    u32 channel_id = ICC_CHN_ACORE_CCORE_MIN <<16 | IFC_RECV_FUNC_SOCP_DEBUG;
    if(channel_id != chanid)
        return 0;

    socp_crit("enter here:\n");

    osl_sem_up(&g_stSocpDebugCtrl.task_sem);
    return 0;
}
int socp_debug_icc_task(void* para)
{
    int ret;
    u32 i;
    u32 chanid = ICC_CHN_ACORE_CCORE_MIN <<16 | IFC_RECV_FUNC_SOCP_DEBUG;
    u8  p[32];
    while(1)
    {
        osl_sem_down(&g_stSocpDebugCtrl.task_sem);

        memset_s(p, sizeof(p), 0, sizeof(p));

        ret = bsp_icc_read(chanid,p, sizeof(p));
        if(ret< (int)0 || ret >= (int)sizeof(p))
        {
            continue;
        }

        for(i = 0;i<sizeof(g_strSocpOps)/sizeof(g_strSocpOps[0]);i++)
        {
            if(!strncmp(g_strSocpOps[i].OpsCmd,(char*)p,strlen(g_strSocpOps[i].OpsCmd))){
                g_strSocpOps[i].OpsFunc();
            }
        }
    }
}

/*lint -save -e745*/
__init int socp_debug_init(void)
{
    u32 channel_id = ICC_CHN_ACORE_CCORE_MIN <<16 | IFC_RECV_FUNC_SOCP_DEBUG;

    osl_sem_init(0, &g_stSocpDebugCtrl.task_sem);

    (void)osl_task_init("socpProc",30, 0x1000, (OSL_TASK_FUNC)socp_debug_icc_task,NULL, &g_stSocpDebugCtrl.task_id);

    (void)bsp_icc_event_register(channel_id, (read_cb_func)socp_debug_icc_msg_callback,NULL,NULL,NULL);

    return 0;
}

/*****************************************************************************
* �� �� ��   : socp_debug_set_reg_bits
*
* ��������  : ����socp����ʱ���ã���������socp�Ĵ���
*
* �������  :   reg
                pos
                bits
                val
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
void socp_debug_set_reg_bits(u32 reg, u32 pos, u32 bits, u32 val)
{
    SOCP_REG_SETBITS(reg, pos, bits, val);
}

/*****************************************************************************
* �� �� ��   : socp_debug_get_reg_bits
*
* ��������  : ����socp����ʱ���ã���������socp�Ĵ���
*
* �������  :   reg
                pos
                bit
                val
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
u32 socp_debug_get_reg_bits(u32 reg, u32 pos, u32 bits)
{
    return SOCP_REG_GETBITS(reg, pos, bits);
}

/*****************************************************************************
* �� �� ��   : socp_debug_write_reg
*
* ��������  : ����socp����ʱ���ã����ڶ�ȡ�Ĵ���ֵ
*
* �������  :   reg
                result
*
* �������  : ��
*
* �� �� ֵ   : �Ĵ���ֵ
*****************************************************************************/
u32 socp_debug_read_reg(u32 reg)
{
    u32 result = 0;

    SOCP_REG_READ(reg, result);

    return result;
}

/*****************************************************************************
* �� �� ��   : socp_debug_read_reg
*
* ��������  : ����socp����ʱ���ã����ڶ�ȡ�Ĵ���ֵ
*
* �������  :   reg
                result
*
* �������  : ��
*
* �� �� ֵ   : ������
*****************************************************************************/
u32 socp_debug_write_reg(u32 reg, u32 data)
{
    u32 result = 0;

    SOCP_REG_WRITE(reg, data);
    SOCP_REG_READ(reg, result);

    if(result == data)
    {
        return 0;
    }

    return (u32)-1;
}

/*****************************************************************************
* �� �� ��   : socp_help
*
* ��������  : ��ȡsocp��ӡ��Ϣ
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
void socp_help(void)
{
    socp_crit("\r socp_show_debug_gbl\n");
    socp_crit("\r socp_show_enc_src_chan_curn");
    socp_crit("\r socp_show_enc_src_chan_add\n");
    socp_crit("\r socp_show_enc_src_chan_add\n");
    socp_crit("\r socp_show_enc_dst_chan_cur\n");
    socp_crit("\r socp_show_enc_dst_chan_add\n");
    socp_crit("\r socp_show_enc_dst_chan_all\n");
    socp_crit("\r socp_show_dec_src_chan_cur\n");
    socp_crit("\r socp_show_dec_src_chan_add\n");
    socp_crit("\r socp_show_dec_src_chan_all\n");
    socp_crit("\r socp_show_dec_dst_chan_cur\n");
    socp_crit("\r socp_show_dec_dst_chan_add\n");
    socp_crit("\r socp_show_dec_dst_chan_all\n");
    socp_crit("\r socp_show_ccore_head_err_cnt\n");
    socp_crit("\r socp_debug_cnt_show\n");
}

/*****************************************************************************
* �� �� ��   : socp_show_debug_gbl
*
* ��������  : ��ʾȫ��debug ������Ϣ
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
void socp_show_debug_gbl(void)
{
    SOCP_DEBUG_GBL_S *sSocpDebugGblInfo;

    sSocpDebugGblInfo = &g_stSocpDebugInfo.sSocpDebugGBl;

    socp_crit(" SOCP global status:\n");
    socp_crit(" g_strSocpStat.baseAddr): 0x%x\n", (s32)g_strSocpStat.baseAddr);
    socp_crit(" sSocpDebugGblInfo->u32SocpAllocEncSrcCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpAllocEncSrcCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpAllocEncSrcSucCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpAllocEncSrcSucCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpSetEncDstCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpSetEncDstCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpSetEncDstSucCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpSetEncDstSucCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpSetDecSrcCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpSetDecSrcCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpSetDeSrcSucCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpSetDeSrcSucCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpAllocDecDstCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpAllocDecDstCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpAllocDecDstSucCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpAllocDecDstSucCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpAppEtrIntCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpAppEtrIntCnt);
    socp_crit(" sSocpDebugGblInfo->u32SocpAppSucIntCnt: 0x%x\n", (s32)sSocpDebugGblInfo->u32SocpAppSucIntCnt);
}

/* cov_verified_stop */

/*****************************************************************************
* �� �� ��   : socp_show_enc_src_chan_cur
*
* ��������  : ��ӡ����Դͨ����ǰ����
*
* �������  : ͨ��ID
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/

u32 socp_show_enc_src_chan_cur(u32 u32UniqueId)
{
    u32 u32RealId   = 0;
    u32 u32ChanType = 0;
    u32 ret;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    if((ret=(u32)socp_check_chan_type(u32ChanType, SOCP_CODER_SRC_CHAN)) != BSP_OK)
    {
        return ret;
    }
    if((ret=(u32)socp_check_encsrc_chan_id(u32RealId)) != BSP_OK)
    {
        return ret;
    }

    socp_crit("EncSrc channel 0x%x status:\n", u32UniqueId);
    socp_crit("u32ChanID:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32ChanID);
    socp_crit("u32AllocStat:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32AllocStat);
    socp_crit("u32ChanEn:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32ChanEn);
    socp_crit("u32DestChanID:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32DestChanID);
    socp_crit("ePriority:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].ePriority);
    socp_crit("u32BypassEn:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].u32BypassEn);
    socp_crit("eChnMode:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].eChnMode);
    socp_crit("eDataType:\t\t%d\n", g_strSocpStat.sEncSrcChan[u32RealId].eDataType);
    socp_crit("sEncSrcBuf.u32Read:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Read);
    socp_crit("sEncSrcBuf.u32Write:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Write);
    socp_crit("sEncSrcBuf.u32Length:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sEncSrcBuf.u32Length);
    if (SOCP_ENCSRC_CHNMODE_LIST == g_strSocpStat.sEncSrcChan[u32RealId].eChnMode)
    {
        socp_crit("sRdBuf.u32Read:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Read);
        socp_crit("sRdBuf.u32Write:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Write);
        socp_crit("sRdBuf.u32Length:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].sRdBuf.u32Length);
        socp_crit("u32RdThreshold:\t\t0x%x\n", g_strSocpStat.sEncSrcChan[u32RealId].u32RdThreshold);
    }

    return BSP_OK;
}



/*****************************************************************************
* �� �� ��   : socp_show_enc_src_chan_add
*
* ��������  : ��ӡ����Դͨ���ۼ�ͳ��ֵ
*
* �������  : ͨ��ID
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
u32 socp_show_enc_src_chan_add(u32 u32UniqueId)
{
    u32 u32ChanType;
    u32 u32RealChanID;
    SOCP_DEBUG_ENCSRC_S *sSocpAddDebugEncSrc;
    s32 ret;

    u32RealChanID = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);
    if((ret=socp_check_chan_type(u32ChanType, SOCP_CODER_SRC_CHAN)) != BSP_OK)
    {
        return (u32)ret;
    }

    sSocpAddDebugEncSrc = &g_stSocpDebugInfo.sSocpDebugEncSrc;

    if((ret=socp_check_encsrc_chan_id(u32RealChanID)) != BSP_OK)
    {
        return (u32)ret;
    }
    socp_crit("EncSec channelc 0x%x cnt:\n", u32UniqueId);
    socp_crit("u32SocpFreeEncSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpFreeEncSrcCnt[u32RealChanID]);
    socp_crit("u32SocpStartEncSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpStartEncSrcCnt[u32RealChanID]);
    socp_crit("u32SocpStopEncSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpStopEncSrcCnt[u32RealChanID]);
    socp_crit("u32SocpSoftResetEncSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpSoftResetEncSrcCnt[u32RealChanID]);
    socp_crit("u32SocpRegEventEncSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpRegEventEncSrcCnt[u32RealChanID]);
    socp_crit("u32SocpGetWBufEncSrcEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpGetWBufEncSrcEtrCnt[u32RealChanID]);
    socp_crit("u32SocpGetWBufEncSrcSucCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpGetWBufEncSrcSucCnt[u32RealChanID]);
    socp_crit("u32socp_write_doneEncSrcEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32socp_write_doneEncSrcEtrCnt[u32RealChanID]);
    socp_crit("u32socp_write_doneEncSrcSucCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32socp_write_doneEncSrcSucCnt[u32RealChanID]);
    socp_crit("u32socp_write_doneEncSrcFailCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32socp_write_doneEncSrcFailCnt[u32RealChanID]);
    socp_crit("u32SocpRegRdCBEncSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpRegRdCBEncSrcCnt[u32RealChanID]);
    socp_crit("u32SocpGetRdBufEncSrcEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpGetRdBufEncSrcEtrCnt[u32RealChanID]);
    socp_crit("u32SocpGetRdBufEncSrcSucCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpGetRdBufEncSrcSucCnt[u32RealChanID]);
    socp_crit("u32SocpReadRdDoneEncSrcEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcEtrCnt[u32RealChanID]);
    socp_crit("u32SocpReadRdDoneEncSrcSucCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcSucCnt[u32RealChanID]);
    socp_crit("u32SocpReadRdDoneEncSrcFailCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpReadRdDoneEncSrcFailCnt[u32RealChanID]);
    socp_crit("u32SocpEncSrcIsrHeadIntCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcIsrHeadIntCnt[u32RealChanID]);
    socp_crit("u32SocpEncSrcTskHeadCbOriCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcTskHeadCbOriCnt[u32RealChanID]);
    socp_crit("u32SocpEncSrcTskHeadCbCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcTskHeadCbCnt[u32RealChanID]);
    socp_crit("u32SocpEncSrcIsrRdIntCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcIsrRdIntCnt[u32RealChanID]);
    socp_crit("u32SocpEncSrcTskRdCbOriCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcTskRdCbOriCnt[u32RealChanID]);
    socp_crit("u32SocpEncSrcTskRdCbCnt: 0x%x\n",
           (s32)sSocpAddDebugEncSrc->u32SocpEncSrcTskRdCbCnt[u32RealChanID]);

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��   : socp_show_enc_src_chan_add
*
* ��������  : ��ӡ���б���Դͨ����Ϣ
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
void  socp_show_enc_src_chan_all(void)
{
    u32 i;

    for (i = 0; i < SOCP_MAX_ENCSRC_CHN; i++)
    {
        (void)socp_show_enc_src_chan_cur(i);
        (void)socp_show_enc_src_chan_add(i);
    }

    return;
}

/*****************************************************************************
* �� �� ��   : socp_show_enc_dst_chan_cur
*
* ��������  : ��ӡ����Ŀ��ͨ����Ϣ
*
* �������  : ͨ��ID
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
u32 socp_show_enc_dst_chan_cur(u32 u32UniqueId)
{
    u32 u32RealId   = 0;
    u32 u32ChanType = 0;
    u32 ret;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    if((ret=socp_check_chan_type(u32ChanType, SOCP_CODER_DEST_CHAN)) != BSP_OK)
    {
        return ret;
    }

    socp_crit("EncDst channel 0x%x status:\n", u32UniqueId);
    socp_crit("u32ChanID:%d\n", g_strSocpStat.sEncDstChan[u32RealId].u32ChanID);
    socp_crit("u32SetStat:%d\n", g_strSocpStat.sEncDstChan[u32RealId].u32SetStat);
    socp_crit("sEncDstBuf.u32Read:0x%x\n", g_strSocpStat.sEncDstChan[u32RealId].sEncDstBuf.u32Read);
    socp_crit("sEncDstBuf.u32Write:0x%x\n", g_strSocpStat.sEncDstChan[u32RealId].sEncDstBuf.u32Write);
    socp_crit("sEncDstBuf.u32Length:0x%x\n", g_strSocpStat.sEncDstChan[u32RealId].sEncDstBuf.u32Length);

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��   : socp_show_enc_dst_chan_add
*
* ��������  : ��ӡ����Ŀ��ͨ���ۼ�ͳ��ֵ
*
* �������  : ͨ��ID
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
u32 socp_show_enc_dst_chan_add(u32 u32UniqueId)
{
    u32 u32RealChanID;
    u32 u32ChanType = 0;
    u32 ret;
    SOCP_DEBUG_ENCDST_S *sSocpAddDebugEncDst;

    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);
    if((ret=socp_check_chan_type(u32ChanType, SOCP_CODER_DEST_CHAN)) != BSP_OK)
    {
        return ret;
    }

    u32RealChanID = SOCP_REAL_CHAN_ID(u32UniqueId);
    sSocpAddDebugEncDst = &g_stSocpDebugInfo.sSocpDebugEncDst;

    socp_crit("EncDst channel 0x%x Cnt:\n", u32UniqueId);
    socp_crit("u32SocpRegEventEncDstCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpRegEventEncDstCnt[u32RealChanID]);
    socp_crit("u32SocpRegReadCBEncDstCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpRegReadCBEncDstCnt[u32RealChanID]);
    socp_crit("u32SocpGetReadBufEncDstEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpGetReadBufEncDstEtrCnt[u32RealChanID]);
    socp_crit("u32SocpGetReadBufEncDstSucCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpGetReadBufEncDstSucCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneEncDstEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32socp_read_doneEncDstEtrCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneEncDstSucCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32socp_read_doneEncDstSucCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneEncDstFailCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32socp_read_doneEncDstFailCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneZeroEncDstCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32socp_read_doneZeroEncDstCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneValidEncDstCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32socp_read_doneValidEncDstCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstIsrTrfIntCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstIsrTrfIntCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstTskTrfCbOriCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstTskTrfCbOriCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstTskTrfCbCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstTskTrfCbCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstIsrOvfIntCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstIsrOvfIntCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstTskOvfCbOriCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstTskOvfCbOriCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstTskOvfCbCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstTskOvfCbCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstIsrThresholdOvfIntCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstIsrThresholdOvfIntCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstTskThresholdOvfCbOriCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstTskThresholdOvfCbOriCnt[u32RealChanID]);
    socp_crit("u32SocpEncDstTskThresholdOvfCbCnt: 0x%x\n",
           (s32)sSocpAddDebugEncDst->u32SocpEncDstTskThresholdOvfCbCnt[u32RealChanID]);

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��   : socp_show_enc_dst_chan_all
*
* ��������  : ��ӡ����Ŀ��ͨ����Ϣ
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
void socp_show_enc_dst_chan_all(void)
{
    u32 i;
    u32 u32UniqueId = 0;

    for (i = 0; i < SOCP_MAX_ENCDST_CHN; i++)
    {
        u32UniqueId = SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, i);
        (void)socp_show_enc_dst_chan_cur(u32UniqueId);
        (void)socp_show_enc_dst_chan_add(u32UniqueId);
    }

    return;
}
/*****************************************************************************
* �� �� ��   : socp_show_dec_src_chan_cur
*
* ��������  : ��ӡ����Դͨ����Ϣ
*
* �������  : ͨ��ID
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
u32 socp_show_dec_src_chan_cur(u32 u32UniqueId)
{
    u32 u32RealId   = 0;
    u32 u32ChanType = 0;
    u32 ret;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    if((ret=socp_check_chan_type(u32ChanType, SOCP_DECODER_SRC_CHAN)) != BSP_OK)
    {
        return ret;
    }

    socp_crit("DecSrc channel 0x%x status:\n", u32UniqueId);
    socp_crit("u32ChanID:%d\n", g_strSocpStat.sDecSrcChan[u32RealId].u32ChanID);
    socp_crit("u32SetStat:%d\n", g_strSocpStat.sDecSrcChan[u32RealId].u32SetStat);
    socp_crit("u32ChanEn:%d\n", g_strSocpStat.sDecSrcChan[u32RealId].u32ChanEn);
    socp_crit("eChnMode:%d\n", g_strSocpStat.sDecSrcChan[u32RealId].eChnMode);
    socp_crit("sDecSrcBuf.u32Read:0x%x\n", g_strSocpStat.sDecSrcChan[u32RealId].sDecSrcBuf.u32Read);
    socp_crit("sDecSrcBuf.u32Write:0x%x\n", g_strSocpStat.sDecSrcChan[u32RealId].sDecSrcBuf.u32Write);
    socp_crit("sDecSrcBuf.u32Length:0x%x\n", g_strSocpStat.sDecSrcChan[u32RealId].sDecSrcBuf.u32Length);

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��   : socp_show_dec_src_chan_add
*
* ��������  : ��ӡ����Դͨ���ۼ�ͳ��ֵ
*
* �������  : ͨ��ID
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
u32 socp_show_dec_src_chan_add(u32 u32UniqueId)
{
    u32 u32RealChanID;
    SOCP_DEBUG_DECSRC_S *sSocpAddDebugDecSrc;
    u32 u32ChanType = 0;
    u32 ret;

    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);
    if((ret=socp_check_chan_type(u32ChanType, SOCP_DECODER_SRC_CHAN)) != BSP_OK)
    {
        return ret;
    }
    u32RealChanID = SOCP_REAL_CHAN_ID(u32UniqueId);
    sSocpAddDebugDecSrc = &g_stSocpDebugInfo.sSocpDebugDecSrc;

    socp_crit("DecSrc channel 0x%x Cnt:\n", u32UniqueId);
    socp_crit("u32SocpSoftResetDecSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpSoftResetDecSrcCnt[u32RealChanID]);
    socp_crit("u32SocpStartDecSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpStartDecSrcCnt[u32RealChanID]);
    socp_crit("u32SocpStopDecSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpStopDecSrcCnt[u32RealChanID]);
    socp_crit("u32SocpRegEventDecSrcCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpRegEventDecSrcCnt[u32RealChanID]);
    socp_crit("u32SocpGetWBufDecSrcEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpGetWBufDecSrcEtrCnt[u32RealChanID]);
    socp_crit("u32SocpGetWBufDecSrcSucCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpGetWBufDecSrcSucCnt[u32RealChanID]);
    socp_crit("u32socp_write_doneDecSrcEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32socp_write_doneDecSrcEtrCnt[u32RealChanID]);
    socp_crit("u32socp_write_doneDecSrcSucCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32socp_write_doneDecSrcSucCnt[u32RealChanID]);
    socp_crit("u32socp_write_doneDecSrcFailCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32socp_write_doneDecSrcFailCnt[u32RealChanID]);
    socp_crit("u32SocpDecSrcIsrErrIntCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpDecSrcIsrErrIntCnt[u32RealChanID]);
    socp_crit("u32SocpDecSrcTskErrCbOriCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpDecSrcTskErrCbOriCnt[u32RealChanID]);
    socp_crit("u32SocpDecSrcTskErrCbCnt: 0x%x\n",
           (s32)sSocpAddDebugDecSrc->u32SocpDecSrcTskErrCbCnt[u32RealChanID]);
    return BSP_OK;
}

/*****************************************************************************
* �� �� ��   : socp_show_dec_src_chan_all
*
* ��������  : ��ӡ����Դͨ����Ϣ
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
void socp_show_dec_src_chan_all(void)
{
    u32 i;
    u32 u32UniqueId = 0;

    for (i = 0; i < SOCP_MAX_DECSRC_CHN; i++)
    {
        u32UniqueId = SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, i);
        (void)socp_show_dec_src_chan_cur(u32UniqueId);
        (void)socp_show_dec_src_chan_add(u32UniqueId);
    }

    return;
}

/*****************************************************************************
* �� �� ��   : socp_show_dec_dst_chan_cur
*
* ��������  : ��ӡ����Ŀ��ͨ����Ϣ
*
* �������  : ͨ��ID
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
u32 socp_show_dec_dst_chan_cur(u32 u32UniqueId)
{
    u32 u32RealId   = 0;
    u32 u32ChanType = 0;
    u32 ret;

    u32RealId   = SOCP_REAL_CHAN_ID(u32UniqueId);
    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);

    if((ret=socp_check_chan_type(u32ChanType, SOCP_DECODER_DEST_CHAN)) != BSP_OK)
    {
        return ret;
    }
    socp_crit("DecDst channel 0x%x status:\n", u32UniqueId);
    socp_crit("u32ChanID:%d\n", g_strSocpStat.sDecDstChan[u32RealId].u32ChanID);
    socp_crit("u32AllocStat:%d\n", g_strSocpStat.sDecDstChan[u32RealId].u32AllocStat);
    socp_crit("eDataType:%d\n", g_strSocpStat.sDecDstChan[u32RealId].eDataType);
    socp_crit("sDecDstBuf.u32Read:0x%x\n", g_strSocpStat.sDecDstChan[u32RealId].sDecDstBuf.u32Read);
    socp_crit("sDecDstBuf.u32Write:0x%x\n", g_strSocpStat.sDecDstChan[u32RealId].sDecDstBuf.u32Write);
    socp_crit("sDecDstBuf.u32Length:0x%x\n", g_strSocpStat.sDecDstChan[u32RealId].sDecDstBuf.u32Length);

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��   : socp_show_dec_dst_chan_add
*
* ��������  : ��ӡ����Ŀ��ͨ���ۼ�ͳ��ֵ
*
* �������  : ͨ��ID
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
u32 socp_show_dec_dst_chan_add(u32 u32UniqueId)
{
    u32 u32RealChanID;
    SOCP_DEBUG_DECDST_S *sSocpAddDebugDecDst;
    u32 u32ChanType = 0;
    u32 ret;

    u32ChanType = SOCP_REAL_CHAN_TYPE(u32UniqueId);
    if((ret=socp_check_chan_type(u32ChanType, SOCP_DECODER_DEST_CHAN)) != BSP_OK)
    {
        return ret;
    }

    u32RealChanID = SOCP_REAL_CHAN_ID(u32UniqueId);
    sSocpAddDebugDecDst = &g_stSocpDebugInfo.sSocpDebugDecDst;

    socp_crit("DecDst channel 0x%x Cnt:\n", u32UniqueId);
    socp_crit("u32SocpFreeDecDstCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpFreeDecDstCnt[u32RealChanID]);
    socp_crit("u32SocpRegEventDecDstCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpRegEventDecDstCnt[u32RealChanID]);
    socp_crit("u32SocpRegReadCBDecDstCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpRegReadCBDecDstCnt[u32RealChanID]);
    socp_crit("u32SocpGetReadBufDecDstEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpGetReadBufDecDstEtrCnt[u32RealChanID]);
    socp_crit("u32SocpGetReadBufDecDstSucCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpGetReadBufDecDstSucCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneDecDstEtrCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32socp_read_doneDecDstEtrCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneDecDstSucCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32socp_read_doneDecDstSucCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneDecDstFailCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32socp_read_doneDecDstFailCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneZeroDecDstCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32socp_read_doneZeroDecDstCnt[u32RealChanID]);
    socp_crit("u32socp_read_doneValidDecDstCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32socp_read_doneValidDecDstCnt[u32RealChanID]);
    socp_crit("u32SocpDecDstIsrTrfIntCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpDecDstIsrTrfIntCnt[u32RealChanID]);
    socp_crit("u32SocpDecDstTskTrfCbOriCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpDecDstTskTrfCbOriCnt[u32RealChanID]);
    socp_crit("u32SocpDecDstTskTrfCbCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpDecDstTskTrfCbCnt[u32RealChanID]);
    socp_crit("u32SocpDecDstIsrOvfIntCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpDecDstIsrOvfIntCnt[u32RealChanID]);
    socp_crit("u32SocpDecDstTskOvfCbOriCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpDecDstTskOvfCbOriCnt[u32RealChanID]);
    socp_crit("u32SocpDecDstTskOvfCbCnt: 0x%x\n",
           (s32)sSocpAddDebugDecDst->u32SocpDecDstTskOvfCbCnt[u32RealChanID]);

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��   : socp_show_dec_dst_chan_all
*
* ��������  : ��ӡ����Ŀ��ͨ����Ϣ
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
void socp_show_dec_dst_chan_all(void)
{
    u32 i;
    u32 u32UniqueId = 0;

    for (i = 0; i < SOCP_MAX_DECDST_CHN; i++)
    {
        u32UniqueId = SOCP_CHAN_DEF(SOCP_DECODER_DEST_CHAN, i);
        (void)socp_show_dec_dst_chan_cur(u32UniqueId);
        (void)socp_show_dec_dst_chan_add(u32UniqueId);
    }

    return;
}

EXPORT_SYMBOL(socp_show_dec_src_chan_cur);
EXPORT_SYMBOL(socp_show_dec_src_chan_add);
EXPORT_SYMBOL(socp_show_dec_src_chan_all);
EXPORT_SYMBOL(socp_show_dec_dst_chan_cur);
EXPORT_SYMBOL(socp_show_dec_dst_chan_add);
EXPORT_SYMBOL(socp_show_dec_dst_chan_all);

/*****************************************************************************
* �� �� ��   : socp_debug_cnt_show
*
* ��������  : ��ʾdebug ������Ϣ
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ   : ��
*****************************************************************************/
void socp_debug_cnt_show(void)
{
    socp_show_debug_gbl();
    socp_show_enc_src_chan_all();
    socp_show_enc_dst_chan_all();
    socp_show_dec_src_chan_all();
    socp_show_dec_dst_chan_all();
}

void socp_debug_set_trace(u32 v)
{
    g_ulSocpDebugTraceCfg = v;
}
EXPORT_SYMBOL(socp_debug_help);
EXPORT_SYMBOL(socp_debug_AllStore);
EXPORT_SYMBOL(socp_debug_AllClean);
EXPORT_SYMBOL(socp_debug_set_reg_bits);
EXPORT_SYMBOL(socp_debug_get_reg_bits);
EXPORT_SYMBOL(socp_debug_read_reg);
EXPORT_SYMBOL(socp_debug_write_reg);
EXPORT_SYMBOL(socp_help);
EXPORT_SYMBOL(socp_show_debug_gbl);
EXPORT_SYMBOL(socp_show_enc_src_chan_cur);
EXPORT_SYMBOL(socp_show_enc_src_chan_add);
EXPORT_SYMBOL(socp_show_enc_src_chan_all);
EXPORT_SYMBOL(socp_show_enc_dst_chan_cur);
EXPORT_SYMBOL(socp_show_enc_dst_chan_add);
EXPORT_SYMBOL(socp_show_enc_dst_chan_all);
EXPORT_SYMBOL(socp_debug_cnt_show);




