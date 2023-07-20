/*
 *
 * All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses. You may choose this file to be licensed under the terms
 * of the GNU General Public License (GPL) Version 2 or the 2-clause
 * BSD license listed below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef __PPP_RAND_H__
#define __PPP_RAND_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "v_IO.h"
#include "TTFComm.h"

#if (VOS_RTOSCK == VOS_OS_VER) 
#include "TTFMemInterface.h"
#else
#include "TTFUtil.h"
#endif

#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/

/*����������A/C��ʹ�ýӿں�����ͬ�������������*/
#if (VOS_RTOSCK == VOS_OS_VER) 

// C�˽ӿڶ���
#define PPP_RAND_MEMCPY_S(Dest, ulDestSize, Src, Count)     PSCCORE_MEM_CPY(Dest, ulDestSize, Src, Count)
#define PPP_RAND_GET_STAT_SUM()                             PPPC_GenerateStatSum()
extern VOS_UINT32 PPPC_GenerateStatSum(VOS_VOID); 

#else

// A�˽ӿڶ���
#define PPP_RAND_MEMCPY_S(Dest, ulDestSize, Src, Count)     PSACORE_MEM_CPY(Dest, ulDestSize, Src, Count)
#define PPP_RAND_GET_STAT_SUM()                             PPP_GenerateStatSum()
extern VOS_UINT32 PPP_GenerateStatSum(VOS_VOID); 

#endif


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
  4 STRUCT����
*****************************************************************************/

/*****************************************************************************
  5 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  6 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  7 ��Ϣ����
*****************************************************************************/

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
VOS_VOID PPP_GetSecurityRand
(
    VOS_UINT8                           ucRandByteLen,
    VOS_UINT8                          *pucRand
);


#pragma pack()

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


#endif  /*end of __PPP_RAND_H__*/
