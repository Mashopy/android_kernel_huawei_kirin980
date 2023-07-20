


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "oal_kernel_file.h"
#include "wlan_spec.h"
#include "mac_vap.h"
#include "mac_ie.h"
#include "mac_frame.h"
#include "hmac_mgmt_bss_comm.h"
#include "mac_resource.h"
#include "hmac_device.h"
#include "hmac_resource.h"
#include "hmac_fsm.h"
#include "hmac_encap_frame.h"
#include "hmac_tx_amsdu.h"
#include "hmac_mgmt_ap.h"
#include "hmac_mgmt_sta.h"
#include "hmac_blockack.h"
#include "hmac_p2p.h"
#ifdef _PRE_WLAN_FEATURE_HILINK
#include "hmac_fbt_main.h"
#endif
#ifdef _PRE_WLAN_1103_CHR
#include "hmac_dfx.h"
#endif
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_MGMT_BSS_COMM_C

#ifdef _PRE_WLAN_FEATURE_SNIFFER
#include <hwnet/ipv4/sysctl_sniffer.h>
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/*
��:����VAP ��Э������
��:����USER ��Э������
*/
#ifdef _PRE_WLAN_FEATURE_11AX
oal_uint8 g_auc_avail_protocol_mode_etc[WLAN_PROTOCOL_BUTT][WLAN_PROTOCOL_BUTT] =
{
/**
--------11A--------------------------11B------------------11G---------------ONE_11G----------------TWO_11G--------------------HT------------------------VHT------------------HT_ONLY----------------VHT_ONLY----------HT_11G----------------HE
**/
    {WLAN_LEGACY_11A_MODE, WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT,     WLAN_PROTOCOL_BUTT,      WLAN_LEGACY_11A_MODE,   WLAN_LEGACY_11A_MODE,    WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT,   WLAN_LEGACY_11A_MODE}, //11A
    {WLAN_PROTOCOL_BUTT  , WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11B_MODE,   WLAN_LEGACY_11B_MODE,    WLAN_LEGACY_11B_MODE,   WLAN_LEGACY_11B_MODE,    WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT,   WLAN_LEGACY_11B_MODE}, //11B
    {WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT,   WLAN_LEGACY_11G_MODE, WLAN_LEGACY_11G_MODE,   WLAN_LEGACY_11G_MODE,    WLAN_LEGACY_11G_MODE,   WLAN_LEGACY_11G_MODE,    WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_LEGACY_11G_MODE, WLAN_LEGACY_11G_MODE}, //11G
    {WLAN_PROTOCOL_BUTT  , WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT},   //ONE_11G
    {WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT,   WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_TWO_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT},   //TWO_11G
    {WLAN_LEGACY_11A_MODE, WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_HT_MODE,           WLAN_HT_MODE,            WLAN_HT_ONLY_MODE,  WLAN_PROTOCOL_BUTT, WLAN_HT_11G_MODE,     WLAN_HT_MODE},         //HT
    {WLAN_LEGACY_11A_MODE, WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_HT_MODE,           WLAN_VHT_MODE,           WLAN_HT_ONLY_MODE,  WLAN_HT_ONLY_MODE,  WLAN_PROTOCOL_BUTT,   WLAN_VHT_MODE},        //VHT
    {WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT,     WLAN_PROTOCOL_BUTT,      WLAN_HT_ONLY_MODE,      WLAN_HT_ONLY_MODE,       WLAN_HT_ONLY_MODE,  WLAN_HT_ONLY_MODE,  WLAN_HT_ONLY_MODE,    WLAN_HT_ONLY_MODE},    //HT_ONLY
    {WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT,     WLAN_PROTOCOL_BUTT,      WLAN_PROTOCOL_BUTT,     WLAN_VHT_ONLY_MODE,      WLAN_PROTOCOL_BUTT, WLAN_VHT_ONLY_MODE, WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT},   //VHT_ONLY
    {WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT  , WLAN_LEGACY_11G_MODE, WLAN_LEGACY_11G_MODE,   WLAN_LEGACY_11G_MODE,    WLAN_HT_11G_MODE,       WLAN_PROTOCOL_BUTT,      WLAN_HT_ONLY_MODE,  WLAN_PROTOCOL_BUTT, WLAN_HT_11G_MODE,     WLAN_PROTOCOL_BUTT},   //HT_11G
    {WLAN_LEGACY_11A_MODE, WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_HT_MODE,           WLAN_VHT_MODE,           WLAN_HT_ONLY_MODE,  WLAN_VHT_ONLY_MODE, WLAN_PROTOCOL_BUTT,   WLAN_HE_MODE},         /*he*/
};
#else
oal_uint8 g_auc_avail_protocol_mode_etc[WLAN_PROTOCOL_BUTT][WLAN_PROTOCOL_BUTT] =
{
    {WLAN_LEGACY_11A_MODE, WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT,     WLAN_PROTOCOL_BUTT,      WLAN_LEGACY_11A_MODE,   WLAN_LEGACY_11A_MODE, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT},
    {WLAN_PROTOCOL_BUTT  , WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11B_MODE,   WLAN_LEGACY_11B_MODE,    WLAN_LEGACY_11B_MODE,   WLAN_LEGACY_11B_MODE, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT},
    {WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT,   WLAN_LEGACY_11G_MODE, WLAN_LEGACY_11G_MODE,   WLAN_LEGACY_11G_MODE,    WLAN_LEGACY_11G_MODE,   WLAN_LEGACY_11G_MODE, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_LEGACY_11G_MODE},
    {WLAN_PROTOCOL_BUTT  , WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_LEGACY_11G_MODE},
    {WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT,   WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_TWO_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_PROTOCOL_BUTT, WLAN_PROTOCOL_BUTT, WLAN_LEGACY_11G_MODE},
    {WLAN_LEGACY_11A_MODE, WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_HT_MODE,           WLAN_HT_MODE,         WLAN_HT_ONLY_MODE,  WLAN_PROTOCOL_BUTT, WLAN_HT_11G_MODE},
    {WLAN_LEGACY_11A_MODE, WLAN_LEGACY_11B_MODE, WLAN_LEGACY_11G_MODE, WLAN_MIXED_ONE_11G_MODE,WLAN_MIXED_ONE_11G_MODE, WLAN_HT_MODE,           WLAN_VHT_MODE,        WLAN_HT_ONLY_MODE,  WLAN_VHT_ONLY_MODE, WLAN_PROTOCOL_BUTT},
    {WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT,     WLAN_PROTOCOL_BUTT,      WLAN_HT_ONLY_MODE,      WLAN_HT_ONLY_MODE,    WLAN_HT_ONLY_MODE,  WLAN_HT_ONLY_MODE,  WLAN_HT_ONLY_MODE},
    {WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT  , WLAN_PROTOCOL_BUTT,     WLAN_PROTOCOL_BUTT,      WLAN_PROTOCOL_BUTT,     WLAN_VHT_ONLY_MODE,   WLAN_PROTOCOL_BUTT, WLAN_VHT_ONLY_MODE, WLAN_PROTOCOL_BUTT},
    {WLAN_PROTOCOL_BUTT,   WLAN_PROTOCOL_BUTT  , WLAN_LEGACY_11G_MODE, WLAN_LEGACY_11G_MODE,   WLAN_LEGACY_11G_MODE,    WLAN_HT_11G_MODE,       WLAN_PROTOCOL_BUTT,   WLAN_HT_ONLY_MODE,  WLAN_PROTOCOL_BUTT, WLAN_HT_11G_MODE},
};
#endif

oal_uint32  hmac_mgmt_tx_addba_timeout_etc(oal_void *p_arg);

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP

oal_void  hmac_rx_ba_session_decr_etc(hmac_vap_stru *pst_hmac_vap, oal_uint8 uc_tidno)
{
    if (0 == mac_mib_get_RxBASessionNumber(&pst_hmac_vap->st_vap_base_info))
    {
        OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA, "{hmac_rx_ba_session_decr_etc::tid[%d] rx_session already zero.}", uc_tidno);
        return;
    }

    mac_mib_decr_RxBASessionNumber(&pst_hmac_vap->st_vap_base_info);

    OAM_WARNING_LOG2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA, "{hmac_rx_ba_session_decr_etc::tid[%d] tx_session[%d] remove.}", uc_tidno, mac_mib_get_RxBASessionNumber(&pst_hmac_vap->st_vap_base_info));
}


oal_void  hmac_tx_ba_session_decr_etc(hmac_vap_stru *pst_hmac_vap, oal_uint8 uc_tidno)
{
    if (0 == mac_mib_get_TxBASessionNumber(&pst_hmac_vap->st_vap_base_info))
    {
        OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA, "{hmac_tx_ba_session_decr_etc::tid[%d] tx_session already zero.}", uc_tidno);
        return;
    }

    mac_mib_decr_TxBASessionNumber(&pst_hmac_vap->st_vap_base_info);

    OAM_WARNING_LOG2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA, "{hmac_tx_ba_session_decr_etc::tid[%d] tx_session[%d] remove.}", uc_tidno, mac_mib_get_TxBASessionNumber(&pst_hmac_vap->st_vap_base_info));
}

#else

oal_void  hmac_rx_ba_session_decr_etc(mac_device_stru *pst_mac_device, oal_uint8 uc_tidno)
{
    if (0 == pst_mac_device->uc_rx_ba_session_num)
    {
        OAM_WARNING_LOG1(0, OAM_SF_BA, "{hmac_rx_ba_session_decr_etc::tid[%d] rx_session already zero.}", uc_tidno);
        return;
    }

    pst_mac_device->uc_rx_ba_session_num--;

    OAM_WARNING_LOG2(0, OAM_SF_BA, "{hmac_rx_ba_session_decr_etc::tid[%d] rx_session[%d] remove.}", uc_tidno, pst_mac_device->uc_rx_ba_session_num);
}


oal_void  hmac_tx_ba_session_decr_etc(mac_device_stru *pst_mac_device, oal_uint8 uc_tidno)
{
    if (0 == pst_mac_device->uc_tx_ba_session_num)
    {
        OAM_WARNING_LOG1(0, OAM_SF_BA, "{hmac_tx_ba_session_decr_etc::tid[%d] tx_session already zero.}", uc_tidno);
        return;
    }

    pst_mac_device->uc_tx_ba_session_num--;

    OAM_WARNING_LOG2(0, OAM_SF_BA, "{hmac_tx_ba_session_decr_etc::tid[%d] tx_session[%d] remove.}", uc_tidno, pst_mac_device->uc_tx_ba_session_num);
}


OAL_STATIC OAL_INLINE oal_void  hmac_tx_ba_session_incr(mac_device_stru *pst_mac_device, oal_uint8 uc_tidno)
{
    pst_mac_device->uc_tx_ba_session_num++;

    OAM_WARNING_LOG2(0, OAM_SF_BA, "{hmac_tx_ba_session_incr::tid[%d] tx_session[%d] setup.}", uc_tidno, pst_mac_device->uc_tx_ba_session_num);
}
#endif

oal_uint16  hmac_mgmt_encap_addba_req_etc(
                hmac_vap_stru          *pst_vap,
                oal_uint8              *puc_data,
                dmac_ba_tx_stru        *pst_tx_ba,
                oal_uint8               uc_tid)
{
    oal_uint16  us_index = 0;
    oal_uint16  us_ba_param;
    if ((OAL_PTR_NULL == pst_vap) || (OAL_PTR_NULL == puc_data) || (OAL_PTR_NULL == pst_tx_ba))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_encap_addba_req_etc::null param.vap:%x data:%x ba:%x}",pst_vap,puc_data,pst_tx_ba);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /*************************************************************************/
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /*************************************************************************/

    /*************************************************************************/
    /*                Set the fields in the frame header                     */
    /*************************************************************************/

    /* Frame Control Field ��ֻ��Ҫ����Type/Subtypeֵ����������Ϊ0 */
    mac_hdr_set_frame_control(puc_data, WLAN_PROTOCOL_VERSION| WLAN_FC0_TYPE_MGT | WLAN_FC0_SUBTYPE_ACTION);

    /* DA is address of STA requesting association */
    oal_set_mac_addr(puc_data + 4, pst_tx_ba->puc_dst_addr);

    /* SA��ֵΪdot11MACAddress��ֵ */
    oal_set_mac_addr(puc_data + 10, mac_mib_get_StationID(&pst_vap->st_vap_base_info));

    oal_set_mac_addr(puc_data + 16, pst_vap->st_vap_base_info.auc_bssid);

    /*************************************************************************/
    /*                Set the contents of the frame body                     */
    /*************************************************************************/

    /* ������ָ��frame body��ʼλ�� */
    us_index = MAC_80211_FRAME_LEN;

    /* ����Category */
    puc_data[us_index++] = MAC_ACTION_CATEGORY_BA;

    /* ����Action */
    puc_data[us_index++] = MAC_BA_ACTION_ADDBA_REQ;

    /* ����Dialog Token */
    puc_data[us_index++] = pst_tx_ba->uc_dialog_token;

    /*
        ����Block Ack Parameter set field
        bit0 - AMSDU Allowed
        bit1 - Immediate or Delayed block ack
        bit2-bit5 - TID
        bit6-bit15 -  Buffer size
    */
    us_ba_param = pst_tx_ba->en_amsdu_supp;         /* bit0 */
    us_ba_param |= (pst_tx_ba->uc_ba_policy << 1);  /* bit1 */
    us_ba_param |= (uc_tid << 2);                   /* bit2 */

    us_ba_param |= (oal_uint16)(pst_tx_ba->us_baw_size << 6);   /* bit6 */


    puc_data[us_index++] = (oal_uint8)(us_ba_param & 0xFF);
    puc_data[us_index++] = (oal_uint8)((us_ba_param >> 8) & 0xFF);

    /* ����BlockAck timeout */
    puc_data[us_index++] = (oal_uint8)(pst_tx_ba->us_ba_timeout & 0xFF);
    puc_data[us_index++] = (oal_uint8)((pst_tx_ba->us_ba_timeout >> 8) & 0xFF);

    /*
        Block ack starting sequence number�ֶ���Ӳ������
        bit0-bit3 fragmentnumber
        bit4-bit15: sequence number
    */

    /* us_buf_seq�˴��ݲ���д����dmac��Ჹ����д */
    *(oal_uint16 *)&puc_data[us_index++] = 0;
    us_index++;
    //puc_data[us_index++] = 0;
    //puc_data[us_index++] = 0;

    /* ���ص�֡�����в�����FCS */
    return us_index;
}


oal_uint16  hmac_mgmt_encap_addba_rsp_etc(
                hmac_vap_stru      *pst_vap,
                oal_uint8          *puc_data,
                hmac_ba_rx_stru    *pst_addba_rsp,
                oal_uint8           uc_tid,
                oal_uint8           uc_status)
{
    oal_uint16  us_index    = 0;
    oal_uint16  us_ba_param  = 0;
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    hmac_user_stru *pst_hmac_user;
    hmac_user_btcoex_stru *pst_hmac_user_btcoex;
#endif
    if ((OAL_PTR_NULL == pst_vap) || (OAL_PTR_NULL == puc_data) || (OAL_PTR_NULL == pst_addba_rsp))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_encap_addba_req_etc::null param.vap:%x data:%x rsp:%x}",pst_vap,puc_data,pst_addba_rsp);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /*************************************************************************/
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /*************************************************************************/

    /*************************************************************************/
    /*                Set the fields in the frame header                     */
    /*************************************************************************/

    /* All the fields of the Frame Control Field are set to zero. Only the   */
    /* Type/Subtype field is set.                                            */
    mac_hdr_set_frame_control(puc_data, WLAN_PROTOCOL_VERSION| WLAN_FC0_TYPE_MGT | WLAN_FC0_SUBTYPE_ACTION);

    /* DA is address of STA requesting association */
    oal_set_mac_addr(puc_data + 4, pst_addba_rsp->puc_transmit_addr);

    /* SA is the dot11MACAddress */
    oal_set_mac_addr(puc_data + 10, mac_mib_get_StationID(&pst_vap->st_vap_base_info));

    oal_set_mac_addr(puc_data + 16, pst_vap->st_vap_base_info.auc_bssid);

    /*************************************************************************/
    /*                Set the contents of the frame body                     */
    /*************************************************************************/
    /*************************************************************************/
    /*             ADDBA Response Frame - Frame Body                         */
    /*    ---------------------------------------------------------------    */
    /*    | Category | Action | Dialog | Status  | Parameters | Timeout |    */
    /*    ---------------------------------------------------------------    */
    /*    | 1        | 1      | 1      | 2       | 2          | 2       |    */
    /*    ---------------------------------------------------------------    */
    /*                                                                       */
    /*************************************************************************/

    /* Initialize index and the frame data pointer */
    us_index= MAC_80211_FRAME_LEN;

    /* Action Category���� */
    puc_data[us_index++] = MAC_ACTION_CATEGORY_BA;

    /* �ض�Action�����µ�action��֡���� */
    puc_data[us_index++] = MAC_BA_ACTION_ADDBA_RSP;

    /* Dialog Token�����ã���Ҫ��req��copy���� */
    puc_data[us_index++] = pst_addba_rsp->uc_dialog_token;

    /* ״̬������ */
    puc_data[us_index++] = uc_status;
    puc_data[us_index++]  = 0;

    /* Block Ack Parameter���� */
    /* B0 - AMSDU Support, B1- Immediate or Delayed block ack */
    /* B2-B5 : TID, B6-B15: Buffer size */
    us_ba_param  = pst_addba_rsp->en_amsdu_supp;                    /* BIT0 */
    us_ba_param |= (pst_addba_rsp->uc_ba_policy << 1);              /* BIT1 */
    us_ba_param |= (uc_tid << 2);                                   /* BIT2 */
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    /* �ֶ����þۺϸ���������ɾ��BAʱ������64�ۺ� */
    pst_hmac_user = mac_vap_get_hmac_user_by_addr_etc(&(pst_vap->st_vap_base_info), pst_addba_rsp->puc_transmit_addr);

    if (OAL_PTR_NULL != pst_hmac_user)
    {
        if (OAL_TRUE == MAC_BTCOEX_CHECK_VALID_STA(&(pst_vap->st_vap_base_info)))
        {
            pst_hmac_user_btcoex = &(pst_hmac_user->st_hmac_user_btcoex);

            /* 1.�������û� */
            if (OAL_FALSE == pst_hmac_user_btcoex->st_hmac_btcoex_addba_req.en_ba_handle_allow)
            {
                if(BTCOEX_BLACKLIST_TPYE_FIX_BASIZE == HMAC_BTCOEX_GET_BLACKLIST_TYPE(pst_hmac_user))
                {
                    us_ba_param |= (oal_uint16)(BTCOEX_BLACKLIST_BA_SIZE_LIMIT << 6);
                }
                else
                {
                    /* ������ʱ��btcoex�ۺ�ҵ���ڽ���״̬������Ĭ�Ͼۺϸ����ָ�wifi���� */
                    us_ba_param |= (oal_uint16)(pst_addba_rsp->us_baw_size << 6);   /* BIT6 */
                }
            }
            /* 2.���津����ɾ�������չ��������� */
            else if((OAL_TRUE == pst_hmac_user_btcoex->en_delba_btcoex_trigger)&&
                (0 != pst_hmac_user_btcoex->us_ba_size))
            {
                us_ba_param |= (oal_uint16)(pst_hmac_user_btcoex->us_ba_size << 6);   /* BIT6 */
            }
            else
            {
                us_ba_param |= (oal_uint16)(pst_addba_rsp->us_baw_size << 6);   /* BIT6 */
            }
        }
        else
        {
            us_ba_param |= (oal_uint16)(pst_addba_rsp->us_baw_size << 6);   /* BIT6 */
        }
    }
    else
#endif
    {
        us_ba_param |= (oal_uint16)(pst_addba_rsp->us_baw_size << 6);   /* BIT6 */
    }

    puc_data[us_index++] = (oal_uint8)(us_ba_param & 0xFF);
    puc_data[us_index++] = (oal_uint8)((us_ba_param >> 8) & 0xFF);

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1151)
    puc_data[us_index++] = (oal_uint8)(pst_addba_rsp->us_ba_timeout & 0xFF);
    puc_data[us_index++] = (oal_uint8)((pst_addba_rsp->us_ba_timeout >> 8) & 0xFF);
#else
    puc_data[us_index++] = 0x00;
    puc_data[us_index++] = 0x00;
#endif

    /* ���ص�֡�����в�����FCS */
    return us_index;
}


oal_uint16  hmac_mgmt_encap_delba_etc(
                hmac_vap_stru                      *pst_vap,
                oal_uint8                          *puc_data,
                oal_uint8                          *puc_addr,
                oal_uint8                           uc_tid,
                mac_delba_initiator_enum_uint8      en_initiator,
                oal_uint8                           reason)
{
    oal_uint16  us_index = 0;

    if ((OAL_PTR_NULL == pst_vap) || (OAL_PTR_NULL == puc_data) || (OAL_PTR_NULL == puc_addr))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_encap_delba_etc::null param, %d %d %d.}", pst_vap, puc_data, puc_addr);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /*************************************************************************/
    /*                        Management Frame Format                        */
    /* --------------------------------------------------------------------  */
    /* |Frame Control|Duration|DA|SA|BSSID|Sequence Control|Frame Body|FCS|  */
    /* --------------------------------------------------------------------  */
    /* | 2           |2       |6 |6 |6    |2               |0 - 2312  |4  |  */
    /* --------------------------------------------------------------------  */
    /*                                                                       */
    /*************************************************************************/

    /*************************************************************************/
    /*                Set the fields in the frame header                     */
    /*************************************************************************/

    /* All the fields of the Frame Control Field are set to zero. Only the   */
    /* Type/Subtype field is set.                                            */
    mac_hdr_set_frame_control(puc_data, WLAN_PROTOCOL_VERSION| WLAN_FC0_TYPE_MGT | WLAN_FC0_SUBTYPE_ACTION);

    /* duration */
    puc_data[2] = 0;
    puc_data[3] = 0;

    /* DA is address of STA requesting association */
    oal_set_mac_addr(puc_data + 4, puc_addr);

    /* SA is the dot11MACAddress */
    oal_set_mac_addr(puc_data + 10, mac_mib_get_StationID(&pst_vap->st_vap_base_info));

    oal_set_mac_addr(puc_data + 16, pst_vap->st_vap_base_info.auc_bssid);

    /* seq control */
    puc_data[22] = 0;
    puc_data[23] = 0;

    /*************************************************************************/
    /*                Set the contents of the frame body                     */
    /*************************************************************************/

    /*************************************************************************/
    /*             DELBA Response Frame - Frame Body                         */
    /*        -------------------------------------------------              */
    /*        | Category | Action |  Parameters | Reason code |              */
    /*        -------------------------------------------------              */
    /*        | 1        | 1      |       2     | 2           |              */
    /*        -------------------------------------------------              */
    /*                          Parameters                                   */
    /*                  -------------------------------                      */
    /*                  | Reserved | Initiator |  TID  |                     */
    /*                  -------------------------------                      */
    /*             bit  |    11    |    1      |  4    |                     */
    /*                  --------------------------------                     */
    /*************************************************************************/
    /* Initialize index and the frame data pointer */
    us_index = MAC_80211_FRAME_LEN;

    /* Category */
    puc_data[us_index++] = MAC_ACTION_CATEGORY_BA;

    /* Action */
    puc_data[us_index++] = MAC_BA_ACTION_DELBA;

    /* DELBA parameter set */
    /* B0 - B10 -reserved */
    /* B11 - initiator */
    /* B12-B15 - TID */
    puc_data[us_index++]  = 0;
    puc_data[us_index]    = (oal_uint8)(en_initiator << 3);
    puc_data[us_index++] |= (oal_uint8)(uc_tid << 4);

    /* Reason field */
    /* Reason can be either of END_BA, QSTA_LEAVING, UNKNOWN_BA */
    puc_data[us_index++] = reason;
    puc_data[us_index++] = 0;

    /* ���ص�֡�����в�����FCS */
    return us_index;
}


oal_uint32  hmac_mgmt_tx_addba_req_etc(
                hmac_vap_stru              *pst_hmac_vap,
                hmac_user_stru             *pst_hmac_user,
                mac_action_mgmt_args_stru  *pst_action_args)
{
    mac_device_stru            *pst_device;
    mac_vap_stru               *pst_mac_vap;
    frw_event_mem_stru         *pst_event_mem;      /* �����¼����ص��ڴ�ָ�� */
    oal_netbuf_stru            *pst_addba_req;
    dmac_ba_tx_stru             st_tx_ba;
    oal_uint8                   uc_tidno;
    oal_uint16                  us_frame_len;
    frw_event_stru             *pst_hmac_to_dmac_ctx_event;
    dmac_tx_event_stru         *pst_tx_event;
    dmac_ctx_action_event_stru  st_wlan_ctx_action;
    oal_uint32                  ul_ret;
    mac_tx_ctl_stru            *pst_tx_ctl;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == pst_action_args))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_tx_addba_req_etc::null param, %d %d %d.}", pst_hmac_vap, pst_hmac_user, pst_action_args);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap = &(pst_hmac_vap->st_vap_base_info);
    if (MAC_VAP_STATE_BUTT == pst_mac_vap->en_vap_state)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_req_etc:: vap has been down/del, vap_state[%d].}",pst_mac_vap->en_vap_state);
        return OAL_FAIL;
    }

    /* ��ȡdevice�ṹ */
    pst_device = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_device)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_req_etc::pst_device null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����ADDBA_REQ����֡�ڴ� */
    pst_addba_req = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (OAL_PTR_NULL == pst_addba_req)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_req_etc::pst_addba_req null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAL_MEM_NETBUF_TRACE(pst_addba_req, OAL_TRUE);

    OAL_NETBUF_PREV(pst_addba_req) = OAL_PTR_NULL;
    OAL_NETBUF_NEXT(pst_addba_req) = OAL_PTR_NULL;

    uc_tidno = (oal_uint8)(pst_action_args->ul_arg1);
    /* ��tid��Ӧ��txBA�Ự״̬���� */
    oal_spin_lock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));

    pst_hmac_vap->uc_ba_dialog_token++;
    st_tx_ba.uc_dialog_token = pst_hmac_vap->uc_ba_dialog_token;  /* ��֤ba�Ự�����ܹ����� */
    st_tx_ba.us_baw_size     = (oal_uint8)(pst_action_args->ul_arg2);
    st_tx_ba.uc_ba_policy    = (oal_uint8)(pst_action_args->ul_arg3);
    st_tx_ba.us_ba_timeout   = (oal_uint16)(pst_action_args->ul_arg4);
    st_tx_ba.puc_dst_addr    = pst_hmac_user->st_user_base_info.auc_user_mac_addr;

    /* ���˶�AMPDU+AMSDU��֧�� */
    st_tx_ba.en_amsdu_supp   = (oal_bool_enum_uint8)mac_mib_get_AmsduPlusAmpduActive(pst_mac_vap);

    /*lint -e502*/
    if (OAL_FALSE == st_tx_ba.en_amsdu_supp)
    {
        HMAC_USER_SET_AMSDU_NOT_SUPPORT(pst_hmac_user, uc_tidno);
    }
    /*lint +e502*/
    else
    {
        HMAC_USER_SET_AMSDU_SUPPORT(pst_hmac_user, uc_tidno);
    }

    /* ���÷�װ����֡�ӿ� */
    us_frame_len = hmac_mgmt_encap_addba_req_etc(pst_hmac_vap, oal_netbuf_data(pst_addba_req), &st_tx_ba, uc_tidno);
    OAL_MEMZERO((oal_uint8*)&st_wlan_ctx_action, OAL_SIZEOF(st_wlan_ctx_action));
    /*��ֵҪ����Dmac����Ϣ*/
    st_wlan_ctx_action.us_frame_len        = us_frame_len;
    st_wlan_ctx_action.uc_hdr_len          = MAC_80211_FRAME_LEN;
    st_wlan_ctx_action.en_action_category  = MAC_ACTION_CATEGORY_BA;
    st_wlan_ctx_action.uc_action           = MAC_BA_ACTION_ADDBA_REQ;
    st_wlan_ctx_action.us_user_idx         = pst_hmac_user->st_user_base_info.us_assoc_id;
    st_wlan_ctx_action.uc_tidno            = uc_tidno;
    st_wlan_ctx_action.uc_dialog_token     = st_tx_ba.uc_dialog_token;
    st_wlan_ctx_action.uc_ba_policy        = st_tx_ba.uc_ba_policy;
    st_wlan_ctx_action.us_baw_size         = st_tx_ba.us_baw_size;
    st_wlan_ctx_action.us_ba_timeout       = st_tx_ba.us_ba_timeout;
    st_wlan_ctx_action.en_amsdu_supp       = st_tx_ba.en_amsdu_supp;

    oal_memcopy((oal_uint8 *)(oal_netbuf_data(pst_addba_req) + us_frame_len), (oal_uint8 *)&st_wlan_ctx_action, OAL_SIZEOF(dmac_ctx_action_event_stru));
    oal_netbuf_put(pst_addba_req, (us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru)));

    /* ��ʼ��CB */
    OAL_MEMZERO(oal_netbuf_cb(pst_addba_req), OAL_NETBUF_CB_SIZE());
    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_addba_req);
    MAC_GET_CB_MPDU_LEN(pst_tx_ctl) = us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru);
    MAC_GET_CB_FRAME_TYPE(pst_tx_ctl) = WLAN_CB_FRAME_TYPE_ACTION;
    MAC_GET_CB_FRAME_SUBTYPE(pst_tx_ctl) = WLAN_ACTION_BA_ADDBA_REQ;

    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_tx_event_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_req_etc::pst_event_mem null.}");
        oal_netbuf_free(pst_addba_req);
        oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_to_dmac_ctx_event = frw_get_event_stru(pst_event_mem);
    FRW_EVENT_HDR_INIT(&(pst_hmac_to_dmac_ctx_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_MGMT,
                       OAL_SIZEOF(dmac_tx_event_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    pst_tx_event = (dmac_tx_event_stru *)(pst_hmac_to_dmac_ctx_event->auc_event_data);
    pst_tx_event->pst_netbuf    = pst_addba_req;
    pst_tx_event->us_frame_len  = us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru);

    ul_ret = frw_event_dispatch_event_etc(pst_event_mem);
    if (ul_ret != OAL_SUCC)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_req_etc::frw_event_dispatch_event_etc failed[%d].}", ul_ret);
        oal_netbuf_free(pst_addba_req);
        FRW_EVENT_FREE(pst_event_mem);
        oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
        return ul_ret;
    }

    FRW_EVENT_FREE(pst_event_mem);

    /* ���¶�Ӧ��TID��Ϣ */
    pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.en_ba_status    = DMAC_BA_INPROGRESS;
    pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.uc_dialog_token = st_tx_ba.uc_dialog_token;
    pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.uc_ba_policy    = st_tx_ba.uc_ba_policy;

#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    mac_mib_incr_TxBASessionNumber(pst_mac_vap);
#else
    hmac_tx_ba_session_incr(pst_device, uc_tidno);
#endif
    /* ����ADDBA��ʱ��ʱ�� */
    FRW_TIMER_CREATE_TIMER(&pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_addba_timer,
                           hmac_mgmt_tx_addba_timeout_etc,
                           WLAN_ADDBA_TIMEOUT,
                           &pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_alarm_data,
                           OAL_FALSE,
                           OAM_MODULE_ID_HMAC,
                           pst_device->ul_core_id);

    /* ��tid��Ӧ��tx BA�Ự״̬���� */
    oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));

    return OAL_SUCC;
}


oal_uint32  hmac_mgmt_tx_addba_rsp_etc(
                hmac_vap_stru              *pst_hmac_vap,
                hmac_user_stru             *pst_hmac_user,
                hmac_ba_rx_stru            *pst_ba_rx_info,
                oal_uint8                   uc_tid,
                oal_uint8                   uc_status)
{
    mac_device_stru                *pst_device;
    mac_vap_stru                   *pst_mac_vap;
    frw_event_mem_stru             *pst_event_mem;      /* �����¼����ص��ڴ�ָ�� */
    frw_event_stru                 *pst_hmac_to_dmac_ctx_event;
    dmac_tx_event_stru             *pst_tx_event;
    dmac_ctx_action_event_stru      st_wlan_ctx_action;
    oal_netbuf_stru                *pst_addba_rsp;
    oal_uint16                      us_frame_len;
    oal_uint32                      ul_ret = OAL_SUCC;
    mac_tx_ctl_stru                *pst_tx_ctl;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == pst_ba_rx_info))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_tx_addba_rsp_etc::send addba rsp failed, null param, %d %d %d.}", pst_hmac_vap, pst_hmac_user, pst_ba_rx_info);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap = &(pst_hmac_vap->st_vap_base_info);
    if (MAC_VAP_STATE_BUTT == pst_mac_vap->en_vap_state)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_rsp_etc:: vap has been down/del, vap_state[%d].}",pst_mac_vap->en_vap_state);
        return OAL_FAIL;
    }

    /* ��ȡdevice�ṹ */
    pst_device = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);

    if (OAL_PTR_NULL == pst_device)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_rsp_etc::send addba rsp failed, pst_device null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����ADDBA_RSP����֡�ڴ� */
    pst_addba_rsp = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (OAL_PTR_NULL == pst_addba_rsp)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_rsp_etc::send addba rsp failed, pst_addba_rsp mem alloc failed.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAL_MEM_NETBUF_TRACE(pst_addba_rsp, OAL_TRUE);

    OAL_NETBUF_PREV(pst_addba_rsp) = OAL_PTR_NULL;
    OAL_NETBUF_NEXT(pst_addba_rsp) = OAL_PTR_NULL;

    us_frame_len = hmac_mgmt_encap_addba_rsp_etc(pst_hmac_vap, oal_netbuf_data(pst_addba_rsp), pst_ba_rx_info, uc_tid, uc_status);
    OAL_MEMZERO((oal_uint8*)&st_wlan_ctx_action, OAL_SIZEOF(st_wlan_ctx_action));
    st_wlan_ctx_action.en_action_category  = MAC_ACTION_CATEGORY_BA;
    st_wlan_ctx_action.uc_action           = MAC_BA_ACTION_ADDBA_RSP;
    st_wlan_ctx_action.uc_hdr_len          = MAC_80211_FRAME_LEN;
    st_wlan_ctx_action.us_baw_size         = pst_ba_rx_info->us_baw_size;
    st_wlan_ctx_action.us_frame_len        = us_frame_len;
    st_wlan_ctx_action.us_user_idx         = pst_hmac_user->st_user_base_info.us_assoc_id;
    st_wlan_ctx_action.uc_tidno            = uc_tid;
    st_wlan_ctx_action.uc_status           = uc_status;
    st_wlan_ctx_action.us_ba_timeout       = pst_ba_rx_info->us_ba_timeout;
    st_wlan_ctx_action.en_back_var         = pst_ba_rx_info->en_back_var;
    st_wlan_ctx_action.uc_lut_index        = pst_ba_rx_info->uc_lut_index;
    st_wlan_ctx_action.us_baw_start        = pst_ba_rx_info->us_baw_start;
    st_wlan_ctx_action.uc_ba_policy        = pst_ba_rx_info->uc_ba_policy;

    oal_memcopy((oal_uint8 *)(oal_netbuf_data(pst_addba_rsp) + us_frame_len), (oal_uint8 *)&st_wlan_ctx_action, OAL_SIZEOF(dmac_ctx_action_event_stru));
    oal_netbuf_put(pst_addba_rsp, (us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru)));

    /* ��дnetbuf��cb�ֶΣ������͹���֡�ͷ�����ɽӿ�ʹ�� */
    OAL_MEMZERO(oal_netbuf_cb(pst_addba_rsp), OAL_NETBUF_CB_SIZE());
    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_addba_rsp);
    MAC_GET_CB_TX_USER_IDX(pst_tx_ctl)   = pst_hmac_user->st_user_base_info.us_assoc_id;
    MAC_GET_CB_WME_TID_TYPE(pst_tx_ctl) = uc_tid;
    MAC_GET_CB_MPDU_LEN(pst_tx_ctl) = us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru);
    MAC_GET_CB_FRAME_TYPE(pst_tx_ctl) = WLAN_CB_FRAME_TYPE_ACTION;
    MAC_GET_CB_FRAME_SUBTYPE(pst_tx_ctl) = WLAN_ACTION_BA_ADDBA_RSP;


    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_tx_event_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_rsp_etc::send addba rsp failed, pst_event_mem mem alloc failed.}");
        oal_netbuf_free(pst_addba_rsp);

        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_to_dmac_ctx_event = frw_get_event_stru(pst_event_mem);
    FRW_EVENT_HDR_INIT(&(pst_hmac_to_dmac_ctx_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_MGMT,
                       OAL_SIZEOF(dmac_tx_event_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    /*��д�¼�payload */
    pst_tx_event = (dmac_tx_event_stru *)(pst_hmac_to_dmac_ctx_event->auc_event_data);
    pst_tx_event->pst_netbuf    = pst_addba_rsp;
    pst_tx_event->us_frame_len  = us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru);


    ul_ret = frw_event_dispatch_event_etc(pst_event_mem);
    if (ul_ret != OAL_SUCC)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_rsp_etc::send addba rsp failed, frw_event_dispatch_event_etc failed[%d].}", ul_ret);
        oal_netbuf_free(pst_addba_rsp);
    }
    else
    {
        pst_ba_rx_info->en_ba_status = DMAC_BA_COMPLETE;
    }

    FRW_EVENT_FREE(pst_event_mem);
    return ul_ret;
}

oal_uint32  hmac_mgmt_tx_delba_etc(
                hmac_vap_stru              *pst_hmac_vap,
                hmac_user_stru             *pst_hmac_user,
                mac_action_mgmt_args_stru  *pst_action_args)
{
    mac_device_stru                  *pst_device;
    mac_vap_stru                     *pst_mac_vap;
    frw_event_mem_stru               *pst_event_mem;      /* �����¼����ص��ڴ�ָ�� */
    oal_netbuf_stru                  *pst_delba;
    oal_uint16                        us_frame_len;
    frw_event_stru                   *pst_hmac_to_dmac_ctx_event;
    dmac_tx_event_stru               *pst_tx_event;
    dmac_ctx_action_event_stru        st_wlan_ctx_action;
    mac_delba_initiator_enum_uint8    en_initiator;
    oal_uint32                        ul_ret;
    mac_tx_ctl_stru                  *pst_tx_ctl;
    oal_uint8                         uc_tidno;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == pst_action_args))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_tx_delba_etc::null param, %d %d %d.}", pst_hmac_vap, pst_hmac_user, pst_action_args);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap = &(pst_hmac_vap->st_vap_base_info);
    if (MAC_VAP_STATE_BUTT == pst_mac_vap->en_vap_state)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_delba_etc:: vap has been down/del, vap_state[%d].}",pst_mac_vap->en_vap_state);
        return OAL_FAIL;
    }

    /* ��ȡdevice�ṹ */
    pst_device = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);

    if (OAL_PTR_NULL == pst_device)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_delba_etc::pst_device null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    en_initiator = (mac_delba_initiator_enum_uint8)(pst_action_args->ul_arg2);
    uc_tidno     = (oal_uint8)(pst_action_args->ul_arg1);

    /* ��tid��Ӧ��tx BA�Ự״̬���� */
    oal_spin_lock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));

    if(MAC_ORIGINATOR_DELBA == en_initiator)
    {
        if(DMAC_BA_INIT == pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.en_ba_status)
        {
            oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
            return OAL_SUCC;
        }
    }

    /* ����DEL_BA����֡�ڴ� */
    pst_delba = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (OAL_PTR_NULL == pst_delba)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_delba_etc::pst_delba null.}");
        oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAL_MEM_NETBUF_TRACE(pst_delba, OAL_TRUE);

    OAL_NETBUF_PREV(pst_delba) = OAL_PTR_NULL;
    OAL_NETBUF_NEXT(pst_delba) = OAL_PTR_NULL;

    /* ���÷�װ����֡�ӿ� */
    us_frame_len = hmac_mgmt_encap_delba_etc(pst_hmac_vap, (oal_uint8 *)OAL_NETBUF_HEADER(pst_delba), pst_action_args->puc_arg5, uc_tidno, en_initiator, (oal_uint8)pst_action_args->ul_arg3);
    OAL_MEMZERO((oal_uint8*)&st_wlan_ctx_action, OAL_SIZEOF(st_wlan_ctx_action));
    st_wlan_ctx_action.us_frame_len        = us_frame_len;
    st_wlan_ctx_action.uc_hdr_len          = MAC_80211_FRAME_LEN;
    st_wlan_ctx_action.en_action_category  = MAC_ACTION_CATEGORY_BA;
    st_wlan_ctx_action.uc_action           = MAC_BA_ACTION_DELBA;
    st_wlan_ctx_action.us_user_idx         = pst_hmac_user->st_user_base_info.us_assoc_id;
    st_wlan_ctx_action.uc_tidno            = uc_tidno;
    st_wlan_ctx_action.uc_initiator        = en_initiator;

    oal_memcopy((oal_uint8 *)(oal_netbuf_data(pst_delba) + us_frame_len), (oal_uint8 *)&st_wlan_ctx_action, OAL_SIZEOF(dmac_ctx_action_event_stru));
    oal_netbuf_put(pst_delba, (us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru)));

#ifdef _PRE_WLAN_1103_CHR
    hmac_chr_set_del_ba_info(uc_tidno, (oal_uint16)pst_action_args->ul_arg3);
#endif
    /* ��ʼ��CB */
    OAL_MEMZERO(oal_netbuf_cb(pst_delba), OAL_NETBUF_CB_SIZE());
    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_delba);
    MAC_GET_CB_MPDU_LEN(pst_tx_ctl)      = us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru);
    MAC_GET_CB_FRAME_TYPE(pst_tx_ctl)    = WLAN_CB_FRAME_TYPE_ACTION;
    MAC_GET_CB_FRAME_SUBTYPE(pst_tx_ctl) = WLAN_ACTION_BA_DELBA;

    /* ���¼�����DMACģ�鷢�� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_tx_event_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_delba_etc::pst_event_mem null.}");
        /* �ͷŹ���֡�ڴ浽netbuf�ڴ�� */
        oal_netbuf_free(pst_delba);
        /* ��tid��Ӧ��tx BA�Ự״̬���� */
        oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����¼�ָ�� */
    pst_hmac_to_dmac_ctx_event = frw_get_event_stru(pst_event_mem);

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_hmac_to_dmac_ctx_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_MGMT,
                       OAL_SIZEOF(dmac_tx_event_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    /*��д�¼�payload */
    pst_tx_event = (dmac_tx_event_stru *)(pst_hmac_to_dmac_ctx_event->auc_event_data);
    pst_tx_event->pst_netbuf    = pst_delba;
    pst_tx_event->us_frame_len  = us_frame_len + OAL_SIZEOF(dmac_ctx_action_event_stru);

    /* �ַ� */
    ul_ret = frw_event_dispatch_event_etc(pst_event_mem);
    if (ul_ret != OAL_SUCC)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_delba_etc::frw_event_dispatch_event_etc failed[%d].}", ul_ret);
        oal_netbuf_free(pst_delba);
        FRW_EVENT_FREE(pst_event_mem);
        oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
        return ul_ret;
    }

    FRW_EVENT_FREE(pst_event_mem);

    if (MAC_RECIPIENT_DELBA == en_initiator)
    {
        /* ���¶�Ӧ��TID��Ϣ */
        hmac_ba_reset_rx_handle_etc(pst_device, &pst_hmac_user->ast_tid_info[uc_tidno].pst_ba_rx_info, uc_tidno, OAL_FALSE);
    }
    else
    {
        /* ���¶�Ӧ��TID��Ϣ */
        pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.en_ba_status = DMAC_BA_INIT;
        pst_hmac_user->auc_ba_flag[uc_tidno] = 0;

#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
        hmac_tx_ba_session_decr_etc(pst_hmac_vap, pst_hmac_user->ast_tid_info[uc_tidno].uc_tid_no);
#else
        hmac_tx_ba_session_decr_etc(pst_device, pst_hmac_user->ast_tid_info[uc_tidno].uc_tid_no);
#endif
        /* ��ԭ����AMPDU��AMSDU��֧����� */
        HMAC_USER_SET_AMSDU_SUPPORT(pst_hmac_user, uc_tidno);
    }

    /* ��tid��Ӧ��tx BA�Ự״̬���� */
    oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));

    return OAL_SUCC;
}


oal_uint32  hmac_mgmt_rx_addba_req_etc(
                hmac_vap_stru              *pst_hmac_vap,
                hmac_user_stru             *pst_hmac_user,
                oal_uint8                  *puc_payload)
{
    mac_device_stru                *pst_device;
    mac_vap_stru                   *pst_mac_vap;
    oal_uint8                       uc_tid = 0;
    oal_uint8                       uc_status = MAC_SUCCESSFUL_STATUSCODE;
    oal_uint8                       uc_reorder_index;
    hmac_ba_rx_stru                *pst_ba_rx_stru;
    oal_uint32                      ul_ret;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == puc_payload))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_rx_addba_req_etc::addba req receive failed, null param, 0x%x 0x%x 0x%x.}", pst_hmac_vap, pst_hmac_user, puc_payload);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap = &(pst_hmac_vap->st_vap_base_info);

    /* 11n���������ſɽ���ampdu */
    if ((!(pst_mac_vap->en_protocol >= WLAN_HT_MODE)) || (!(pst_hmac_user->st_user_base_info.en_protocol_mode >= WLAN_HT_MODE)))
    {
        OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_BA,
                      "{hmac_mgmt_rx_addba_req_etc::ampdu is not supprot or not open, vap protocol mode = %d, user protocol mode = %d.}",
                       pst_mac_vap->en_protocol, pst_hmac_user->st_user_base_info.en_protocol_mode);
        return OAL_SUCC;
    }

    /* ��ȡdevice�ṹ */
    pst_device = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);

    if (OAL_PTR_NULL == pst_device)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_req_etc::addba req receive failed, pst_device null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /******************************************************************/
    /*       ADDBA Request Frame - Frame Body                         */
    /* ---------------------------------------------------------------*/
    /* | Category | Action | Dialog | Parameters | Timeout | SSN     |*/
    /* ---------------------------------------------------------------*/
    /* | 1        | 1      | 1      | 2          | 2       | 2       |*/
    /* ---------------------------------------------------------------*/
    /*                                                                */
    /******************************************************************/

    uc_tid        = (puc_payload[3] & 0x3C) >> 2;
    if (uc_tid >= WLAN_TID_MAX_NUM)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_req_etc::addba req receive failed, tid %d overflow.}", uc_tid);
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }

    if (OAL_PTR_NULL != pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_req_etc::addba req received, but tid [%d] already set up.}", uc_tid);
        hmac_ba_reset_rx_handle_etc(pst_device, &pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info, uc_tid, OAL_FALSE);
        //return OAL_SUCC;
    }

    pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info = (hmac_ba_rx_stru *)OAL_MEM_ALLOC(OAL_MEM_POOL_ID_LOCAL, (oal_uint16)OAL_SIZEOF(hmac_ba_rx_stru), OAL_TRUE);
    if (OAL_PTR_NULL == pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_req_etc::addba req receive failed, pst_ba_rx_hdl mem alloc failed.tid[%d]}", uc_tid);

        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_ba_rx_stru = pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info;
    pst_ba_rx_stru->en_ba_status = DMAC_BA_INIT;
    pst_ba_rx_stru->uc_dialog_token = puc_payload[2];

    /*��ʼ��reorder����*/
    for (uc_reorder_index = 0; uc_reorder_index < WLAN_AMPDU_RX_BUFFER_SIZE; uc_reorder_index++)
    {
        pst_ba_rx_stru->ast_re_order_list[uc_reorder_index].in_use     = 0;
        pst_ba_rx_stru->ast_re_order_list[uc_reorder_index].us_seq_num = 0;
        pst_ba_rx_stru->ast_re_order_list[uc_reorder_index].uc_num_buf = 0;
        oal_netbuf_list_head_init(&(pst_ba_rx_stru->ast_re_order_list[uc_reorder_index].st_netbuf_head));
    }

    /*��ʼ�����մ���*/
    pst_ba_rx_stru->us_baw_start = (puc_payload[7] >> 4) | (puc_payload[8] << 4);
    pst_ba_rx_stru->us_baw_size  = (puc_payload[3] & 0xC0) >> 6;
    pst_ba_rx_stru->us_baw_size |= (puc_payload[4] << 2);
    if ((0 == pst_ba_rx_stru->us_baw_size) || (pst_ba_rx_stru->us_baw_size > WLAN_AMPDU_RX_BUFFER_SIZE))
    {
        pst_ba_rx_stru->us_baw_size = WLAN_AMPDU_RX_BUFFER_SIZE;
    }

    if (1 == pst_ba_rx_stru->us_baw_size)
    {
        pst_ba_rx_stru->us_baw_size = WLAN_AMPDU_RX_BUFFER_SIZE;
    }
    /* ��������ֶ����ý��վۺϸ��� */
    if(OAL_TRUE == g_st_rx_buffer_size_stru.uc_rx_buffer_size_set_en)
    {
        pst_ba_rx_stru->us_baw_size = g_st_rx_buffer_size_stru.uc_rx_buffer_size;
    }
    pst_ba_rx_stru->us_baw_end   = DMAC_BA_SEQ_ADD(pst_ba_rx_stru->us_baw_start, (pst_ba_rx_stru->us_baw_size - 1));
    pst_ba_rx_stru->us_baw_tail  = DMAC_BA_SEQNO_SUB(pst_ba_rx_stru->us_baw_start, 1);
    pst_ba_rx_stru->us_baw_head  = DMAC_BA_SEQNO_SUB(pst_ba_rx_stru->us_baw_start, HMAC_BA_BMP_SIZE);
    pst_ba_rx_stru->uc_mpdu_cnt  = 0;
    pst_ba_rx_stru->en_is_ba     = OAL_TRUE;  //Ba session is processing

    /*��ʼ����ʱ����Դ*/
    pst_ba_rx_stru->st_alarm_data.pst_ba           = pst_ba_rx_stru;
    pst_ba_rx_stru->st_alarm_data.us_mac_user_idx  = pst_hmac_user->st_user_base_info.us_assoc_id;
    pst_ba_rx_stru->st_alarm_data.uc_vap_id        = pst_mac_vap->uc_vap_id;
    pst_ba_rx_stru->st_alarm_data.uc_tid           = uc_tid;
    pst_ba_rx_stru->st_alarm_data.us_timeout_times = 0;
    pst_ba_rx_stru->st_alarm_data.en_direction     = MAC_RECIPIENT_DELBA;
    pst_ba_rx_stru->en_timer_triggered             = OAL_FALSE;

    oal_spin_lock_init(&pst_ba_rx_stru->st_ba_lock);

    /*Ba�Ự������ʼ��*/
    pst_ba_rx_stru->us_ba_timeout = puc_payload[5] | (puc_payload[6] << 8);
    pst_ba_rx_stru->en_amsdu_supp = (pst_hmac_vap->bit_rx_ampduplusamsdu_active ? OAL_TRUE : OAL_FALSE);
    pst_ba_rx_stru->en_back_var   = MAC_BACK_COMPRESSED;
    pst_ba_rx_stru->puc_transmit_addr = pst_hmac_user->st_user_base_info.auc_user_mac_addr;
    pst_ba_rx_stru->uc_ba_policy      = (puc_payload[3] & 0x02) >> 1;

#ifndef _PRE_PROFILING_MODE
    /*profiling�����У����ն˲�ɾ��ba*/
    FRW_TIMER_CREATE_TIMER(&(pst_hmac_user->ast_tid_info[uc_tid].st_ba_timer),
                           hmac_ba_timeout_fn_etc,
                           pst_hmac_vap->us_rx_timeout[WLAN_WME_TID_TO_AC(uc_tid)],
                           &(pst_ba_rx_stru->st_alarm_data),
                           OAL_FALSE,
                           OAM_MODULE_ID_HMAC,
                           pst_device->ul_core_id);
#endif
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    mac_mib_incr_RxBASessionNumber(pst_mac_vap);
#else
    pst_device->uc_rx_ba_session_num++;
#endif

    /* �жϽ����ܷ�ɹ� */
    uc_status = hmac_mgmt_check_set_rx_ba_ok_etc(pst_hmac_vap, pst_hmac_user, pst_ba_rx_stru, pst_device, &(pst_hmac_user->ast_tid_info[uc_tid]));
    if (MAC_SUCCESSFUL_STATUSCODE == uc_status)
    {
        pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info->en_ba_status = DMAC_BA_INPROGRESS;
    }

    ul_ret = hmac_mgmt_tx_addba_rsp_etc(pst_hmac_vap, pst_hmac_user, pst_ba_rx_stru, uc_tid, uc_status);

    OAM_WARNING_LOG4(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_req_etc::process addba req receive and send addba rsp, uc_tid[%d], uc_status[%d], baw_size[%d], RXAMSDU[%d]}\r\n",
      uc_tid, uc_status, pst_ba_rx_stru->us_baw_size, pst_ba_rx_stru->en_amsdu_supp);

    if ((MAC_SUCCESSFUL_STATUSCODE != uc_status) || (OAL_SUCC != ul_ret))
    {
        /*pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info�޸�Ϊ�ں������ÿգ�������
                ����һ��*/
        hmac_ba_reset_rx_handle_etc(pst_device, &pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info, uc_tid, OAL_FALSE);
    }

    return OAL_SUCC;
}


oal_uint32  hmac_mgmt_rx_addba_rsp_etc(
                hmac_vap_stru              *pst_hmac_vap,
                hmac_user_stru             *pst_hmac_user,
                oal_uint8                  *puc_payload)
{
    mac_device_stru            *pst_mac_device;
    mac_vap_stru               *pst_mac_vap;
    frw_event_mem_stru         *pst_event_mem;      /* �����¼����ص��ڴ�ָ�� */
    frw_event_stru             *pst_hmac_to_dmac_crx_sync;
    dmac_ctx_action_event_stru *pst_rx_addba_rsp_event;
    oal_uint8                   uc_tidno;
    hmac_tid_stru              *pst_tid;
    oal_uint8                   uc_dialog_token;
    oal_uint8                   uc_ba_policy;
    oal_uint16                  us_baw_size;
    oal_uint8                   uc_ba_status;
    oal_bool_enum_uint8         en_amsdu_supp;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == puc_payload))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::null param, %d %d %d.}", pst_hmac_vap, pst_hmac_user, puc_payload);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap = &(pst_hmac_vap->st_vap_base_info);

    /* ��ȡdevice�ṹ */
    pst_mac_device = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_device))
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::pst_mac_device null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /******************************************************************/
    /*       ADDBA Response Frame - Frame Body                        */
    /* ---------------------------------------------------------------*/
    /* | Category | Action | Dialog | Status  | Parameters | Timeout |*/
    /* ---------------------------------------------------------------*/
    /* | 1        | 1      | 1      | 2       | 2          | 2       |*/
    /* ---------------------------------------------------------------*/
    /*                                                                */
    /******************************************************************/

    uc_tidno = (puc_payload[5] & 0x3C) >> 2;
    /* Э��֧��tidΪ0~15,02ֻ֧��tid0~7 */
    if(uc_tidno >= WLAN_TID_MAX_NUM)
    {
        /* ����tid > 7��respֱ�Ӻ��� */
        OAM_WARNING_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::addba rsp tid[%d]} token[%d] state[%d]", uc_tidno, puc_payload[2], puc_payload[3]);
        return OAL_SUCC;
    }

    uc_dialog_token = puc_payload[2];
    uc_ba_status    = puc_payload[3];
    uc_ba_policy    = ((puc_payload[5] & 0x02) >> 1);
    en_amsdu_supp   = puc_payload[5] & BIT0;

    pst_tid  = &(pst_hmac_user->ast_tid_info[uc_tidno]);

    /* ��tid��Ӧ��tx BA�Ự״̬���� */
    oal_spin_lock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));

     /* BA״̬�ɹ�����token��policy��ƥ�䣬��Ҫɾ���ۺ� */
    
    if ((DMAC_BA_INPROGRESS == pst_tid->st_ba_tx_info.en_ba_status) && (MAC_SUCCESSFUL_STATUSCODE == uc_ba_status))
    {
        if ((uc_dialog_token != pst_tid->st_ba_tx_info.uc_dialog_token) || (uc_ba_policy != pst_tid->st_ba_tx_info.uc_ba_policy))
        {
            /* ��tid��Ӧ��tx BA�Ự״̬���� */
            oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::addba rsp tid[%d],status SUCC,but token/policy wr}", uc_tidno);
            OAM_WARNING_LOG4(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::rsp policy[%d],req policy[%d], rsp dialog[%d], req dialog[%d]}",
                  uc_ba_policy, pst_tid->st_ba_tx_info.uc_ba_policy, uc_dialog_token, pst_tid->st_ba_tx_info.uc_dialog_token);
            return OAL_SUCC;
        }
    }

    /* ֹͣ��ʱ�� */
    if (OAL_TRUE == pst_tid->st_ba_tx_info.st_addba_timer.en_is_registerd)
    {
        FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&pst_tid->st_ba_tx_info.st_addba_timer);
    }

    if (DMAC_BA_INIT == pst_tid->st_ba_tx_info.en_ba_status)
    {
        /* ��tid��Ӧ��tx BA�Ự״̬���� */
        oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
        hmac_tx_ba_del(pst_hmac_vap, pst_hmac_user, uc_tidno);
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::addba rsp is received when ba status is DMAC_BA_INIT.tid[%d]}", uc_tidno);
        return OAL_SUCC;
    }

    if (DMAC_BA_COMPLETE == pst_tid->st_ba_tx_info.en_ba_status)
    {
        OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::addba rsp is received when ba status is DMAC_BA_COMPLETE or uc_dialog_token wrong.tid[%d], status[%d]}",
          uc_tidno, pst_tid->st_ba_tx_info.en_ba_status);
        /* ��tid��Ӧ��tx BA�Ự״̬���� */
        oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
        return OAL_SUCC;
    }



    /* ���¼���DMAC���� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_ctx_action_event_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::pst_event_mem null.}");
        oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����¼�ָ�� */
    pst_hmac_to_dmac_crx_sync = frw_get_event_stru(pst_event_mem);

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_hmac_to_dmac_crx_sync->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_BA_SYNC,
                       OAL_SIZEOF(dmac_ctx_action_event_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    /* ��ȡ֡����Ϣ������DMAC��ͬ������д�¼�payload */
    pst_rx_addba_rsp_event = (dmac_ctx_action_event_stru *)(pst_hmac_to_dmac_crx_sync->auc_event_data);
    pst_rx_addba_rsp_event->en_action_category = MAC_ACTION_CATEGORY_BA;
    pst_rx_addba_rsp_event->uc_action          = MAC_BA_ACTION_ADDBA_RSP;
    pst_rx_addba_rsp_event->us_user_idx        = pst_hmac_user->st_user_base_info.us_assoc_id;
    pst_rx_addba_rsp_event->uc_status          = uc_ba_status;
    pst_rx_addba_rsp_event->uc_tidno           = uc_tidno;
    pst_rx_addba_rsp_event->uc_dialog_token    = uc_dialog_token;
    pst_rx_addba_rsp_event->uc_ba_policy     = uc_ba_policy;
    pst_rx_addba_rsp_event->us_ba_timeout    = puc_payload[7] | (puc_payload[8] << 8);
    pst_rx_addba_rsp_event->en_amsdu_supp    = en_amsdu_supp;

    us_baw_size      = (oal_uint16)(((puc_payload[5] & 0xC0) >> 6) | (puc_payload[6] << 2));
    if ((0 == us_baw_size) || (us_baw_size > WLAN_AMPDU_TX_MAX_BUF_SIZE))
    {
        us_baw_size = WLAN_AMPDU_TX_MAX_BUF_SIZE;
    }

    pst_rx_addba_rsp_event->us_baw_size      = us_baw_size;

    /* �ַ� */
    frw_event_dispatch_event_etc(pst_event_mem);

    /* �ͷ��¼��ڴ� */
    FRW_EVENT_FREE(pst_event_mem);

    /* �����¼����ٴ���host BA�������ֹ�첽����ADDBA REQ */
    if (MAC_SUCCESSFUL_STATUSCODE == uc_ba_status)
    {
        /* ����hmacģ���Ӧ��BA�������Ϣ */
        pst_tid->st_ba_tx_info.en_ba_status     = DMAC_BA_COMPLETE;
        pst_tid->st_ba_tx_info.uc_addba_attemps = 0;

        /*lint -e502*/
        if (en_amsdu_supp && mac_mib_get_AmsduPlusAmpduActive(pst_mac_vap))
        {
            HMAC_USER_SET_AMSDU_SUPPORT(pst_hmac_user, uc_tidno);
        }
        else
        {
            HMAC_USER_SET_AMSDU_NOT_SUPPORT(pst_hmac_user, uc_tidno);
        }
        /*lint +e502*/
    }
    else/* BA���ܾ� */
    {
        OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_addba_rsp_etc::addba rsp  status err[%d].tid[%d],DEL BA.}",uc_ba_status, uc_tidno);

#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
        hmac_tx_ba_session_decr_etc(pst_hmac_vap, uc_tidno);
#else
        hmac_tx_ba_session_decr_etc(pst_mac_device, uc_tidno);
#endif

        /* �����¼�ɾ��dmac��BA�����������HMACģ����Ϣ��ȷ��ɾ��dmac ba�¼�����һ��ADDBA REQ�¼�֮ǰ����dmac */
        pst_tid->st_ba_tx_info.en_ba_status = DMAC_BA_INIT;
    }

    /* ��tid��Ӧ��tx BA�Ự״̬���� */
    oal_spin_unlock_bh(&(pst_hmac_user->ast_tid_info[uc_tidno].st_ba_tx_info.st_ba_status_lock));

    return OAL_SUCC;
}


oal_uint32  hmac_mgmt_rx_delba_etc(
                hmac_vap_stru    *pst_hmac_vap,
                hmac_user_stru   *pst_hmac_user,
                oal_uint8        *puc_payload)
{
    frw_event_mem_stru           *pst_event_mem;      /* �����¼����ص��ڴ�ָ�� */
    frw_event_stru               *pst_hmac_to_dmac_crx_sync;
    dmac_ctx_action_event_stru   *pst_wlan_crx_action;
    mac_device_stru              *pst_device;
    hmac_tid_stru                *pst_tid;
    oal_uint8                     uc_tid;
    oal_uint8                     uc_initiator;
    oal_uint16                    us_reason;

    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == puc_payload)))
    {
        OAM_ERROR_LOG3(0, OAM_SF_BA, "{hmac_mgmt_rx_delba_etc::null param, %d %d %d.}", pst_hmac_vap, pst_hmac_user, puc_payload);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡdevice�ṹ */
    pst_device = mac_res_get_dev_etc(pst_hmac_vap->st_vap_base_info.uc_device_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_device))
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_delba_etc::pst_device null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /************************************************/
    /*       DELBA Response Frame - Frame Body      */
    /* -------------------------------------------- */
    /* | Category | Action | Parameters | Reason  | */
    /* -------------------------------------------- */
    /* | 1        | 1      | 2          | 2       | */
    /* -------------------------------------------- */
    /*                                              */
    /************************************************/
    uc_tid       = (puc_payload[3] & 0xF0) >> 4;
    uc_initiator = (puc_payload[3] & 0x08) >> 3;
    us_reason    = (puc_payload[4] & 0xFF) | ((puc_payload[5] << 8) & 0xFF00);

    /* tid��������������Խ�� */
    if (uc_tid >= WLAN_TID_MAX_NUM)
    {
        OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_delba_etc::delba receive failed, tid %d overflow.}", uc_tid);
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }
    pst_tid      = &(pst_hmac_user->ast_tid_info[uc_tid]);

    OAM_WARNING_LOG3(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_delba_etc::receive delba from peer sta, tid[%d], uc_initiator[%d], reason[%d].}",
      uc_tid, uc_initiator, us_reason);

    /* ��tid��Ӧ��tx BA�Ự״̬���� */
    oal_spin_lock_bh(&(pst_tid->st_ba_tx_info.st_ba_status_lock));

    /* ����BA���ͻỰ */
    if (MAC_RECIPIENT_DELBA == uc_initiator)
    {
        if (DMAC_BA_INIT == pst_tid->st_ba_tx_info.en_ba_status)
        {
            oal_spin_unlock_bh(&(pst_tid->st_ba_tx_info.st_ba_status_lock));
            return OAL_SUCC;
        }

        pst_hmac_user->auc_ba_flag[uc_tid]  = 0;

        /* ��ԭ����AMPDU��AMSDU��֧����� */
        HMAC_USER_SET_AMSDU_SUPPORT(pst_hmac_user, uc_tid);

#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
        hmac_tx_ba_session_decr_etc(pst_hmac_vap, pst_hmac_user->ast_tid_info[uc_tid].uc_tid_no);
#else
        hmac_tx_ba_session_decr_etc(pst_device, pst_hmac_user->ast_tid_info[uc_tid].uc_tid_no);
#endif
    }
    else   /* ����BA���ջỰ */
    {
        hmac_ba_reset_rx_handle_etc(pst_device, &pst_hmac_user->ast_tid_info[uc_tid].pst_ba_rx_info, uc_tid, OAL_FALSE);
        OAM_INFO_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA,
                      "{hmac_mgmt_rx_delba_etc::rcv rx dir del ba.}\r\n");
    }

    /* ���¼���DMAC���� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_ctx_action_event_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_BA, "{hmac_mgmt_rx_delba_etc::pst_event_mem null.}");
        oal_spin_unlock_bh(&(pst_tid->st_ba_tx_info.st_ba_status_lock));
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����¼�ָ�� */
    pst_hmac_to_dmac_crx_sync = frw_get_event_stru(pst_event_mem);

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_hmac_to_dmac_crx_sync->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_BA_SYNC,
                       OAL_SIZEOF(dmac_ctx_action_event_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_hmac_vap->st_vap_base_info.uc_chip_id,
                       pst_hmac_vap->st_vap_base_info.uc_device_id,
                       pst_hmac_vap->st_vap_base_info.uc_vap_id);

    /*��д�¼�payload */
    pst_wlan_crx_action = (dmac_ctx_action_event_stru *)(pst_hmac_to_dmac_crx_sync->auc_event_data);
    pst_wlan_crx_action->en_action_category  = MAC_ACTION_CATEGORY_BA;
    pst_wlan_crx_action->uc_action           = MAC_BA_ACTION_DELBA;
    pst_wlan_crx_action->us_user_idx         = pst_hmac_user->st_user_base_info.us_assoc_id;

    pst_wlan_crx_action->uc_tidno            = uc_tid;
    pst_wlan_crx_action->uc_initiator        = uc_initiator;

    /* �ַ� */
    frw_event_dispatch_event_etc(pst_event_mem);

    /* �ͷ��¼��ڴ� */
    FRW_EVENT_FREE(pst_event_mem);

    /* DELBA�¼��ȴ����ٸ�״̬,��ֹaddba req�ȴ��� */
    if (MAC_RECIPIENT_DELBA == uc_initiator)
    {
        pst_tid->st_ba_tx_info.en_ba_status = DMAC_BA_INIT;
    }

    /* ��tid��Ӧ��tx BA�Ự״̬���� */
    oal_spin_unlock_bh(&(pst_tid->st_ba_tx_info.st_ba_status_lock));

    return OAL_SUCC;
}


oal_uint32  hmac_mgmt_tx_addba_timeout_etc(oal_void *p_arg)
{
    hmac_vap_stru                      *pst_vap = OAL_PTR_NULL;         /* vapָ�� */
    oal_uint8                          *puc_da;                         /* �����û�Ŀ�ĵ�ַ��ָ�� */
    hmac_user_stru                     *pst_hmac_user;
    mac_action_mgmt_args_stru           st_action_args;
    dmac_ba_alarm_stru                 *pst_alarm_data;

    if (OAL_PTR_NULL == p_arg)
    {
        OAM_ERROR_LOG0(0, OAM_SF_BA, "{hmac_mgmt_tx_addba_timeout_etc::p_arg null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_alarm_data = (dmac_ba_alarm_stru *)p_arg;

    pst_hmac_user = (hmac_user_stru *)mac_res_get_hmac_user_etc(pst_alarm_data->us_mac_user_idx);
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_ERROR_LOG1(0, OAM_SF_BA, "{hmac_mgmt_tx_addba_timeout_etc::pst_hmac_user[%d] null.}", pst_alarm_data->us_mac_user_idx);
        return OAL_ERR_CODE_PTR_NULL;
    }

    puc_da  = pst_hmac_user->st_user_base_info.auc_user_mac_addr;

    pst_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_alarm_data->uc_vap_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_vap))
    {
        OAM_ERROR_LOG0(pst_hmac_user->st_user_base_info.uc_vap_id, OAM_SF_BA, "{hmac_mgmt_tx_addba_timeout_etc::pst_hmac_user null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����DELBA֡ */
    st_action_args.uc_category = MAC_ACTION_CATEGORY_BA;
    st_action_args.uc_action   = MAC_BA_ACTION_DELBA;
    st_action_args.ul_arg1     = pst_alarm_data->uc_tid;        /* ������֡��Ӧ��TID�� */
    st_action_args.ul_arg2     = MAC_ORIGINATOR_DELBA;         /* DELBA�У�����ɾ��BA�Ự�ķ���� */
    st_action_args.ul_arg3     = MAC_QSTA_TIMEOUT;                  /* DELBA�д���ɾ��reason */
    st_action_args.puc_arg5    = puc_da;                        /* DELBA�д���Ŀ�ĵ�ַ */

    hmac_mgmt_tx_delba_etc(pst_vap, pst_hmac_user, &st_action_args);

    return OAL_SUCC;
}


oal_uint32  hmac_mgmt_tx_ampdu_start_etc(
                hmac_vap_stru              *pst_hmac_vap,
                hmac_user_stru             *pst_hmac_user,
                mac_priv_req_args_stru     *pst_priv_req)
{
    frw_event_mem_stru         *pst_event_mem;      /* �����¼����ص��ڴ�ָ�� */
    frw_event_stru             *pst_crx_priv_req_event;
    mac_priv_req_args_stru     *pst_rx_ampdu_start_event;
    oal_uint8                   uc_tidno;
    hmac_tid_stru              *pst_tid;
    oal_uint32                  ul_ret;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == pst_priv_req))
    {
        OAM_ERROR_LOG3(0, OAM_SF_AMPDU, "{hmac_mgmt_tx_ampdu_start_etc::param null, %d %d %d.}", pst_hmac_vap, pst_hmac_user, pst_priv_req);
        return OAL_ERR_CODE_PTR_NULL;
    }

    uc_tidno = pst_priv_req->uc_arg1;
    pst_tid  = &(pst_hmac_user->ast_tid_info[uc_tidno]);

    /* AMPDUΪNORMAL ACKʱ����Ӧ��BA�Ựû�н������򷵻� */
    if (WLAN_TX_NORMAL_ACK == pst_priv_req->uc_arg3)
    {
        if (DMAC_BA_INIT == pst_tid->st_ba_tx_info.en_ba_status)
        {
            OAM_INFO_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AMPDU, "{hmac_mgmt_tx_ampdu_start_etc::normal ack but ba session is not build.}");
            return OAL_SUCC;
        }
    }

    /* ���¼���DMAC���� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(mac_priv_req_args_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AMPDU, "{hmac_mgmt_tx_ampdu_start_etc::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����¼�ָ�� */
    pst_crx_priv_req_event = frw_get_event_stru(pst_event_mem);

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_crx_priv_req_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_PRIV_REQ,
                       OAL_SIZEOF(mac_priv_req_args_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_hmac_vap->st_vap_base_info.uc_chip_id,
                       pst_hmac_vap->st_vap_base_info.uc_device_id,
                       pst_hmac_vap->st_vap_base_info.uc_vap_id);

    /* ��ȡ����AMPDU�Ĳ�������dmac�������� */
    pst_rx_ampdu_start_event = (mac_priv_req_args_stru *)(pst_crx_priv_req_event->auc_event_data);
    pst_rx_ampdu_start_event->uc_type = MAC_A_MPDU_START;
    pst_rx_ampdu_start_event->uc_arg1 = pst_priv_req->uc_arg1;
    pst_rx_ampdu_start_event->uc_arg2 = pst_priv_req->uc_arg2;
    pst_rx_ampdu_start_event->uc_arg3 = pst_priv_req->uc_arg3;
    pst_rx_ampdu_start_event->us_user_idx = pst_hmac_user->st_user_base_info.us_assoc_id;   /* ���������Դ�ص����� */

    /* �ַ� */
    ul_ret = frw_event_dispatch_event_etc(pst_event_mem);

    /* �ͷ��¼��ڴ� */
    FRW_EVENT_FREE(pst_event_mem);

    return ul_ret;
}


oal_uint32  hmac_mgmt_tx_ampdu_end_etc(
                hmac_vap_stru              *pst_hmac_vap,
                hmac_user_stru             *pst_hmac_user,
                mac_priv_req_args_stru     *pst_priv_req)
{

    frw_event_mem_stru         *pst_event_mem;      /* �����¼����ص��ڴ�ָ�� */
    frw_event_stru             *pst_crx_priv_req_event;
    mac_priv_req_args_stru     *pst_rx_ampdu_end_event;
    oal_uint32                  ul_ret;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == pst_priv_req))
    {
        OAM_ERROR_LOG3(0, OAM_SF_AMPDU, "{hmac_mgmt_tx_ampdu_end_etc::param null, %d %d %d.}", pst_hmac_vap, pst_hmac_user, pst_priv_req);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���¼���DMAC���� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(mac_priv_req_args_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AMPDU, "{hmac_mgmt_tx_ampdu_end_etc::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����¼�ָ�� */
    pst_crx_priv_req_event = frw_get_event_stru(pst_event_mem);

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_crx_priv_req_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_PRIV_REQ,
                       OAL_SIZEOF(mac_priv_req_args_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_hmac_vap->st_vap_base_info.uc_chip_id,
                       pst_hmac_vap->st_vap_base_info.uc_device_id,
                       pst_hmac_vap->st_vap_base_info.uc_vap_id);

    /* ��ȡ����AMPDU�Ĳ�������dmac�������� */
    pst_rx_ampdu_end_event = (mac_priv_req_args_stru *)(pst_crx_priv_req_event->auc_event_data);
    pst_rx_ampdu_end_event->uc_type = MAC_A_MPDU_END;              /* ���� */
    pst_rx_ampdu_end_event->uc_arg1 = pst_priv_req->uc_arg1;        /* tid no */
    pst_rx_ampdu_end_event->us_user_idx = pst_hmac_user->st_user_base_info.us_assoc_id;   /* ���������Դ�ص����� */

    /* �ַ� */
    ul_ret = frw_event_dispatch_event_etc(pst_event_mem);


    /* �ͷ��¼��ڴ� */
    FRW_EVENT_FREE(pst_event_mem);

    return ul_ret;
}



oal_uint32 hmac_tx_mgmt_send_event_etc(mac_vap_stru *pst_vap, oal_netbuf_stru *pst_mgmt_frame, oal_uint16 us_frame_len)
{
    frw_event_mem_stru   *pst_event_mem;
    frw_event_stru       *pst_event;
    oal_uint32            ul_return;
    dmac_tx_event_stru   *pst_ctx_stru;

    if (OAL_PTR_NULL == pst_vap || OAL_PTR_NULL == pst_mgmt_frame)
    {
        OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{hmac_tx_mgmt_send_event_etc::param null, %d %d.}", pst_vap, pst_mgmt_frame);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���¼���DMAC,��DMAC�������VAP���� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_tx_event_stru));
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(pst_vap->uc_vap_id, OAM_SF_SCAN, "{hmac_tx_mgmt_send_event_etc::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event = frw_get_event_stru(pst_event_mem);

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                    FRW_EVENT_TYPE_WLAN_CTX,
                    DMAC_WLAN_CTX_EVENT_SUB_TYPE_MGMT,
                    OAL_SIZEOF(dmac_tx_event_stru),
                    FRW_EVENT_PIPELINE_STAGE_1,
                    pst_vap->uc_chip_id,
                    pst_vap->uc_device_id,
                    pst_vap->uc_vap_id);

    pst_ctx_stru                = (dmac_tx_event_stru *)pst_event->auc_event_data;
    pst_ctx_stru->pst_netbuf    = pst_mgmt_frame;
    pst_ctx_stru->us_frame_len  = us_frame_len;

#ifdef _PRE_WLAN_FEATURE_SNIFFER
    proc_sniffer_write_file(NULL, 0, (oal_uint8 *)(OAL_NETBUF_HEADER(pst_mgmt_frame)), us_frame_len, 1);
#endif

    ul_return = frw_event_dispatch_event_etc(pst_event_mem);
    if (OAL_SUCC != ul_return)
    {
        OAM_WARNING_LOG1(pst_vap->uc_vap_id, OAM_SF_SCAN, "{hmac_tx_mgmt_send_event_etc::frw_event_dispatch_event_etc failed[%d].}", ul_return);
        FRW_EVENT_FREE(pst_event_mem);
        return ul_return;
    }

    /* �ͷ��¼� */
    FRW_EVENT_FREE(pst_event_mem);

    return OAL_SUCC;
}


oal_uint32  hmac_mgmt_reset_psm_etc(mac_vap_stru *pst_vap, oal_uint16 us_user_id)
{
    frw_event_mem_stru   *pst_event_mem;
    frw_event_stru       *pst_event;
    oal_uint16           *pus_user_id;
    hmac_user_stru       *pst_hmac_user;

    if (OAL_PTR_NULL == pst_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_PWR, "{hmac_mgmt_reset_psm_etc::pst_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    /* ������ֱ�������õ�һЩ����������Ҫ�ٴ�ͬ�� */
    pst_hmac_user = (hmac_user_stru *)mac_res_get_hmac_user_etc(us_user_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_user))
    {
        OAM_ERROR_LOG1(pst_vap->uc_vap_id, OAM_SF_PWR, "{hmac_mgmt_reset_psm_etc::pst_hmac_user[%d] null.}", us_user_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(oal_uint16));
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(pst_vap->uc_vap_id, OAM_SF_PWR, "{hmac_mgmt_reset_psm_etc::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event = frw_get_event_stru(pst_event_mem);

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                    FRW_EVENT_TYPE_WLAN_CTX,
                    DMAC_WLAN_CTX_EVENT_SUB_TYPE_RESET_PSM,
                    OAL_SIZEOF(oal_uint16),
                    FRW_EVENT_PIPELINE_STAGE_1,
                    pst_vap->uc_chip_id,
                    pst_vap->uc_device_id,
                    pst_vap->uc_vap_id);

    pus_user_id = (oal_uint16 *)pst_event->auc_event_data;

    *pus_user_id = us_user_id;

    frw_event_dispatch_event_etc(pst_event_mem);

    FRW_EVENT_FREE(pst_event_mem);

    return OAL_SUCC;
}


#if (_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT)
OAL_STATIC oal_uint32 hmac_sa_query_del_user(mac_vap_stru *pst_mac_vap, hmac_user_stru  *pst_hmac_user)
{
    wlan_vap_mode_enum_uint8      en_vap_mode;
    hmac_vap_stru                *pst_hmac_vap  = OAL_PTR_NULL;
    oal_uint32                    ul_ret;
    mac_sa_query_stru            *pst_sa_query_info;

    if ((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == pst_hmac_user))
    {
        OAM_ERROR_LOG2(0, OAM_SF_PMF, "{hmac_sa_query_del_user::param null, %d %d.}", pst_mac_vap, pst_hmac_user);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_device_id, OAM_SF_PMF, "{hmac_sa_query_del_user::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* pending SA Query requests ���������� & sa query���̿�ʼʱ������ */
    pst_sa_query_info = &pst_hmac_user->st_sa_query_info;
    pst_sa_query_info->ul_sa_query_count      = 0;
    pst_sa_query_info->ul_sa_query_start_time = 0;

    /* �޸� state & ɾ�� user */
    en_vap_mode = pst_mac_vap->en_vap_mode;
    switch (en_vap_mode)
    {
        case WLAN_VAP_MODE_BSS_AP:
            {
                /* ���¼��ϱ��ںˣ��Ѿ�ȥ����ĳ��STA */
                hmac_handle_disconnect_rsp_ap_etc(pst_hmac_vap,pst_hmac_user);
            }
            break;

         case WLAN_VAP_MODE_BSS_STA:
            {
                /* �ϱ��ں�sta�Ѿ���ĳ��apȥ���� */
                hmac_sta_handle_disassoc_rsp_etc(pst_hmac_vap, MAC_DEAUTH_LV_SS);
            }
             break;
         default:
             break;
    }

    /* ɾ��user */
    ul_ret = hmac_user_del_etc(&pst_hmac_vap->st_vap_base_info, pst_hmac_user);
    if( OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_device_id, OAM_SF_PMF, "{hmac_sa_query_del_user::hmac_user_del_etc failed[%d].}", ul_ret);
        return OAL_FAIL;
    }
    return OAL_SUCC;
}


OAL_STATIC oal_uint32 hmac_send_sa_query_req(mac_vap_stru *pst_mac_vap,
                                                     hmac_user_stru  *pst_hmac_user,
                                                     oal_bool_enum_uint8 en_is_protected,
                                                     oal_uint16 us_init_trans_id)
{
    oal_uint16                    us_sa_query_len = 0;
    oal_netbuf_stru              *pst_sa_query    = OAL_PTR_NULL;
    mac_tx_ctl_stru              *pst_tx_ctl      = OAL_PTR_NULL;
    oal_uint32                    ul_ret;

    if ((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == pst_hmac_user))
    {
        OAM_ERROR_LOG2(0, OAM_SF_PMF, "{hmac_send_sa_query_req::param null, %d %d.}", pst_mac_vap, pst_hmac_user);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ����SA Query ֡�ռ� */
    pst_sa_query = (oal_netbuf_stru *)OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if(OAL_PTR_NULL == pst_sa_query)
    {
       OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_PMF, "{hmac_send_sa_query_req::pst_sa_query null.}");
       return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }

    /* ��װSA Query request֡*/
    OAL_MEMZERO(oal_netbuf_cb(pst_sa_query), OAL_NETBUF_CB_SIZE());
    us_sa_query_len = hmac_encap_sa_query_req_etc(pst_mac_vap,
                                             (oal_uint8 *)OAL_NETBUF_HEADER(pst_sa_query),
                                             pst_hmac_user->st_user_base_info.auc_user_mac_addr,
                                             pst_hmac_user->st_sa_query_info.us_sa_query_trans_id);

    /* ��������֡���� */
    if (OAL_TRUE == en_is_protected)
    {
        mac_set_protectedframe((oal_uint8 *)OAL_NETBUF_HEADER(pst_sa_query));
    }

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_sa_query); /* ��ȡcb�ṹ�� */
    MAC_GET_CB_MPDU_LEN(pst_tx_ctl)     = us_sa_query_len;               /* dmac������Ҫ��mpdu���� */
    MAC_GET_CB_TX_USER_IDX(pst_tx_ctl)  = pst_hmac_user->st_user_base_info.us_assoc_id;   /* ���������Ҫ��ȡuser�ṹ�� */

    oal_netbuf_put(pst_sa_query, us_sa_query_len);


    /* Buffer this frame in the Memory Queue for transmission */
    ul_ret = hmac_tx_mgmt_send_event_etc(pst_mac_vap, pst_sa_query, us_sa_query_len);
    if (OAL_SUCC != ul_ret)
    {
        oal_netbuf_free(pst_sa_query);
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_PMF, "{hmac_send_sa_query_req::hmac_tx_mgmt_send_event_etc failed[%d].}", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}

oal_uint32 hmac_pmf_check_err_code_etc(mac_user_stru *pst_user_base_info, oal_bool_enum_uint8 en_is_protected, oal_uint8 *puc_mac_hdr)
{
    oal_bool_enum_uint8 en_aim_err_code = OAL_FALSE;
    oal_uint16 us_err_code = MAC_UNSPEC_REASON;

    us_err_code = OAL_MAKE_WORD16(puc_mac_hdr[MAC_80211_FRAME_LEN], puc_mac_hdr[MAC_80211_FRAME_LEN + 1]);
    en_aim_err_code = ((MAC_NOT_AUTHED == us_err_code) || (MAC_NOT_ASSOCED == us_err_code)) ? OAL_TRUE : OAL_FALSE;

    if ((OAL_TRUE  == pst_user_base_info->st_cap_info.bit_pmf_active) &&
        (OAL_TRUE  == en_aim_err_code || OAL_FALSE == en_is_protected))
    {
        return OAL_SUCC;
    }

    return OAL_FAIL;

}


oal_uint32 hmac_sa_query_interval_timeout_etc(oal_void *p_arg)
{
    hmac_user_stru                *pst_hmac_user;
    hmac_vap_stru                 *pst_hmac_vap;
    oal_uint32                     ul_relt;
    oal_uint32                     ul_now;
    oal_uint32                     ul_passed;
    oal_uint32                     ul_retry_timeout;

    pst_hmac_user = (hmac_user_stru *)p_arg;
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_sa_query_interval_timeout_etc::invalid param.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap  = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_hmac_user->st_user_base_info.uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (MAC_USER_STATE_ASSOC != pst_hmac_user->st_user_base_info.en_user_asoc_state)
    {
        pst_hmac_user->st_sa_query_info.ul_sa_query_count    = 0;
        OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_ANY, "{hmac_sa_query_interval_timeout_etc::USER UNEXPECTED STATE[%d].}",
                                        pst_hmac_user->st_user_base_info.en_user_asoc_state);
        return OAL_SUCC;
    }

    ul_now    = (oal_uint32)OAL_TIME_GET_STAMP_MS();
    ul_passed = (oal_uint32)OAL_TIME_GET_RUNTIME(pst_hmac_user->st_sa_query_info.ul_sa_query_start_time,ul_now);

    if (ul_passed >= mac_mib_get_dot11AssociationSAQueryMaximumTimeout(&(pst_hmac_vap->st_vap_base_info)))
    {
        /*change state & ul_sa_query_count=0*/
        OAM_WARNING_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_ANY, "{hmac_sa_query_interval_timeout_etc::Deleting user.}");
        ul_relt = hmac_sa_query_del_user(&(pst_hmac_vap->st_vap_base_info), pst_hmac_user);
        if (OAL_SUCC != ul_relt)
        {
            OAM_ERROR_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_ANY,
                           "{hmac_sa_query_interval_timeout_etc::hmac_sa_query_del_user failed[%d].}", ul_relt);
            return OAL_ERR_CODE_PMF_SA_QUERY_DEL_USER_FAIL;
        }
        return OAL_SUCC;
    }

    /* �ٷ���һ֡sa query request */
    pst_hmac_user->st_sa_query_info.us_sa_query_trans_id += 1;
    pst_hmac_user->st_sa_query_info.ul_sa_query_count    += 1;
    OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_ANY,
                    "{hmac_sa_query_interval_timeout_etc::SA query, trans_id %d.}", pst_hmac_user->st_sa_query_info.us_sa_query_trans_id);

    ul_retry_timeout = mac_mib_get_dot11AssociationSAQueryRetryTimeout(&(pst_hmac_vap->st_vap_base_info));

    /* ���ü����ʱ�� */
    FRW_TIMER_CREATE_TIMER(&(pst_hmac_user->st_sa_query_info.st_sa_query_interval_timer),
                           hmac_sa_query_interval_timeout_etc,
                           ul_retry_timeout,
                           (oal_void *)pst_hmac_user,
                           OAL_FALSE,
                           OAM_MODULE_ID_HMAC,
                           pst_hmac_vap->st_vap_base_info.ul_core_id);

    ul_relt = hmac_send_sa_query_req(&(pst_hmac_vap->st_vap_base_info),
                                     pst_hmac_user,
                                     pst_hmac_user->st_sa_query_info.en_is_protected,
                                     pst_hmac_user->st_sa_query_info.us_sa_query_trans_id);
    if (OAL_SUCC != ul_relt)
    {
       OAM_ERROR_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_ANY,
                           "{hmac_sa_query_interval_timeout_etc::hmac_send_sa_query_req failed[%d].}", ul_relt);
       return OAL_ERR_CODE_PMF_SA_QUERY_REQ_SEND_FAIL;
    }
    return OAL_SUCC;
}


oal_uint32 hmac_start_sa_query_etc(mac_vap_stru *pst_mac_vap, hmac_user_stru  *pst_hmac_user, oal_bool_enum_uint8 en_is_protected)
{
    oal_uint32                    ul_retry_timeout;
    hmac_vap_stru                *pst_hmac_vap    = OAL_PTR_NULL;
    oal_uint32                    ul_ret;
    oal_uint16                    us_init_trans_id = 0;


    /* ����ж� */
    if ((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == pst_hmac_user))
    {
       OAM_ERROR_LOG2(0, OAM_SF_ANY, "{hmac_start_sa_query_etc::param null, %d %d.}", pst_mac_vap, pst_hmac_user);
       return OAL_ERR_CODE_PTR_NULL;
    }

    /* �ж�vap����pmf���� */
    if (OAL_TRUE != pst_hmac_user->st_user_base_info.st_cap_info.bit_pmf_active)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_start_sa_query_etc::bit_pmf_active is down.}");
        return OAL_ERR_CODE_PMF_DISABLED;
    }

    /* �����ظ�����SA Query���� */
    if (0 != pst_hmac_user->st_sa_query_info.ul_sa_query_count)
    {
        OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_start_sa_query_etc::SA Query is already in process.}");
        return OAL_SUCC;
    }

    /* ���hmac vap �ṹָ�� */
    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_start_sa_query_etc::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    ul_retry_timeout = mac_mib_get_dot11AssociationSAQueryRetryTimeout(pst_mac_vap);

    /* ��¼sa query���̿�ʼʱ��,��λms */
    pst_hmac_user->st_sa_query_info.ul_sa_query_start_time = (oal_uint32)OAL_TIME_GET_STAMP_MS();
    /* ��ó�ʼtrans_id */
    pst_hmac_user->st_sa_query_info.us_sa_query_trans_id   = (oal_uint16)OAL_TIME_GET_STAMP_MS();

    pst_hmac_user->st_sa_query_info.ul_sa_query_count      = 1;
    pst_hmac_user->st_sa_query_info.en_is_protected        = en_is_protected;

    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_start_sa_query_etc::SA query, trans_id %d.}", pst_hmac_user->st_sa_query_info.us_sa_query_trans_id);

    /* ���ü����ʱ�� */
    FRW_TIMER_CREATE_TIMER(&(pst_hmac_user->st_sa_query_info.st_sa_query_interval_timer),
                           hmac_sa_query_interval_timeout_etc,
                           ul_retry_timeout,
                           (oal_void *)pst_hmac_user,
                           OAL_FALSE,
                           OAM_MODULE_ID_HMAC,
                           pst_mac_vap->ul_core_id);

    /* ����SA Query request����ʼ��ѯ���� */
    ul_ret = hmac_send_sa_query_req(pst_mac_vap, pst_hmac_user, en_is_protected, us_init_trans_id);
    if (OAL_SUCC != ul_ret)
    {
       OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_start_sa_query_etc::hmac_send_sa_query_req failed[%d].}", ul_ret);
       return OAL_ERR_CODE_PMF_SA_QUERY_REQ_SEND_FAIL;
    }

    return OAL_SUCC;
}



oal_void  hmac_send_sa_query_rsp_etc(mac_vap_stru *pst_mac_vap, oal_uint8 *pst_hdr, oal_bool_enum_uint8 en_is_protected)
{
    oal_uint16                    us_sa_query_len = 0;
    oal_netbuf_stru              *pst_sa_query    = 0;
    mac_tx_ctl_stru              *pst_tx_ctl      = OAL_PTR_NULL;
    oal_uint16                    us_user_idx     = MAC_INVALID_USER_ID;
    oal_uint32                    ul_ret;

    if (OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == pst_hdr)
    {
        OAM_ERROR_LOG2(0, OAM_SF_ANY, "{hmac_send_sa_query_rsp_etc::param null, %d %d.}", pst_mac_vap, pst_hdr);
        return;
    }
    OAM_INFO_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_send_sa_query_rsp_etc::AP ready to TX a sa query rsp.}");

    pst_sa_query = (oal_netbuf_stru *)OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
    if (OAL_PTR_NULL == pst_sa_query)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_send_sa_query_rsp_etc::pst_sa_query null.}");
        return;
    }

    OAL_MEMZERO(oal_netbuf_cb(pst_sa_query), OAL_NETBUF_CB_SIZE());
    us_sa_query_len = hmac_encap_sa_query_rsp_etc(pst_mac_vap, pst_hdr, (oal_uint8 *)OAL_NETBUF_HEADER(pst_sa_query));

    /*��������֡����*/
    if (OAL_TRUE == en_is_protected)
    {
        mac_set_protectedframe((oal_uint8 *)OAL_NETBUF_HEADER(pst_sa_query));
    }

    ul_ret = mac_vap_find_user_by_macaddr_etc(pst_mac_vap, ((mac_ieee80211_frame_stru *)pst_hdr)->auc_address2, &us_user_idx);
    if(ul_ret != OAL_SUCC)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_send_sa_query_rsp_etc::mac_vap_find_user_by_macaddr_etc fail,error_code=%d.}", ul_ret);
        oal_netbuf_free(pst_sa_query);
        return;
    }

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_sa_query); /* ��ȡcb�ṹ�� */
    MAC_GET_CB_MPDU_LEN(pst_tx_ctl)     = us_sa_query_len;               /* dmac������Ҫ��mpdu���� */
    MAC_GET_CB_TX_USER_IDX(pst_tx_ctl)  = us_user_idx;                   /* ���������Ҫ��ȡuser�ṹ�� */

    oal_netbuf_put(pst_sa_query, us_sa_query_len);

    /* Buffer this frame in the Memory Queue for transmission */
    ul_ret = hmac_tx_mgmt_send_event_etc(pst_mac_vap, pst_sa_query, us_sa_query_len);
    if (OAL_SUCC != ul_ret)
    {
        oal_netbuf_free(pst_sa_query);
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_send_sa_query_rsp_etc::hmac_tx_mgmt_send_event_etc failed[%d].}", ul_ret);
    }

    return;
}
#endif

oal_void  hmac_mgmt_send_deauth_frame_etc(mac_vap_stru *pst_mac_vap, oal_uint8 *puc_da, oal_uint16 us_err_code, oal_bool_enum_uint8 en_is_protected)
{
    oal_uint16        us_deauth_len = 0;
    oal_netbuf_stru  *pst_deauth    = 0;
    mac_tx_ctl_stru  *pst_tx_ctl;
    oal_uint32        ul_ret;

    if (OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_da)
    {
        OAM_ERROR_LOG2(0, OAM_SF_AUTH, "{hmac_mgmt_send_deauth_frame_etc::param null, %d %d.}", pst_mac_vap, pst_mac_vap);
        return;
    }

    if (MAC_VAP_STATE_BUTT == pst_mac_vap->en_vap_state)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_AUTH, "{hmac_mgmt_send_deauth_frame_etc:: vap has been down/del, vap_state[%d].}", pst_mac_vap->en_vap_state);
        return;
    }

    pst_deauth = (oal_netbuf_stru *)OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);

    OAL_MEM_NETBUF_TRACE(pst_deauth, OAL_TRUE);

    if(OAL_PTR_NULL == pst_deauth)
    {
        /* Reserved Memory pool tried for high priority deauth frames */
        pst_deauth = (oal_netbuf_stru *)OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF,WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
        if(OAL_PTR_NULL == pst_deauth)
        {
            OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_AUTH, "{hmac_mgmt_send_deauth_frame_etc::pst_deauth null.}");
            return;
        }
    }

    OAL_MEMZERO(oal_netbuf_cb(pst_deauth), OAL_NETBUF_CB_SIZE());

    us_deauth_len = hmac_mgmt_encap_deauth_etc(pst_mac_vap, (oal_uint8 *)OAL_NETBUF_HEADER(pst_deauth), puc_da, us_err_code);
    if (0 == us_deauth_len)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_AUTH, "{hmac_mgmt_send_deauth_frame_etc:: us_deauth_len = 0.}");
        oal_netbuf_free(pst_deauth);
        return;
    }
    oal_netbuf_put(pst_deauth, us_deauth_len);

    /* MIB variables related to deauthentication are updated */
    //mac_mib_set_DeauthenticateReason(pst_mac_vap, us_err_code);
    //mac_mib_set_DeauthenticateStation(pst_mac_vap, puc_da);

    /* ���ӷ���ȥ��֤֡ʱ��ά����Ϣ */
    OAM_WARNING_LOG4(pst_mac_vap->uc_vap_id, OAM_SF_CONN,
              "{hmac_mgmt_send_deauth_frame_etc:: DEAUTH TX :  to %2x:XX:XX:XX:%2x:%2x, status code[%d]}",
               puc_da[0], puc_da[4], puc_da[5],us_err_code);

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_deauth);                              /* ��ȡcb�ṹ�� */
    MAC_GET_CB_MPDU_LEN(pst_tx_ctl)   = us_deauth_len;                                      /* dmac������Ҫ��mpdu���� */
    /* ���������Ҫ��ȡuser�ṹ�� */
    ul_ret = mac_vap_set_cb_tx_user_idx(pst_mac_vap, pst_tx_ctl, puc_da);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_AUTH, "(hmac_mgmt_send_deauth_frame_etc::fail to find user by xx:xx:xx:%2x:%2x:%2x.}",
        puc_da[3],
        puc_da[4],
        puc_da[5]);
    }

    /* Buffer this frame in the Memory Queue for transmission */
    ul_ret = hmac_tx_mgmt_send_event_etc(pst_mac_vap, pst_deauth, us_deauth_len);
    if (OAL_SUCC != ul_ret)
    {
        oal_netbuf_free(pst_deauth);

        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_AUTH, "{hmac_mgmt_send_deauth_frame_etc::hmac_tx_mgmt_send_event_etc failed[%d].}", ul_ret);
    }
}

oal_uint32  hmac_config_send_deauth_etc(mac_vap_stru *pst_mac_vap, oal_uint8 *puc_da)
{
    hmac_user_stru                 *pst_hmac_user;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_da))
    {
        OAM_ERROR_LOG2(0, OAM_SF_AUTH, "{hmac_config_send_deauth_etc::param null, %d %d.}", pst_mac_vap, puc_da);
        return OAL_ERR_CODE_PTR_NULL;
    }


    pst_hmac_user = mac_vap_get_hmac_user_by_addr_etc(pst_mac_vap, puc_da);
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_AUTH, "{hmac_config_send_deauth_etc::pst_hmac_user null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (MAC_USER_STATE_ASSOC != pst_hmac_user->st_user_base_info.en_user_asoc_state)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_AUTH, "{hmac_config_send_deauth_etc::the user is unassociated.}");
        return OAL_FAIL;
    }


    /* ��ȥ��֤֡ */
    hmac_mgmt_send_deauth_frame_etc(pst_mac_vap, puc_da, MAC_AUTH_NOT_VALID, OAL_FALSE);

    /* ɾ���û� */
    hmac_user_del_etc(pst_mac_vap, pst_hmac_user);

    return OAL_SUCC;
}


oal_void  hmac_mgmt_send_disassoc_frame_etc(mac_vap_stru *pst_mac_vap,oal_uint8 *puc_da, oal_uint16 us_err_code, oal_bool_enum_uint8 en_is_protected)
{
    oal_uint16        us_disassoc_len = 0;
    oal_netbuf_stru  *pst_disassoc    = 0;
    mac_tx_ctl_stru  *pst_tx_ctl;
    oal_uint32        ul_ret;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    oal_uint32        ul_pedding_data = 0;
#endif

    if (OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_da)
    {
        OAM_ERROR_LOG2(0, OAM_SF_ASSOC, "{hmac_mgmt_send_disassoc_frame_etc::param null, %d %d.}", pst_mac_vap, pst_mac_vap);
        return;
    }

    if (MAC_VAP_STATE_BUTT == pst_mac_vap->en_vap_state)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ASSOC, "{hmac_mgmt_send_disassoc_frame_etc:: vap has been down/del, vap_state[%d].}", pst_mac_vap->en_vap_state);
        return;
    }

    pst_disassoc = (oal_netbuf_stru *)OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);

    if(OAL_PTR_NULL == pst_disassoc)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ASSOC, "{hmac_mgmt_send_disassoc_frame_etc::pst_disassoc null.}");
        return;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_config_scan_abort_etc(pst_mac_vap, OAL_SIZEOF(oal_uint32), (oal_uint8 *)&ul_pedding_data);
#endif

    OAL_MEM_NETBUF_TRACE(pst_disassoc, OAL_TRUE);

    OAL_MEMZERO(oal_netbuf_cb(pst_disassoc), OAL_NETBUF_CB_SIZE());

    us_disassoc_len = hmac_mgmt_encap_disassoc_etc(pst_mac_vap, (oal_uint8 *)OAL_NETBUF_HEADER(pst_disassoc), puc_da, us_err_code);
    if (0 == us_disassoc_len)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ASSOC, "{hmac_mgmt_send_disassoc_frame_etc:: us_disassoc_len = 0.}");
        oal_netbuf_free(pst_disassoc);
        return;
    }

    //mac_mib_set_DisassocReason(pst_mac_vap, us_err_code);
    //mac_mib_set_DisassocStation(pst_mac_vap, puc_da);

    /* ���ӷ���ȥ����֡ʱ��ά����Ϣ */
    OAM_WARNING_LOG4(pst_mac_vap->uc_vap_id, OAM_SF_CONN,
                     "{hmac_mgmt_send_disassoc_frame_etc:: DISASSOC tx, Because of err_code[%d], send disassoc frame to dest addr, da[%2X:XX:XX:XX:%2X:%2X].}",
                    us_err_code, puc_da[0], puc_da[4], puc_da[5]);

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_disassoc);
    MAC_GET_CB_MPDU_LEN(pst_tx_ctl)  = us_disassoc_len;
    /* ��д�Ƿ�ֵ,�������֮���ȡ�û�ΪNULL,ֱ���ͷ�ȥ��֤֡  */
    ul_ret = mac_vap_set_cb_tx_user_idx(pst_mac_vap, pst_tx_ctl, puc_da);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_ASSOC, "(hmac_mgmt_send_disassoc_frame_etc::fail to find user by xx:xx:xx:0x:0x:0x.}",
        puc_da[3],
        puc_da[4],
        puc_da[5]);
    }

    oal_netbuf_put(pst_disassoc, us_disassoc_len);

    /* ���뷢�Ͷ��� */
    ul_ret = hmac_tx_mgmt_send_event_etc(pst_mac_vap, pst_disassoc, us_disassoc_len);

    if (OAL_SUCC != ul_ret)
    {
        oal_netbuf_free(pst_disassoc);

        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ASSOC, "{hmac_mgmt_send_disassoc_frame_etc::hmac_tx_mgmt_send_event_etc failed[%d].}", ul_ret);
    }
}



oal_void hmac_mgmt_update_assoc_user_qos_table_etc(
                oal_uint8                      *puc_payload,
                oal_uint16                      us_msg_len,
                hmac_user_stru                 *pst_hmac_user)
{
    frw_event_mem_stru                       *pst_event_mem;
    frw_event_stru                           *pst_event;
    dmac_ctx_asoc_set_reg_stru                st_asoc_set_reg_param = {0};
    oal_uint8                                *puc_ie                = OAL_PTR_NULL;
    mac_vap_stru                             *pst_mac_vap           = OAL_PTR_NULL;

    /* ��������û�֮ǰ����wmmʹ�ܵģ�ʲô����������ֱ�ӷ���  */
    if (OAL_TRUE == pst_hmac_user->st_user_base_info.st_cap_info.bit_qos)
    {
        OAM_INFO_LOG0(pst_hmac_user->st_user_base_info.uc_vap_id, OAM_SF_ASSOC,
                      "{hmac_mgmt_update_assoc_user_qos_table_etc::assoc user is wmm cap already.}");
        return;
    }

    mac_user_set_qos_etc(&pst_hmac_user->st_user_base_info, OAL_FALSE);

    pst_mac_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_hmac_user->st_user_base_info.uc_vap_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap))
    {
        OAM_ERROR_LOG0(pst_hmac_user->st_user_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_mgmt_update_assoc_user_qos_table_etc::pst_mac_vap null.}");
        return;
    }

    puc_ie = mac_get_wmm_ie_etc(puc_payload, us_msg_len);

    if (OAL_PTR_NULL != puc_ie)
    {
        mac_user_set_qos_etc(&pst_hmac_user->st_user_base_info, pst_mac_vap->pst_mib_info->st_wlan_mib_sta_config.en_dot11QosOptionImplemented);
    }
    else
    {
        if(IS_STA(pst_mac_vap))
        {
            /* ��������û�֮ǰ����û��Я��wmm ie, �ٲ���HT CAP���� */
            puc_ie = mac_find_ie_etc(MAC_EID_HT_CAP, puc_payload, us_msg_len);
            if (OAL_PTR_NULL != puc_ie)
            {
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
                /* �ٲ���HT CAP������2�ֽ�BIT 5 short GI for 20M ����λ */
                if ((puc_ie[1] >= 2)&&(puc_ie[2] & BIT5))
#endif
                {
                    mac_user_set_qos_etc(&pst_hmac_user->st_user_base_info, OAL_TRUE);
                }
            }
        }
    }

    /* ��������û���������Ȼ����wmmʹ�ܵģ�ʲôҲ������ֱ�ӷ��� */
    if (OAL_FALSE == pst_hmac_user->st_user_base_info.st_cap_info.bit_qos)
    {
        OAM_INFO_LOG0(pst_hmac_user->st_user_base_info.uc_vap_id, OAM_SF_ASSOC,
                      "{hmac_mgmt_update_assoc_user_qos_table_etc::assoc user is not wmm cap.}");
        return;
    }

    /* �������û��Ӳ�֧��wmm��֧��wmmת��ʱ�����¼���DMAC д�Ĵ���*/

    /* �����¼��ڴ� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_ctx_asoc_set_reg_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG1(pst_hmac_user->st_user_base_info.uc_vap_id, OAM_SF_ASSOC,
                      "{hmac_mgmt_update_assoc_user_qos_table_etc::event_mem alloc null, size[%d].}", OAL_SIZEOF(dmac_ctx_asoc_set_reg_stru));
        return;
    }

    st_asoc_set_reg_param.uc_user_index = pst_hmac_user->st_user_base_info.us_assoc_id;

    /* ��д�¼� */
    pst_event = frw_get_event_stru(pst_event_mem);

    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_ASOC_WRITE_REG,
                       OAL_SIZEOF(dmac_ctx_asoc_set_reg_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_hmac_user->st_user_base_info.uc_chip_id,
                       pst_hmac_user->st_user_base_info.uc_device_id,
                       pst_hmac_user->st_user_base_info.uc_vap_id);

    /* �������� */
    oal_memcopy(pst_event->auc_event_data, (oal_void *)&st_asoc_set_reg_param, OAL_SIZEOF(dmac_ctx_asoc_set_reg_stru));

    /* �ַ��¼� */
    frw_event_dispatch_event_etc(pst_event_mem);
    FRW_EVENT_FREE(pst_event_mem);

}


#ifdef _PRE_WLAN_FEATURE_TXBF
oal_void hmac_mgmt_update_11ntxbf_cap_etc(oal_uint8 *puc_payload, hmac_user_stru *pst_hmac_user)
{
    mac_11ntxbf_vendor_ie_stru *pst_vendor_ie;
    if ((OAL_PTR_NULL == puc_payload))
    {
        return;
    }

    /* ��⵽vendor ie*/
    pst_vendor_ie = (mac_11ntxbf_vendor_ie_stru *)puc_payload;

    pst_hmac_user->st_user_base_info.st_cap_info.bit_11ntxbf = pst_vendor_ie->st_11ntxbf.bit_11ntxbf;

    return ;
}
#endif /* #ifdef _PRE_WLAN_FEATURE_TXBF */


oal_uint32  hmac_check_bss_cap_info_etc(oal_uint16 us_cap_info,mac_vap_stru *pst_mac_vap)
{
    hmac_vap_stru                        *pst_hmac_vap;
    oal_uint32                            ul_ret = OAL_FALSE;
    wlan_mib_desired_bsstype_enum_uint8   en_bss_type;

    /* ��ȡCAP INFO��BSS TYPE */
    en_bss_type  =  mac_get_bss_type_etc(us_cap_info);

    pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_CFG, "{hmac_check_bss_cap_info_etc::pst_hmac_vap null.}");
        return OAL_FAIL;
    }

    /* �Ƚ�BSS TYPE�Ƿ�һ�� ��һ�£������STA��Ȼ�������������Ӽ����ԣ�����ģʽ�򷵻ز�֧�� */
    if (en_bss_type != mac_mib_get_DesiredBSSType(pst_mac_vap))
    {
        OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_check_bss_cap_info_etc::vap_bss_type[%d] is different from asoc_user_bss_type[%d].}",
                         mac_mib_get_DesiredBSSType(pst_mac_vap), en_bss_type);
    }

    if (OAL_TRUE == mac_mib_get_WPSActive(pst_mac_vap))
    {
        return OAL_TRUE;
    }

    /* �Ƚ�CAP INFO��privacyλ������Ƿ���ܣ����ܲ�һ�£�����ʧ�� */
    ul_ret = mac_check_mac_privacy_etc(us_cap_info,(oal_uint8 *)pst_mac_vap);
    if (ul_ret != OAL_TRUE)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_check_bss_cap_info_etc::mac privacy capabilities is different.}");
    }

    return OAL_TRUE;
}
#if 0

OAL_STATIC oal_bool_enum_uint8  hmac_user_is_rate_support(hmac_user_stru *pst_hmac_user, oal_uint8 uc_rate)
{
    mac_rate_stru       *pst_op_rates;
    oal_bool_enum_uint8  en_rate_is_supp = OAL_FALSE;
    oal_uint8            uc_loop;

    pst_op_rates = &(pst_hmac_user->st_op_rates);

    for (uc_loop = 0; uc_loop < pst_op_rates->uc_rs_nrates; uc_loop++)
    {
        if ((pst_op_rates->auc_rs_rates[uc_loop] & 0x7F) == uc_rate)
        {
            en_rate_is_supp = OAL_TRUE;
            break;
        }
    }

    return en_rate_is_supp;
}
#endif

oal_void hmac_set_user_protocol_mode_etc(mac_vap_stru *pst_mac_vap, hmac_user_stru *pst_hmac_user)
{
    mac_user_ht_hdl_stru         *pst_mac_ht_hdl;
    mac_vht_hdl_stru             *pst_mac_vht_hdl;

#ifdef _PRE_WLAN_FEATURE_11AX
    mac_he_hdl_stru              *pst_mac_he_hdl;
#endif
    mac_user_stru                *pst_mac_user;

    /* ��ȡHT��VHT�ṹ��ָ�� */
    pst_mac_user    = &pst_hmac_user->st_user_base_info;
    pst_mac_vht_hdl = &(pst_mac_user->st_vht_hdl);
    pst_mac_ht_hdl  = &(pst_mac_user->st_ht_hdl);

#ifdef _PRE_WLAN_FEATURE_11AX
    pst_mac_he_hdl  = &(pst_mac_user->st_he_hdl);
    if(OAL_TRUE == pst_mac_he_hdl->en_he_capable)
    {
        mac_user_set_protocol_mode_etc(pst_mac_user,WLAN_HE_MODE);
    }
    else if (OAL_TRUE == pst_mac_vht_hdl->en_vht_capable)
#else
    if(OAL_TRUE == pst_mac_vht_hdl->en_vht_capable)
#endif
    {
        mac_user_set_protocol_mode_etc(pst_mac_user, WLAN_VHT_MODE);
    }
    else if (OAL_TRUE == pst_mac_ht_hdl->en_ht_capable)
    {
        mac_user_set_protocol_mode_etc(pst_mac_user, WLAN_HT_MODE);
    }
    else
    {
        if (WLAN_BAND_5G == pst_mac_vap->st_channel.en_band)            /* �ж��Ƿ���5G */
        {
            mac_user_set_protocol_mode_etc(pst_mac_user, WLAN_LEGACY_11A_MODE);
        }
        else
        {
            if (OAL_TRUE == hmac_is_support_11grate_etc(pst_hmac_user->st_op_rates.auc_rs_rates, pst_hmac_user->st_op_rates.uc_rs_nrates))
            {
                mac_user_set_protocol_mode_etc(pst_mac_user, WLAN_LEGACY_11G_MODE);
                if (OAL_TRUE == hmac_is_support_11brate_etc(pst_hmac_user->st_op_rates.auc_rs_rates, pst_hmac_user->st_op_rates.uc_rs_nrates))
                {
                    mac_user_set_protocol_mode_etc(pst_mac_user, WLAN_MIXED_ONE_11G_MODE);
                }
            }
            else if (OAL_TRUE == hmac_is_support_11brate_etc(pst_hmac_user->st_op_rates.auc_rs_rates, pst_hmac_user->st_op_rates.uc_rs_nrates))
            {
                mac_user_set_protocol_mode_etc(pst_mac_user, WLAN_LEGACY_11B_MODE);
            }
            else
            {
                OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_set_user_protocol_mode_etc::set user protocol failed.}");
            }
        }
    }

    /* ���������⣺˼��AP 2.4G��11b����5G(11a)����ʱ���͵�assoc rsp֡Я�������ʷֱ���11g��11b������STA�����û�ʱ֪ͨ�㷨ʧ�ܣ�
    AutorateʧЧ��DBAC����£�DBAC�޷������ѹ�����VAP״̬�޷��ָ������� ��ʱ������������ԶԶ������쳣�����ͳһ�����Ż� */
    if (((WLAN_LEGACY_11B_MODE == pst_mac_user->en_protocol_mode) && (WLAN_LEGACY_11A_MODE == pst_mac_vap->en_protocol))
        || ((WLAN_LEGACY_11G_MODE == pst_mac_user->en_protocol_mode) && (WLAN_LEGACY_11B_MODE == pst_mac_vap->en_protocol)))
    {
        mac_user_set_protocol_mode_etc(pst_mac_user, pst_mac_vap->en_protocol);
        oal_memcopy((oal_void *)&(pst_hmac_user->st_op_rates), (oal_void *)&(pst_mac_vap->st_curr_sup_rates.st_rate), OAL_SIZEOF(mac_rate_stru));
    }

    /* ���������⣺ws880����11aʱbeacon��probe resp֡��Э��vht������vap��protocol����Ҫ���ݹ�����Ӧ֡��ʵ������ˢ�³�ʵ��11a���� */
    if ((WLAN_VAP_MODE_BSS_STA == pst_mac_vap->en_vap_mode)&&
        (WLAN_VHT_MODE == pst_mac_vap->en_protocol)&&(WLAN_LEGACY_11A_MODE == pst_mac_user->en_protocol_mode))
    {
        mac_vap_init_by_protocol_etc(pst_mac_vap, pst_mac_user->en_protocol_mode);
    }
}

oal_void hmac_user_init_rates_etc(hmac_user_stru *pst_hmac_user)
{
    OAL_MEMZERO((oal_uint8 *)(&pst_hmac_user->st_op_rates), OAL_SIZEOF(pst_hmac_user->st_op_rates));
}


oal_uint8 hmac_add_user_rates_etc(hmac_user_stru *pst_hmac_user, oal_uint8 uc_rates_cnt, oal_uint8 *puc_rates)
{
    if (pst_hmac_user->st_op_rates.uc_rs_nrates + uc_rates_cnt > WLAN_USER_MAX_SUPP_RATES)
    {
        uc_rates_cnt = WLAN_USER_MAX_SUPP_RATES - pst_hmac_user->st_op_rates.uc_rs_nrates;
    }

    oal_memcopy(&(pst_hmac_user->st_op_rates.auc_rs_rates[pst_hmac_user->st_op_rates.uc_rs_nrates]), puc_rates, uc_rates_cnt);
    pst_hmac_user->st_op_rates.uc_rs_nrates += uc_rates_cnt;

    return uc_rates_cnt;
}

#if (_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT)


oal_void  hmac_rx_sa_query_req_etc(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf, oal_bool_enum_uint8 en_is_protected)
{
    oal_uint8                   *puc_sa;
    hmac_user_stru              *pst_hmac_user   = OAL_PTR_NULL;
    oal_uint8                   *puc_mac_hdr     = OAL_PTR_NULL;

    if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_netbuf))
    {
        OAM_ERROR_LOG2(0, OAM_SF_RX, "{hmac_rx_sa_query_req_etc::null param %d %d.}", pst_hmac_vap, pst_netbuf);
        return;
    }

    puc_mac_hdr  = OAL_NETBUF_HEADER(pst_netbuf);

    mac_rx_get_sa((mac_ieee80211_frame_stru *)puc_mac_hdr, &puc_sa);
    pst_hmac_user = mac_vap_get_hmac_user_by_addr_etc(&pst_hmac_vap->st_vap_base_info, puc_sa);
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_RX,
                      "{hmac_rx_sa_query_req_etc::pst_hmac_user null.}");
        return;
    }

    /*������û��Ĺ���֡�������Բ�һ�£������ñ���*/
    if (en_is_protected != pst_hmac_user->st_user_base_info.st_cap_info.bit_pmf_active)
    {
       OAM_WARNING_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_RX,
                      "{hmac_rx_sa_query_req_etc::PMF check failed.}");
       return;
    }

    /*sa Query rsp����*/
    hmac_send_sa_query_rsp_etc(&pst_hmac_vap->st_vap_base_info, puc_mac_hdr, en_is_protected);

    return;

}
 
oal_void  hmac_rx_sa_query_rsp_etc(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf, oal_bool_enum_uint8 en_is_protected)
{
     oal_uint8                   *puc_mac_hdr     = OAL_PTR_NULL;
     oal_uint8                   *puc_sa;
     hmac_user_stru              *pst_hmac_user   = OAL_PTR_NULL;
     oal_uint16                  *pus_trans_id;
     mac_sa_query_stru           *pst_sa_query_info;

     if ((OAL_PTR_NULL == pst_hmac_vap) || (OAL_PTR_NULL == pst_netbuf))
     {
         OAM_ERROR_LOG2(0, OAM_SF_AMPDU, "{hmac_rx_sa_query_rsp_etc::param null,%d %d.}", pst_hmac_vap, pst_netbuf);
         return ;
     }

     puc_mac_hdr  = OAL_NETBUF_HEADER(pst_netbuf);

     mac_rx_get_sa((mac_ieee80211_frame_stru *)puc_mac_hdr, &puc_sa);
     pst_hmac_user = mac_vap_get_hmac_user_by_addr_etc(&pst_hmac_vap->st_vap_base_info, puc_sa);
     if (OAL_PTR_NULL == pst_hmac_user)
     {
         OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AMPDU, "{hmac_rx_sa_query_rsp_etc::pst_hmac_user null.}");
         return ;
     }

     /*������û��Ĺ���֡�������Բ�һ�£������ñ���*/
     if (en_is_protected != pst_hmac_user->st_user_base_info.st_cap_info.bit_pmf_active)
     {
         OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AMPDU, "{hmac_rx_sa_query_rsp_etc::PMF check failed.}");
         return ;
     }

     /*�Ա�trans_id*/
     pus_trans_id = (oal_uint16 *)(puc_mac_hdr + MAC_80211_FRAME_LEN + 2);
     pst_sa_query_info = &pst_hmac_user->st_sa_query_info;

     /*�յ���Ч��SA query reqponse������������Ч��SA*/
     if (0 == oal_memcmp(pus_trans_id, &(pst_sa_query_info->us_sa_query_trans_id), 2))
     {
         /* pending SA Query requests ���������� & sa query���̿�ʼʱ������*/
         pst_sa_query_info->ul_sa_query_count      = 0;
         pst_sa_query_info->ul_sa_query_start_time = 0;

         /* ɾ��timer */
         if (OAL_FALSE != pst_sa_query_info->st_sa_query_interval_timer.en_is_registerd)
         {
             FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&(pst_sa_query_info->st_sa_query_interval_timer));
         }
     }

     return ;

}
#endif


oal_void  hmac_send_mgmt_to_host_etc(hmac_vap_stru  *pst_hmac_vap,
                                                oal_netbuf_stru *puc_buf,
                                                oal_uint16       us_len,
                                                oal_int          l_freq)
{
    mac_device_stru         *pst_mac_device;
    frw_event_mem_stru      *pst_event_mem;
    frw_event_stru          *pst_event;
    hmac_rx_mgmt_event_stru *pst_mgmt_frame;
    mac_rx_ctl_stru            *pst_rx_info;
#ifdef _PRE_WLAN_FEATURE_P2P
    mac_ieee80211_frame_stru   *pst_frame_hdr;
    mac_vap_stru            *pst_mac_vap;
    oal_uint8                uc_hal_vap_id;
#endif
#ifdef _PRE_WLAN_FEATURE_HILINK
    dmac_rx_ctl_stru        *pst_rx_ctrl;
    oal_int8                 c_rssi_temp;
#endif
    oal_uint8*               pst_mgmt_data;

    pst_mac_device = mac_res_get_dev_etc(pst_hmac_vap->st_vap_base_info.uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_WARNING_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_send_mgmt_to_host_etc::pst_mac_device null.}");
        return;
    }

    /* �׹���һ���µ�sta����¼���WAL */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(hmac_rx_mgmt_event_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_send_mgmt_to_host_etc::pst_event_mem null.}");
        return;
    }

#ifdef _PRE_WLAN_FEATURE_HILINK
    pst_rx_ctrl     = (dmac_rx_ctl_stru *)oal_netbuf_cb(puc_buf);
#endif
    pst_rx_info     = (mac_rx_ctl_stru *)oal_netbuf_cb(puc_buf);


    pst_mgmt_data = (oal_uint8*)oal_memalloc(us_len);
    if(OAL_PTR_NULL == pst_mgmt_data)
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_send_mgmt_to_host_etc::pst_mgmt_data null.}");
        FRW_EVENT_FREE(pst_event_mem);
        return;
    }
    oal_memcopy(pst_mgmt_data, (oal_uint8 *)MAC_GET_RX_CB_MAC_HEADER_ADDR(pst_rx_info), us_len);

    /* ��д�¼� */
    pst_event = frw_get_event_stru(pst_event_mem);

    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_HOST_CTX,
                       HMAC_HOST_CTX_EVENT_SUB_TYPE_RX_MGMT,
                       OAL_SIZEOF(hmac_rx_mgmt_event_stru),
                       FRW_EVENT_PIPELINE_STAGE_0,
                       pst_hmac_vap->st_vap_base_info.uc_chip_id,
                       pst_hmac_vap->st_vap_base_info.uc_device_id,
                       pst_hmac_vap->st_vap_base_info.uc_vap_id);

    /* ��д�ϱ�����֡���� */
    pst_mgmt_frame = (hmac_rx_mgmt_event_stru *)(pst_event->auc_event_data);
    pst_mgmt_frame->puc_buf = (oal_uint8 *)pst_mgmt_data;
    pst_mgmt_frame->us_len  = us_len;
    pst_mgmt_frame->l_freq  = l_freq;
#ifdef _PRE_WLAN_FEATURE_HILINK
    if ((pst_rx_ctrl->st_rx_statistic.c_rssi_dbm > OAL_RSSI_SIGNAL_MAX) || (pst_rx_ctrl->st_rx_statistic.c_rssi_dbm < OAL_RSSI_SIGNAL_MIN))
    {
        c_rssi_temp = (oal_int8)HMAC_RSSI_SIGNAL_INVALID;
    }
    else
    {
        c_rssi_temp = pst_rx_ctrl->st_rx_statistic.c_rssi_dbm + HMAC_FBT_RSSI_ADJUST_VALUE;
        if (c_rssi_temp < 0)
        {
            c_rssi_temp = 0;
        }
        if (c_rssi_temp > HMAC_FBT_RSSI_MAX_VALUE)
        {
            c_rssi_temp = HMAC_FBT_RSSI_MAX_VALUE;
        }
    }
    pst_mgmt_frame->uc_rssi = (oal_uint8)c_rssi_temp;
#endif
    OAL_NETBUF_LEN(puc_buf) = us_len;

    oal_memcopy(pst_mgmt_frame->ac_name, pst_hmac_vap->pst_net_device->name, OAL_IF_NAME_SIZE);

#ifdef _PRE_WLAN_FEATURE_P2P
    pst_frame_hdr  = (mac_ieee80211_frame_stru *)mac_get_rx_cb_mac_hdr(pst_rx_info);
    pst_mac_vap = &(pst_hmac_vap->st_vap_base_info);
    if (!IS_LEGACY_VAP(pst_mac_vap))
    {
        /* �����P2P�豸��������P2P vap ����һ��vap ��Ӧ���hal_vap �������P2P vap ������һ��vap ��Ӧ���hal_vap ��� */
        /* �ԱȽ��յ��Ĺ���֡vap_id �Ƿ��vap ��hal_vap_id ��ͬ */

        /* �ӹ���֡cb�ֶ��е�hal vap id ����Ӧ��Ϣ���Ҷ�Ӧ��net dev ָ��*/
        uc_hal_vap_id = MAC_GET_RX_CB_HAL_VAP_IDX((mac_rx_ctl_stru *)oal_netbuf_cb(puc_buf));
        if (0 == oal_compare_mac_addr(pst_frame_hdr->auc_address1, mac_mib_get_p2p0_dot11StationID(pst_mac_vap)))
        //if (pst_hmac_vap->st_vap_base_info.uc_p2p0_hal_vap_id == uc_hal_vap_id)
        {
            /*�ڶ���net dev��*/
            oal_memcopy(pst_mgmt_frame->ac_name, pst_hmac_vap->pst_p2p0_net_device->name, OAL_IF_NAME_SIZE);
        }
        else if (0 == oal_compare_mac_addr(pst_frame_hdr->auc_address1, mac_mib_get_StationID(pst_mac_vap)))
        //else if (pst_hmac_vap->st_vap_base_info.uc_p2p_gocl_hal_vap_id == uc_hal_vap_id)
        {
            oal_memcopy(pst_mgmt_frame->ac_name, pst_hmac_vap->pst_net_device->name, OAL_IF_NAME_SIZE);
        }
        else if((14 == uc_hal_vap_id)&&
                (WLAN_PROBE_REQ == pst_frame_hdr->st_frame_control.bit_sub_type)&&
                (IS_P2P_CL(pst_mac_vap)||IS_P2P_DEV(pst_mac_vap)))
        {
#if 0
            //probe requestĿǰ���ϱ�wpa supplicant
            //oal_memcopy(pst_mgmt_frame->ac_name, pst_hmac_vap->pst_p2p0_net_device->name, OAL_IF_NAME_SIZE);
#endif
            FRW_EVENT_FREE(pst_event_mem);
            oal_free(pst_mgmt_data);
            return;
        }
        else
        {
            //OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
            //                "{hmac_send_mgmt_to_host_etc::error!.uc_hal_vap_id %d}", uc_hal_vap_id);
            FRW_EVENT_FREE(pst_event_mem);
            oal_free(pst_mgmt_data);
            return;
        }
    }
#endif
    /* �ַ��¼� */
    frw_event_dispatch_event_etc(pst_event_mem);
    FRW_EVENT_FREE(pst_event_mem);

}


oal_uint32 hmac_wpas_mgmt_tx_etc(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    oal_netbuf_stru                *pst_netbuf_mgmt_tx    =OAL_PTR_NULL;
    mac_tx_ctl_stru                *pst_tx_ctl;
    oal_uint32                      ul_ret;
    mac_mgmt_frame_stru            *pst_mgmt_tx;
    mac_device_stru                *pst_mac_device;
    oal_uint8                       ua_dest_mac_addr[WLAN_MAC_ADDR_LEN];
    oal_uint16                      us_user_idx;

    if (OAL_PTR_NULL == pst_mac_vap || OAL_PTR_NULL == puc_param)
    {
        OAM_ERROR_LOG2(0, OAM_SF_P2P, "{hmac_wpas_mgmt_tx_etc::param null, %d %d.}", pst_mac_vap, pst_mac_vap);
        return OAL_FAIL;
    }

    pst_mgmt_tx = (mac_mgmt_frame_stru *)puc_param;

    pst_mac_device  = mac_res_get_dev_etc(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_P2P, "{hmac_wpas_mgmt_tx_etc::pst_mac_device null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_P2P, "{hmac_wpas_mgmt_tx_etc::mgmt frame id=[%d]}", pst_mgmt_tx->mgmt_frame_id);

    /*  ����netbuf �ռ�*/
    pst_netbuf_mgmt_tx = (oal_netbuf_stru *)OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, pst_mgmt_tx->us_len, OAL_NETBUF_PRIORITY_MID);

    if(OAL_PTR_NULL == pst_netbuf_mgmt_tx)
    {
        /* Reserved Memory pool tried for high priority deauth frames */
        pst_netbuf_mgmt_tx = (oal_netbuf_stru *)OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF,WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);
        if(OAL_PTR_NULL == pst_netbuf_mgmt_tx)
        {
            OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_P2P, "{hmac_wpas_mgmt_tx_etc::pst_mgmt_tx null.}");
            return OAL_FAIL;
        }
    }
    OAL_MEM_NETBUF_TRACE(pst_netbuf_mgmt_tx, OAL_TRUE);

    OAL_MEMZERO(oal_netbuf_cb(pst_netbuf_mgmt_tx), OAL_SIZEOF(mac_tx_ctl_stru));

    /*���netbuf*/
    oal_memcopy( (oal_uint8 *)OAL_NETBUF_HEADER(pst_netbuf_mgmt_tx), pst_mgmt_tx->puc_frame, pst_mgmt_tx->us_len);
    oal_netbuf_put(pst_netbuf_mgmt_tx, pst_mgmt_tx->us_len);
    mac_get_address1((oal_uint8 *)pst_mgmt_tx->puc_frame, ua_dest_mac_addr);
    us_user_idx = MAC_INVALID_USER_ID;
    /* ����֡���ܷ����ѹ������û���Ҳ���ܷ���δ�������û����ѹ������û������ҵ���δ�������û��Ҳ����������жϷ���ֵ */
    ul_ret = mac_vap_find_user_by_macaddr_etc(pst_mac_vap, ua_dest_mac_addr, &us_user_idx);

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_netbuf_mgmt_tx);    /* ��ȡcb�ṹ�� */
    MAC_GET_CB_MPDU_LEN(pst_tx_ctl)       = pst_mgmt_tx->us_len;         /* dmac������Ҫ��mpdu���� */
    MAC_GET_CB_TX_USER_IDX(pst_tx_ctl)    = us_user_idx;                 /* ���������Ҫ��ȡuser�ṹ�� */
    MAC_GET_CB_IS_NEED_RESP(pst_tx_ctl)   = OAL_TRUE;
    MAC_GET_CB_IS_NEEDRETRY(pst_tx_ctl)   = OAL_TRUE;
    MAC_GET_CB_MGMT_FRAME_ID(pst_tx_ctl)  = pst_mgmt_tx->mgmt_frame_id;
    MAC_GET_CB_FRAME_TYPE(pst_tx_ctl)     = WLAN_CB_FRAME_TYPE_MGMT;   /* ����subtype��ʶ��, ��action������dmac_tx_action_process */
    MAC_GET_CB_FRAME_SUBTYPE(pst_tx_ctl)  = WLAN_ACTION_P2PGO_FRAME_SUBTYPE;

    /* Buffer this frame in the Memory Queue for transmission */
    ul_ret = hmac_tx_mgmt_send_event_etc(pst_mac_vap, pst_netbuf_mgmt_tx, pst_mgmt_tx->us_len);
    if (OAL_SUCC != ul_ret)
    {
        oal_netbuf_free(pst_netbuf_mgmt_tx);

        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_P2P, "{hmac_wpas_mgmt_tx_etc::hmac_tx_mgmt_send_event_etc failed[%d].}", ul_ret);
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

#if defined(_PRE_WLAN_FEATURE_HS20) || defined(_PRE_WLAN_FEATURE_P2P) || defined(_PRE_WLAN_FEATURE_HILINK) || defined(_PRE_WLAN_FEATURE_11R_AP)

oal_void hmac_rx_mgmt_send_to_host_etc(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf)
{
    mac_rx_ctl_stru            *pst_rx_info;
    oal_int                     l_freq = 0;

    pst_rx_info  = (mac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf);

    l_freq = oal_ieee80211_channel_to_frequency(pst_rx_info->uc_channel_number,
                                                (pst_rx_info->uc_channel_number > 14) ? IEEE80211_BAND_5GHZ : IEEE80211_BAND_2GHZ);
    hmac_send_mgmt_to_host_etc(pst_hmac_vap, pst_netbuf, pst_rx_info->us_frame_len, l_freq);
}
#endif
#ifdef _PRE_WLAN_FEATURE_LOCATION

oal_uint8                     g_auc_send_csi_buf[HMAC_CSI_SEND_BUF_LEN] = {0};
oal_uint8                     g_auc_send_ftm_buf[HMAC_FTM_SEND_BUF_LEN] = {0};


OAL_STATIC oal_uint32  hmac_netlink_location_send(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf)
{
    hmac_location_event_stru      *pst_location_event;
    struct timeval                 st_tv;
    oal_time_stru                  st_local_time;
    oal_uint16                     us_action_len;
    oal_uint8                     *puc_payload;
    mac_rx_ctl_stru               *pst_rx_ctrl;
    oal_uint32                     ul_index = 0;
    oal_uint32                    *pul_len = OAL_PTR_NULL;
    oal_uint8                     *puc_send_csi_buf = g_auc_send_csi_buf;
    oal_uint8                     *puc_send_ftm_buf = g_auc_send_ftm_buf;

    pst_location_event    = (hmac_location_event_stru *)mac_netbuf_get_payload(pst_netbuf);
    pst_rx_ctrl           = (mac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf);
    us_action_len         = pst_rx_ctrl->us_frame_len;

    if (us_action_len < MAC_CSI_LOCATION_INFO_LEN)
    {
        OAM_ERROR_LOG1(0, OAM_SF_FTM, "{hmac_netlink_location_send: unexpected len %d}", us_action_len);
        return OAL_ERR_CODE_MSG_LENGTH_ERR;
    }
    do_gettimeofday(&st_tv);
    OAL_GET_REAL_TIME(&st_local_time);

    switch (pst_location_event->uc_location_type)
    {
        case MAC_HISI_LOCATION_RSSI_IE:

            break;
        case MAC_HISI_LOCATION_CSI_IE:
            puc_payload = (oal_uint8 *)(pst_location_event->auc_payload);

            /*��һƬ*/
            if((0 == puc_payload[0])
               &&((0 == puc_payload[1])||(1 == puc_payload[1])))
            {
                OAL_MEMZERO(puc_send_csi_buf, HMAC_CSI_SEND_BUF_LEN);

                /*Type 4Bytes*/
                *(oal_uint32 *)&puc_send_csi_buf[ul_index] = (oal_uint32)2;
                ul_index += 4;

                /*len 4Bytes*/
                pul_len = (oal_uint32 *)&puc_send_csi_buf[ul_index];
                ul_index += 4;

                /*mac1 6Bytes*/
                oal_memcopy(&puc_send_csi_buf[ul_index], pst_location_event->auc_mac_server, WLAN_MAC_ADDR_LEN);
                ul_index += WLAN_MAC_ADDR_LEN;

                /*mac2 6Bytes*/
                oal_memcopy(&puc_send_csi_buf[ul_index], pst_location_event->auc_mac_client, WLAN_MAC_ADDR_LEN);
                ul_index += WLAN_MAC_ADDR_LEN;

                /*timestamp23Bytes*/
                ul_index += OAL_SPRINTF(puc_send_csi_buf + ul_index, HMAC_FTM_SEND_BUF_LEN,
                                        "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
                                        st_local_time.tm_year+1900, st_local_time.tm_mon+1, st_local_time.tm_mday,
                                        st_local_time.tm_hour, st_local_time.tm_min, st_local_time.tm_sec, st_tv.tv_usec/1000);

                /*rssi snr*/
                oal_memcopy(&puc_send_csi_buf[ul_index], puc_payload + 3 , MAC_REPORT_RSSIINFO_SNR_LEN);

                *pul_len = ul_index + MAC_REPORT_RSSIINFO_SNR_LEN;
                OAM_WARNING_LOG1(0, OAM_SF_FTM, "{hmac_netlink_location_send::send len[%d].}", *pul_len);

            }

            pul_len = (oal_uint32 *)&puc_send_csi_buf[4];

            OAM_WARNING_LOG2(0, OAM_SF_FTM, "{hmac_netlink_location_send::len[%d], copy_len[%d].}", *pul_len, us_action_len - MAC_CSI_LOCATION_INFO_LEN);
            OAM_WARNING_LOG3(0, OAM_SF_FTM, "{hmac_netlink_location_send::frag [%d], [%d] ,[%d].}", puc_payload[0], puc_payload[1], puc_payload[2]);

            if(*pul_len + us_action_len - MAC_CSI_LOCATION_INFO_LEN > HMAC_CSI_SEND_BUF_LEN)
            {
                OAM_ERROR_LOG0(0, OAM_SF_FTM, "{hmac_netlink_location_send::puc_send_buf not enough.}");
                return OAL_FAIL;
            }

            oal_memcopy(&puc_send_csi_buf[*pul_len], puc_payload + 3 + MAC_REPORT_RSSIINFO_SNR_LEN , us_action_len - MAC_CSI_LOCATION_INFO_LEN);
            *pul_len += us_action_len - MAC_CSI_LOCATION_INFO_LEN;

            /*���һƬ*/
            if(1 == pst_location_event->auc_payload[2])
            {
                drv_netlink_location_send((oal_void *)puc_send_csi_buf, *pul_len);
                OAM_WARNING_LOG1(0, OAM_SF_FTM, "{hmac_netlink_location_send::send len[%d].}", *pul_len);
            }

            break;
        case MAC_HISI_LOCATION_FTM_IE:
            OAL_MEMZERO(puc_send_ftm_buf, HMAC_FTM_SEND_BUF_LEN);
            *(oal_uint32 *)&puc_send_ftm_buf[ul_index] = (oal_uint32)3;
            ul_index += 4;
            *(oal_uint32 *)&puc_send_ftm_buf[ul_index] = 99;
            ul_index += 4;
            oal_memcopy(&puc_send_ftm_buf[ul_index], pst_location_event->auc_mac_server, WLAN_MAC_ADDR_LEN);
            ul_index += WLAN_MAC_ADDR_LEN;
            oal_memcopy(&puc_send_ftm_buf[ul_index], pst_location_event->auc_mac_client, WLAN_MAC_ADDR_LEN);
            ul_index += WLAN_MAC_ADDR_LEN;
            ul_index += OAL_SPRINTF(puc_send_ftm_buf + ul_index, HMAC_FTM_SEND_BUF_LEN,
                                    "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
                                    st_local_time.tm_year+1900, st_local_time.tm_mon+1, st_local_time.tm_mday,
                                    st_local_time.tm_hour, st_local_time.tm_min, st_local_time.tm_sec, st_tv.tv_usec/1000);
            puc_payload = (oal_uint8 *)(pst_location_event->auc_payload);

            oal_memcopy(&puc_send_ftm_buf[ul_index], puc_payload, 56);
            drv_netlink_location_send((oal_void *)puc_send_ftm_buf, 99);
            break;
        default:
            return OAL_SUCC;
    }

    return OAL_SUCC;
}
#endif
#if defined(_PRE_WLAN_FEATURE_LOCATION) || defined(_PRE_WLAN_FEATURE_PSD_ANALYSIS)

#if 0
OAL_STATIC oal_int32 hmac_mac2str(oal_uint8 *puc_str, oal_uint8 *puc_mac)
{
    oal_uint8    uc_index;

    for (uc_index = 0; uc_index < OAL_MAC_ADDR_LEN; uc_index++)
    {
       OAL_SPRINTF(puc_str, OAL_MAC_ADDR_LEN, "%02X", puc_mac[uc_index]);
       puc_str += 2;
    }
    puc_str[0] = '\0';

    return 12;
}
#endif


OAL_STATIC oal_uint32  hmac_proc_location_action(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf)
{
    hmac_location_event_stru      *pst_location_event;
    struct timeval                 st_tv;
    oal_time_stru                  st_local_time;
    oal_uint8                      auc_filename[128];
    oal_uint16                     us_action_len;
    oal_uint16                     us_delta_len;
    //oal_uint16                     us_info_idx;
    oal_int32                      l_str_len = 0;
    oal_file                      *f_file;
    oal_mm_segment_t               old_fs;
    oal_uint8                     *puc_payload;
    mac_rx_ctl_stru               *pst_rx_ctrl;
    oal_int8                      *pc_psd_payload;

    /* Vendor Public Action Header| EID |Length |OUI | type | mac_s | mac_c | rssi */
    /* ��ȡ����ʱ�侫ȷ��us 2017-11-03-23-50-12-xxxxxxxx */
    //us_info_idx = 13;
    //us_action_len         = OAL_NETBUF_LEN(pst_netbuf);
    pst_location_event    = (hmac_location_event_stru *)mac_netbuf_get_payload(pst_netbuf);
    pst_rx_ctrl           = (mac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf);
    us_action_len         = pst_rx_ctrl->us_frame_len;

    if (us_action_len < 45)
    {
        OAM_ERROR_LOG1(0, OAM_SF_FTM, "{hmac_proc_location_action: unexpected len %d}", us_action_len);
        return OAL_ERR_CODE_MSG_LENGTH_ERR;
    }
    do_gettimeofday(&st_tv);
    OAL_GET_REAL_TIME(&st_local_time);

    /* ��ȡ�ļ�·��\data\log\location\wlan0\ */
    l_str_len = OAL_SPRINTF(auc_filename, OAL_SIZEOF(auc_filename),"/data/log/location/%s/", pst_hmac_vap->auc_name);
    #if 0
    l_str_len += hmac_mac2str(auc_filename + l_str_len, pst_location_event->auc_mac_server);
    l_str_len += OAL_SPRINTF(auc_filename + l_str_len, OAL_SIZEOF(auc_filename) - l_str_len,"-");
    l_str_len += hmac_mac2str(auc_filename + l_str_len, pst_location_event->auc_mac_client);
    #endif

    OAM_WARNING_LOG1(0, OAM_SF_FTM, "{hmac_proc_location_action::location type[%d]}", pst_location_event->uc_location_type);

    switch (pst_location_event->uc_location_type)
    {
        case MAC_HISI_LOCATION_RSSI_IE:
            /* ��ȡ�ļ��� MAC_ADDR_S_MAC_ADDR_C_RSSI */
            l_str_len += OAL_SPRINTF(auc_filename + l_str_len, OAL_SIZEOF(auc_filename) - l_str_len,"RSSI.TXT");

            break;
        case MAC_HISI_LOCATION_CSI_IE:
            /* ��ȡ�ļ��� MAC_ADDR_S_MAC_ADDR_C_CSI */
            l_str_len += OAL_SPRINTF(auc_filename + l_str_len, OAL_SIZEOF(auc_filename) - l_str_len,"CSI.TXT");

            break;
        case MAC_HISI_LOCATION_FTM_IE:
            /* ��ȡ�ļ��� MAC_ADDR_S_MAC_ADDR_C_FTM */
            l_str_len += OAL_SPRINTF(auc_filename + l_str_len, OAL_SIZEOF(auc_filename) - l_str_len,"FTM.TXT");

            break;
        case MAC_HISI_PSD_SAVE_FILE_IE:
            /* ��ȡ�ļ��� MAC_ADDR_S_MAC_ADDR_C_FTM */
            l_str_len += OAL_SPRINTF(auc_filename + l_str_len, OAL_SIZEOF(auc_filename) - l_str_len,"PSD.TXT");

        break;
        default:
            return OAL_SUCC;
    }

    f_file = oal_kernel_file_open_etc(auc_filename,OAL_O_RDWR|OAL_O_CREAT|OAL_O_APPEND);
    if (IS_ERR_OR_NULL(f_file))
    {
        OAM_ERROR_LOG1(0, OAM_SF_FTM, "{hmac_proc_location_action: ****************save file failed %d }", l_str_len);

        return OAL_ERR_CODE_OPEN_FILE_FAIL;
    }
    old_fs = oal_get_fs();

    /* ����CSI��˵��payload[0]��ʾ��ǰ�ķ�Ƭ���кţ�payload[1]��ʾ�ڴ��ֶ����кţ�0��ʾ���ֶΣ�1��ʾ��һ���ֶ�
       payload[2]��ʾ��ǰ��Ƭ�ǲ������һƬ  */
    if(MAC_HISI_LOCATION_CSI_IE == pst_location_event->uc_location_type)
    {
        if((0 == pst_location_event->auc_payload[0])
           &&((0 == pst_location_event->auc_payload[1])||(1 == pst_location_event->auc_payload[1])))
        {
            oal_kernel_file_print_etc(f_file, "%04d-%02d-%02d-", st_local_time.tm_year+1900, st_local_time.tm_mon+1, st_local_time.tm_mday);
            oal_kernel_file_print_etc(f_file, "%02d-%02d-%02d-%08d : ", st_local_time.tm_hour, st_local_time.tm_min, st_local_time.tm_sec, st_tv.tv_usec);
            /*rssi snr*/
            puc_payload = (oal_uint8 *)(pst_location_event->auc_payload);
            l_str_len = 0;
            while (l_str_len < MAC_REPORT_RSSIINFO_SNR_LEN)
            {
                oal_kernel_file_print_etc(f_file, "%02X ", *(puc_payload++));
                l_str_len++;
            }
        }
    }
    else
    {
        oal_kernel_file_print_etc(f_file, "%04d-%02d-%02d-", st_local_time.tm_year+1900, st_local_time.tm_mon+1, st_local_time.tm_mday);
        oal_kernel_file_print_etc(f_file, "%02d-%02d-%02d-%08d : ", st_local_time.tm_hour, st_local_time.tm_min, st_local_time.tm_sec, st_tv.tv_usec);
    }

    l_str_len = 0;
    puc_payload = (oal_uint8 *)(pst_location_event->auc_payload);

    if(MAC_HISI_LOCATION_CSI_IE == pst_location_event->uc_location_type)
    {
        us_delta_len = us_action_len - MAC_CSI_LOCATION_INFO_LEN;
        puc_payload += MAC_REPORT_RSSIINFO_SNR_LEN + 3 ;
    }
    else
    {
        us_delta_len = us_action_len - MAC_FTM_LOCATION_INFO_LEN + MAC_REPORT_RSSIINFO_LEN;
    }

    if(MAC_HISI_PSD_SAVE_FILE_IE != pst_location_event->uc_location_type)
    {
        while (l_str_len < us_delta_len)
        {
            oal_kernel_file_print_etc(f_file, "%02X ", *(puc_payload++));
            l_str_len++;
        }
    }
    else
    {
        pc_psd_payload = (oal_int8 *)(pst_location_event->auc_payload);
        while (l_str_len < us_delta_len)
        {
            oal_kernel_file_print_etc(f_file, "%d ", *(pc_psd_payload++));
            l_str_len++;
        }
    }

#if 1
   /* δ����80M���� TBD */
   if((MAC_HISI_LOCATION_RSSI_IE == pst_location_event->uc_location_type)
       || (MAC_HISI_LOCATION_FTM_IE == pst_location_event->uc_location_type)
       || (MAC_HISI_PSD_SAVE_FILE_IE == pst_location_event->uc_location_type)
       ||((MAC_HISI_LOCATION_CSI_IE == pst_location_event->uc_location_type)
       && ((1 == pst_location_event->auc_payload[2])
       && ((0 == pst_location_event->auc_payload[1])||(2 == pst_location_event->auc_payload[1])))))
    {
        l_str_len += oal_kernel_file_print_etc(f_file, "\n");
    }
#endif
    oal_kernel_file_close(f_file);
    oal_set_fs(old_fs);

    return OAL_SUCC;
}

oal_uint32 hmac_huawei_action_process(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf, oal_uint8 uc_type)
{
    switch (uc_type)
    {
        case MAC_HISI_LOCATION_RSSI_IE:
        case MAC_HISI_LOCATION_CSI_IE:
        case MAC_HISI_LOCATION_FTM_IE:
        case MAC_HISI_PSD_SAVE_FILE_IE:
            /* �������豸�ϱ���˽����Ϣȥ��ieͷ���¼���hmac���б��� */
            /* type | mac_s | mac_c   | csi or ftm or rssi   */
            /* csi ��Ϣע�ⳤ�� */
            hmac_proc_location_action(pst_hmac_vap, pst_netbuf);
#ifdef _PRE_WLAN_FEATURE_LOCATION
            hmac_netlink_location_send(pst_hmac_vap, pst_netbuf);
#endif
            break;
        default:
            break;
    }

    return OAL_SUCC;
}
#endif

oal_uint32  hmac_mgmt_tx_event_status_etc(mac_vap_stru *pst_mac_vap, oal_uint8 uc_len, oal_uint8 *puc_param)
{
    dmac_crx_mgmt_tx_status_stru    *pst_mgmt_tx_status_param;
    dmac_crx_mgmt_tx_status_stru    *pst_mgmt_tx_status_param_2wal;
    frw_event_mem_stru              *pst_event_mem;
    frw_event_stru                  *pst_event;
    //mac_cfg_del_user_param_stru      st_del_user;
    oal_uint32                       ul_ret;
    //hmac_user_stru                  *pst_hmac_user;
    //mac_device_stru                 *pst_mac_device;
    //oal_uint8                        auc_user_mac_adr[WLAN_MAC_ADDR_LEN] = {0};

    if (OAL_PTR_NULL == puc_param)
    {
        OAM_ERROR_LOG0(0, OAM_SF_P2P, "{hmac_mgmt_tx_event_status_etc::puc_param is null!}\r\n");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mgmt_tx_status_param = (dmac_crx_mgmt_tx_status_stru *)puc_param;

    OAM_WARNING_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_P2P, "{hmac_mgmt_tx_event_status_etc::dmac tx mgmt status report.userindx[%d], tx mgmt status[%d], frame_id[%d]}",
                           pst_mgmt_tx_status_param->us_user_idx,
                           pst_mgmt_tx_status_param->uc_dscr_status,
                           pst_mgmt_tx_status_param->mgmt_frame_id);

    /* �׹���֡��������¼���WAL */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_crx_mgmt_tx_status_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_P2P, "{hmac_mgmt_tx_event_status_etc::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��д�¼� */
    pst_event = frw_get_event_stru(pst_event_mem);

    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_HOST_CTX,
                       HMAC_HOST_CTX_EVENT_SUB_TYPE_MGMT_TX_STATUS,
                       OAL_SIZEOF(dmac_crx_mgmt_tx_status_stru),
                       FRW_EVENT_PIPELINE_STAGE_0,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    pst_mgmt_tx_status_param_2wal = (dmac_crx_mgmt_tx_status_stru *)(pst_event->auc_event_data);
    pst_mgmt_tx_status_param_2wal->uc_dscr_status = pst_mgmt_tx_status_param->uc_dscr_status;
    pst_mgmt_tx_status_param_2wal->mgmt_frame_id  = pst_mgmt_tx_status_param->mgmt_frame_id;

    /* �ַ��¼� */
    ul_ret = frw_event_dispatch_event_etc(pst_event_mem);
    FRW_EVENT_FREE(pst_event_mem);


    return ul_ret;
}


oal_void  hmac_vap_set_user_avail_rates_etc(mac_vap_stru *pst_mac_vap, hmac_user_stru *pst_hmac_user)
{
    mac_user_stru         *pst_mac_user;
    mac_curr_rateset_stru *pst_mac_vap_rate;
    hmac_rate_stru         *pst_hmac_user_rate;
    mac_rate_stru          st_avail_op_rates;
    oal_uint8              uc_mac_vap_rate_num;
    oal_uint8              uc_hmac_user_rate_num;
    oal_uint8              uc_vap_rate_idx;
    oal_uint8              uc_user_rate_idx;
    oal_uint8              uc_user_avail_rate_idx = 0;

    /* ��ȡVAP��USER���ʵĽṹ��ָ�� */
    pst_mac_user        = &(pst_hmac_user->st_user_base_info);
    pst_mac_vap_rate    = &(pst_mac_vap->st_curr_sup_rates);
    pst_hmac_user_rate   = &(pst_hmac_user->st_op_rates);
    OAL_MEMZERO((oal_uint8 *)(&st_avail_op_rates), OAL_SIZEOF(mac_rate_stru));

    uc_mac_vap_rate_num     = pst_mac_vap_rate->st_rate.uc_rs_nrates;
    uc_hmac_user_rate_num    = pst_hmac_user_rate->uc_rs_nrates;

    for (uc_vap_rate_idx = 0; uc_vap_rate_idx < uc_mac_vap_rate_num; uc_vap_rate_idx++)
    {
        for(uc_user_rate_idx = 0; uc_user_rate_idx < uc_hmac_user_rate_num; uc_user_rate_idx++)
        {
            if ((pst_mac_vap_rate->st_rate.ast_rs_rates[uc_vap_rate_idx].uc_mac_rate > 0) &&
                (IS_EQUAL_RATES(pst_mac_vap_rate->st_rate.ast_rs_rates[uc_vap_rate_idx].uc_mac_rate, pst_hmac_user_rate->auc_rs_rates[uc_user_rate_idx])))
            {
                st_avail_op_rates.auc_rs_rates[uc_user_avail_rate_idx] = (pst_hmac_user_rate->auc_rs_rates[uc_user_rate_idx] & 0x7F);
                uc_user_avail_rate_idx++;
                st_avail_op_rates.uc_rs_nrates++;
            }
        }
    }

    mac_user_set_avail_op_rates_etc(pst_mac_user, st_avail_op_rates.uc_rs_nrates, st_avail_op_rates.auc_rs_rates);
    OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_vap_set_user_avail_rates_etc::uc_avail_op_rates_num=[%d].}",st_avail_op_rates.uc_rs_nrates);
}

oal_uint32 hmac_proc_ht_cap_ie_etc(mac_vap_stru *pst_mac_vap, mac_user_stru *pst_mac_user, oal_uint8 *puc_ht_cap_ie)
{
    oal_uint8                           uc_mcs_bmp_index;
    mac_user_ht_hdl_stru               *pst_ht_hdl;
    oal_uint16                          us_tmp_info_elem;
    oal_uint16                          us_tmp_txbf_low;
    oal_uint16                          us_ht_cap_info;
    oal_uint32                          ul_tmp_txbf_elem;
    oal_uint16                          us_offset =  0;

    if ((OAL_PTR_NULL == pst_mac_vap) ||
        (OAL_PTR_NULL == pst_mac_user) ||
        (OAL_PTR_NULL == puc_ht_cap_ie))
    {
        OAM_WARNING_LOG3(0, OAM_SF_ROAM,
                        "{hmac_proc_ht_cap_ie_etc::PARAM NULL! vap=[0x%X],user=[0x%X],cap_ie=[0x%X].}", pst_mac_vap, pst_mac_user, puc_ht_cap_ie);
        return OAL_ERR_CODE_PTR_NULL;
    }

     /* ����֧��11n�Ž��к����Ĵ��� */
    if (OAL_FALSE == mac_mib_get_HighThroughputOptionImplemented(pst_mac_vap))
    {
        return OAL_SUCC;
    }

    mac_user_set_ht_capable_etc(pst_mac_user, OAL_TRUE);

    pst_ht_hdl      = &pst_mac_user->st_ht_hdl;

    /* ���� HT Capability Element �� AP����ʾ������HT capable. */
    pst_ht_hdl->en_ht_capable = OAL_TRUE;

    us_offset += MAC_IE_HDR_LEN;

    /********************************************/
    /*     ���� HT Capabilities Info Field      */
    /********************************************/
    us_ht_cap_info = OAL_MAKE_WORD16(puc_ht_cap_ie[us_offset], puc_ht_cap_ie[us_offset + 1]);

    /* ���STA��֧�ֵ�LDPC�������� B0��0:��֧�֣�1:֧�� */
    pst_ht_hdl->bit_ldpc_coding_cap = (us_ht_cap_info & BIT0);

    /* ��ȡAP��֧�ֵĴ�������  */
    pst_ht_hdl->bit_supported_channel_width = ((us_ht_cap_info & BIT1) >> 1);

    /* ���ռ临�ý���ģʽ B2~B3 */
    mac_ie_proc_sm_power_save_field_etc(pst_mac_user, (oal_uint8)((us_ht_cap_info & (BIT3 | BIT2)) >> 2));

    /* ��ȡAP֧��Greenfield��� */
    pst_ht_hdl->bit_ht_green_field = ((us_ht_cap_info & BIT4) >> 4);

    /* ��ȡAP֧��20MHz Short-GI��� */
    pst_ht_hdl->bit_short_gi_20mhz = ((us_ht_cap_info & BIT5) >> 5);

    /* ��ȡAP֧��40MHz Short-GI��� */
    pst_ht_hdl->bit_short_gi_40mhz = ((us_ht_cap_info & BIT6) >> 6);

    /* ��ȡAP֧��STBC PPDU��� */
    pst_ht_hdl->bit_rx_stbc = (oal_uint8)((us_ht_cap_info & (BIT9 | BIT8)) >> 8);

    /* ��ȡAP֧�����A-MSDU������� */
// *pus_amsdu_maxsize = (0 == (us_ht_cap_info & BIT11)) ? WLAN_MIB_MAX_AMSDU_LENGTH_SHORT : WLAN_MIB_MAX_AMSDU_LENGTH_LONG;

    /* ��ȡAP 40M��DSSS/CCK��֧����� */
    pst_ht_hdl->bit_dsss_cck_mode_40mhz = ((us_ht_cap_info & BIT12) >> 12);

    /* ��ȡAP L-SIG TXOP ������֧����� */
    pst_ht_hdl->bit_lsig_txop_protection = ((us_ht_cap_info & BIT15) >> 15);

    us_offset += MAC_HT_CAPINFO_LEN;

    /********************************************/
    /*     ���� A-MPDU Parameters Field         */
    /********************************************/

    /* ��ȡ Maximum Rx A-MPDU factor (B1 - B0) */
    pst_ht_hdl->uc_max_rx_ampdu_factor = (puc_ht_cap_ie[us_offset] & 0x03);

    /* ��ȡ Minmum Rx A-MPDU factor (B3 - B2) */
    pst_ht_hdl->uc_min_mpdu_start_spacing = (puc_ht_cap_ie[us_offset] >> 2) & 0x07;

    us_offset += MAC_HT_AMPDU_PARAMS_LEN;

    /********************************************/
    /*     ���� Supported MCS Set Field         */
    /********************************************/
    for(uc_mcs_bmp_index = 0; uc_mcs_bmp_index < WLAN_HT_MCS_BITMASK_LEN; uc_mcs_bmp_index++)
    {
        pst_ht_hdl->uc_rx_mcs_bitmask[uc_mcs_bmp_index] =
        (mac_mib_get_SupportedMCSTxValue(pst_mac_vap, uc_mcs_bmp_index))&
        (*(oal_uint8 *)(puc_ht_cap_ie + us_offset + uc_mcs_bmp_index));
    }

    pst_ht_hdl->uc_rx_mcs_bitmask[WLAN_HT_MCS_BITMASK_LEN - 1] &= 0x1F;

    us_offset += MAC_HT_SUP_MCS_SET_LEN;

    /********************************************/
    /* ���� HT Extended Capabilities Info Field */
    /********************************************/
    us_ht_cap_info = OAL_MAKE_WORD16(puc_ht_cap_ie[us_offset], puc_ht_cap_ie[us_offset + 1]);

    /* ��ȡ HTC support Information */
    pst_ht_hdl->uc_htc_support = ((us_ht_cap_info & BIT10) >> 10);

    us_offset += MAC_HT_EXT_CAP_LEN;

    /********************************************/
    /*  ���� Tx Beamforming Field               */
    /********************************************/
    us_tmp_info_elem = OAL_MAKE_WORD16(puc_ht_cap_ie[us_offset], puc_ht_cap_ie[us_offset + 1]);
    us_tmp_txbf_low  = OAL_MAKE_WORD16(puc_ht_cap_ie[us_offset + 2], puc_ht_cap_ie[us_offset + 3]);
    ul_tmp_txbf_elem = OAL_MAKE_WORD32(us_tmp_info_elem, us_tmp_txbf_low);
    pst_ht_hdl->bit_imbf_receive_cap                = (ul_tmp_txbf_elem & BIT0);
    pst_ht_hdl->bit_receive_staggered_sounding_cap  = ((ul_tmp_txbf_elem & BIT1) >> 1);
    pst_ht_hdl->bit_transmit_staggered_sounding_cap = ((ul_tmp_txbf_elem & BIT2) >> 2);
    pst_ht_hdl->bit_receive_ndp_cap                 = ((ul_tmp_txbf_elem & BIT3) >> 3);
    pst_ht_hdl->bit_transmit_ndp_cap                = ((ul_tmp_txbf_elem & BIT4) >> 4);
    pst_ht_hdl->bit_imbf_cap                        = ((ul_tmp_txbf_elem & BIT5) >> 5);
    pst_ht_hdl->bit_calibration                     = ((ul_tmp_txbf_elem & 0x000000C0) >> 6);
    pst_ht_hdl->bit_exp_csi_txbf_cap                = ((ul_tmp_txbf_elem & BIT8) >> 8);
    pst_ht_hdl->bit_exp_noncomp_txbf_cap            = ((ul_tmp_txbf_elem & BIT9) >> 9);
    pst_ht_hdl->bit_exp_comp_txbf_cap               = ((ul_tmp_txbf_elem & BIT10) >> 10);
    pst_ht_hdl->bit_exp_csi_feedback                = ((ul_tmp_txbf_elem & 0x00001800) >> 11);
    pst_ht_hdl->bit_exp_noncomp_feedback            = ((ul_tmp_txbf_elem & 0x00006000) >> 13);
    pst_ht_hdl->bit_exp_comp_feedback               = ((ul_tmp_txbf_elem & 0x0001C000) >> 15);
    pst_ht_hdl->bit_min_grouping                    = ((ul_tmp_txbf_elem & 0x00060000) >> 17);
    pst_ht_hdl->bit_csi_bfer_ant_number             = ((ul_tmp_txbf_elem & 0x001C0000) >> 19);
    pst_ht_hdl->bit_noncomp_bfer_ant_number         = ((ul_tmp_txbf_elem & 0x00600000) >> 21);
    pst_ht_hdl->bit_comp_bfer_ant_number            = ((ul_tmp_txbf_elem & 0x01C00000) >> 23);
    pst_ht_hdl->bit_csi_bfee_max_rows               = ((ul_tmp_txbf_elem & 0x06000000) >> 25);
    pst_ht_hdl->bit_channel_est_cap                 = ((ul_tmp_txbf_elem & 0x18000000) >> 27);


    return OAL_SUCC;
}


oal_uint32  hmac_proc_vht_cap_ie_etc(mac_vap_stru *pst_mac_vap, hmac_user_stru *pst_hmac_user, oal_uint8 *puc_vht_cap_ie)
{
    mac_vht_hdl_stru   *pst_mac_vht_hdl;
    mac_vht_hdl_stru    st_mac_vht_hdl;
    mac_user_stru      *pst_mac_user;
    oal_uint16          us_vht_cap_filed_low;
    oal_uint16          us_vht_cap_filed_high;
    oal_uint32          ul_vht_cap_field;
    oal_uint16          us_rx_mcs_map;
    oal_uint16          us_tx_mcs_map;
    oal_uint16          us_rx_highest_supp_logGi_data;
    oal_uint16          us_tx_highest_supp_logGi_data;
    oal_uint16          us_msg_idx = 0;

    /* ����vht cap IE */
    if ((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == puc_vht_cap_ie))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ANY, "{hmac_proc_vht_cap_ie_etc::param null,mac_vap[0x%x], hmac_user[0x%x], vht_cap_ie[0x%x].}", pst_mac_vap, pst_hmac_user, puc_vht_cap_ie);

        return OAL_ERR_CODE_PTR_NULL;
    }

    if (puc_vht_cap_ie[1] < MAC_VHT_CAP_IE_LEN)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_proc_vht_cap_ie_etc::invalid vht cap ie len[%d].}", puc_vht_cap_ie[1]);
        return OAL_FAIL;
    }

    pst_mac_user = &pst_hmac_user->st_user_base_info;

    /* ֧��11ac���Ž��к����Ĵ��� */
    if (OAL_FALSE == mac_mib_get_VHTOptionImplemented(pst_mac_vap))
    {
        return OAL_SUCC;
    }

    pst_mac_vht_hdl = &st_mac_vht_hdl;
    mac_user_get_vht_hdl_etc(pst_mac_user, pst_mac_vht_hdl);

    /* ����˺�������user֧��11ac */
    pst_mac_vht_hdl->en_vht_capable = OAL_TRUE;

#ifdef _PRE_WLAN_FEATURE_11AC2G
    /* ���ƻ�ʵ�������֧��11ac2gģʽ����ص�vht cap */
    if((OAL_FALSE == pst_mac_vap->st_cap_flag.bit_11ac2g)
       && (WLAN_BAND_2G == pst_mac_vap->st_channel.en_band))
    {
        pst_mac_vht_hdl->en_vht_capable = OAL_FALSE;
    }
#endif

    us_msg_idx += MAC_IE_HDR_LEN;

    /* ����VHT capablities info field */
    us_vht_cap_filed_low    = OAL_MAKE_WORD16(puc_vht_cap_ie[us_msg_idx], puc_vht_cap_ie[us_msg_idx + 1]);
    us_vht_cap_filed_high   = OAL_MAKE_WORD16(puc_vht_cap_ie[us_msg_idx + 2], puc_vht_cap_ie[us_msg_idx + 3]);
    ul_vht_cap_field        = OAL_MAKE_WORD32(us_vht_cap_filed_low, us_vht_cap_filed_high);

    /* ����max_mpdu_length �μ�11acЭ�� Table 8-183u*/
    pst_mac_vht_hdl->bit_max_mpdu_length = (ul_vht_cap_field & (BIT1 |BIT0));
    if (0 == pst_mac_vht_hdl->bit_max_mpdu_length)
    {
        pst_mac_vht_hdl->us_max_mpdu_length = 3895;
    }
    else if (1 == pst_mac_vht_hdl->bit_max_mpdu_length)
    {
        pst_mac_vht_hdl->us_max_mpdu_length = 7991;
    }
    else if (2 == pst_mac_vht_hdl->bit_max_mpdu_length)
    {
        pst_mac_vht_hdl->us_max_mpdu_length = 11454;
    }

    /* ����supported_channel_width */
    pst_mac_vht_hdl->bit_supported_channel_width = ((ul_vht_cap_field & (BIT3 |BIT2)) >> 2);

    /* ����rx_ldpc */
    pst_mac_vht_hdl->bit_rx_ldpc = ((ul_vht_cap_field & BIT4) >> 4);

    /* ����short_gi_80mhz��short_gi_160mhz֧����� */
    pst_mac_vht_hdl->bit_short_gi_80mhz = ((ul_vht_cap_field & BIT5) >> 5);
    pst_mac_vht_hdl->bit_short_gi_80mhz &= mac_mib_get_VHTShortGIOptionIn80Implemented(pst_mac_vap);

    pst_mac_vht_hdl->bit_short_gi_160mhz = ((ul_vht_cap_field & BIT6) >> 6);
    pst_mac_vht_hdl->bit_short_gi_160mhz &= mac_mib_get_VHTShortGIOptionIn160and80p80Implemented(pst_mac_vap);

    /* ����tx_stbc ��rx_stbc */
    pst_mac_vht_hdl->bit_tx_stbc = ((ul_vht_cap_field & BIT7) >> 7);
    pst_mac_vht_hdl->bit_rx_stbc = ((ul_vht_cap_field & (BIT10 | BIT9 | BIT8)) >> 8);

    /* ����su_beamformer_cap��su_beamformee_cap */
    pst_mac_vht_hdl->bit_su_beamformer_cap = ((ul_vht_cap_field & BIT11) >> 11);
    pst_mac_vht_hdl->bit_su_beamformee_cap = ((ul_vht_cap_field & BIT12) >> 12);

    /* ����num_bf_ant_supported */
    pst_mac_vht_hdl->bit_num_bf_ant_supported = ((ul_vht_cap_field & (BIT15 | BIT14 | BIT13)) >> 13);

    /*�ԶԶ���������ʼ���������ռ���*/
    pst_mac_user->en_avail_bf_num_spatial_stream = pst_mac_vht_hdl->bit_num_bf_ant_supported;

    /* ����num_sounding_dim */
    pst_mac_vht_hdl->bit_num_sounding_dim =  ((ul_vht_cap_field & (BIT18 | BIT17 | BIT16)) >> 16);

    /* ����mu_beamformer_cap��mu_beamformee_cap */
    pst_mac_vht_hdl->bit_mu_beamformer_cap = ((ul_vht_cap_field & BIT19) >> 19);
    pst_mac_vht_hdl->bit_mu_beamformee_cap = ((ul_vht_cap_field & BIT20) >> 20);

    /* ����vht_txop_ps */
    pst_mac_vht_hdl->bit_vht_txop_ps = ((ul_vht_cap_field & BIT21) >> 21);
#ifdef _PRE_WLAN_FEATURE_TXOPPS
    if (OAL_TRUE == pst_mac_vht_hdl->bit_vht_txop_ps && OAL_TRUE == mac_mib_get_txopps(pst_mac_vap))
    {
        mac_vap_set_txopps(pst_mac_vap, OAL_TRUE);
    }
#endif
    /* ����htc_vht_capable */
    pst_mac_vht_hdl->bit_htc_vht_capable = ((ul_vht_cap_field & BIT22) >> 22);

    /* ����max_ampdu_len_exp */
    pst_mac_vht_hdl->bit_max_ampdu_len_exp = ((ul_vht_cap_field & (BIT25 | BIT24 | BIT23)) >> 23);

    /* ����vht_link_adaptation */
    pst_mac_vht_hdl->bit_vht_link_adaptation = ((ul_vht_cap_field & (BIT27 |BIT26)) >> 26);

    /* ����rx_ant_pattern */
    pst_mac_vht_hdl->bit_rx_ant_pattern = ((ul_vht_cap_field & BIT28) >> 28);

    /* ����tx_ant_pattern */
    pst_mac_vht_hdl->bit_tx_ant_pattern = ((ul_vht_cap_field & BIT29) >> 29);

    us_msg_idx += MAC_VHT_CAP_INFO_FIELD_LEN;

    /* ����VHT Supported MCS Set field */

    us_rx_mcs_map = OAL_MAKE_WORD16(puc_vht_cap_ie[us_msg_idx],puc_vht_cap_ie[us_msg_idx + 1]);

    oal_memcopy(&(pst_mac_vht_hdl->st_rx_max_mcs_map), &us_rx_mcs_map, OAL_SIZEOF(mac_rx_max_mcs_map_stru));

    us_msg_idx += MAC_VHT_CAP_RX_MCS_MAP_FIELD_LEN;

    /* ����rx_highest_supp_logGi_data */
    us_rx_highest_supp_logGi_data = OAL_MAKE_WORD16(puc_vht_cap_ie[us_msg_idx],puc_vht_cap_ie[us_msg_idx + 1]);
    pst_mac_vht_hdl->bit_rx_highest_rate = us_rx_highest_supp_logGi_data & (0x1FFF);

    us_msg_idx += MAC_VHT_CAP_RX_HIGHEST_DATA_FIELD_LEN;

    /* ����tx_mcs_map */
    us_tx_mcs_map = OAL_MAKE_WORD16(puc_vht_cap_ie[us_msg_idx],puc_vht_cap_ie[us_msg_idx + 1]);
    oal_memcopy(&(pst_mac_vht_hdl->st_tx_max_mcs_map), &us_tx_mcs_map, OAL_SIZEOF(mac_tx_max_mcs_map_stru));

    us_msg_idx += MAC_VHT_CAP_TX_MCS_MAP_FIELD_LEN;

    /* ����tx_highest_supp_logGi_data */
    us_tx_highest_supp_logGi_data = OAL_MAKE_WORD16(puc_vht_cap_ie[us_msg_idx],puc_vht_cap_ie[us_msg_idx + 1]);
    pst_mac_vht_hdl->bit_tx_highest_rate = us_tx_highest_supp_logGi_data & (0x1FFF);

    mac_user_set_vht_hdl_etc(pst_mac_user, pst_mac_vht_hdl);
    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_11AX

oal_uint32  hmac_proc_he_cap_ie(mac_vap_stru *pst_mac_vap, hmac_user_stru *pst_hmac_user, oal_uint8 *puc_he_cap_ie)
{
    mac_he_hdl_stru               *pst_mac_he_hdl;
    mac_he_hdl_stru                st_mac_he_hdl;
    mac_user_stru                 *pst_mac_user;
    mac_frame_he_cap_ie_stru       st_he_cap_value;
    oal_uint32                     ul_ret;

    /* ����he cap IE */
    if ((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == puc_he_cap_ie))
    {
        OAM_ERROR_LOG3(0, OAM_SF_11AX, "{hmac_proc_he_cap_ie::param null,mac_vap[0x%x], hmac_user[0x%x], he_cap_ie[0x%x].}", pst_mac_vap, pst_hmac_user, puc_he_cap_ie);

        return OAL_ERR_CODE_PTR_NULL;
    }

    if (puc_he_cap_ie[1] < MAC_HE_CAP_MIN_LEN)
    {
        OAM_WARNING_LOG1(0, OAM_SF_11AX, "{hmac_proc_he_cap_ie::invalid he cap ie len[%d].}", puc_he_cap_ie[1]);
        return OAL_FAIL;
    }

    pst_mac_user = &pst_hmac_user->st_user_base_info;

    /* ֧��11ax���Ž��к����Ĵ��� */
    if (OAL_FALSE == mac_mib_get_HEOptionImplemented(pst_mac_vap))
    {
        return OAL_SUCC;
    }

    pst_mac_he_hdl = &st_mac_he_hdl;
    mac_user_get_he_hdl(pst_mac_user, pst_mac_he_hdl);

    /*���� HE MAC Capabilities Info*/
    OAL_MEMZERO(&st_he_cap_value, OAL_SIZEOF(st_he_cap_value));
    ul_ret = mac_ie_parse_he_cap(puc_he_cap_ie,&st_he_cap_value);
    if(OAL_SUCC != ul_ret)
    {
        return OAL_FAIL;
    }
    pst_mac_he_hdl->en_he_capable = OAL_TRUE;

    oal_memcopy(&pst_mac_he_hdl->st_he_cap_ie, &st_he_cap_value, OAL_SIZEOF(st_he_cap_value));

    mac_user_set_he_hdl(pst_mac_user,pst_mac_he_hdl);

    return OAL_SUCC;
}


oal_uint32  hmac_proc_he_bss_color_change_announcement_ie(mac_vap_stru *pst_mac_vap,hmac_user_stru *pst_hmac_user, oal_uint8 *puc_bss_color_ie)
{
    mac_frame_bss_color_change_annoncement_ie_stru  st_bss_color_info;
    mac_he_hdl_stru                                *pst_mac_he_hdl;

    if (OAL_FALSE == mac_mib_get_HEOptionImplemented(pst_mac_vap))
    {
        return OAL_SUCC;
    }

    OAL_MEMZERO(&st_bss_color_info, OAL_SIZEOF(mac_frame_bss_color_change_annoncement_ie_stru));
    if(OAL_SUCC != mac_ie_parse_he_bss_color_change_announcement_ie(puc_bss_color_ie, &st_bss_color_info))
    {
        return OAL_SUCC;
    }

    pst_mac_he_hdl                      = &pst_hmac_user->st_user_base_info.st_he_hdl;
    pst_mac_he_hdl->bit_bss_color       = st_bss_color_info.bit_new_bss_color;
    pst_mac_he_hdl->bit_bss_color_exist = 1;

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_proc_he_bss_color_change_announcement_ie::new_bss_color=[%d].}",
    pst_mac_he_hdl->bit_bss_color );

    return OAL_SUCC;
}


#endif


oal_void hmac_add_and_clear_repeat_op_rates(oal_uint8 *puc_ie_rates, oal_uint8 uc_ie_num_rates, hmac_rate_stru *pst_op_rates)
{
    oal_uint8                uc_ie_rates_idx;
    oal_uint8                uc_user_rates_idx;

    for(uc_ie_rates_idx = 0; uc_ie_rates_idx < uc_ie_num_rates; uc_ie_rates_idx++)
    {
        /* �жϸ������Ƿ��Ѿ���¼��op�� */
        for(uc_user_rates_idx = 0; uc_user_rates_idx < pst_op_rates->uc_rs_nrates; uc_user_rates_idx++)
        {
            if(IS_EQUAL_RATES(puc_ie_rates[uc_ie_rates_idx], pst_op_rates->auc_rs_rates[uc_user_rates_idx]))
            {
                break;
            }
        }

        /* ���ʱ��˵��ie�е�������op�е����ʶ�����ͬ�����Լ���op�����ʼ��� */
        if(uc_user_rates_idx == pst_op_rates->uc_rs_nrates)
        {
            /* �����ȳ�������ʱ�澯��������op rates�� */
            if(WLAN_USER_MAX_SUPP_RATES == pst_op_rates->uc_rs_nrates)
            {
                OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hmac_add_and_clear_repeat_op_rates::user option rates more then WLAN_USER_MAX_SUPP_RATES.}");
                break;
            }
            pst_op_rates->auc_rs_rates[pst_op_rates->uc_rs_nrates++] = puc_ie_rates[uc_ie_rates_idx];
        }
    }
}


oal_uint32 hmac_ie_proc_assoc_user_legacy_rate(oal_uint8 *puc_payload, oal_uint32 us_rx_len, hmac_user_stru *pst_hmac_user)
{
    oal_uint8               *puc_ie;
    mac_user_stru           *pst_mac_user;
    mac_vap_stru            *pst_mac_vap;
    oal_uint8                uc_num_rates       = 0;
    oal_uint8                uc_num_ex_rates    = 0;

    pst_mac_user = &(pst_hmac_user->st_user_base_info);

    pst_mac_vap = mac_res_get_mac_vap(pst_mac_user->uc_vap_id);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_ie_proc_assoc_user_legacy_rate::pst_mac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* user�Ŀ�ѡ���ʼ���ʱӦ��Ϊ�� */
    if(0 != pst_hmac_user->st_op_rates.uc_rs_nrates)
    {
        OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_ie_proc_assoc_user_legacy_rate::the number of user option rates is not 0.}");
    }

    /* 1.�����������ʼ� */
    puc_ie = mac_find_ie_etc(MAC_EID_RATES, puc_payload, (oal_int32)us_rx_len);

    if (OAL_PTR_NULL != puc_ie)
    {
        uc_num_rates = puc_ie[1];
        /* �ж�supported rates���ȣ�������Ϊ0ʱ�澯�������ȳ�������ʱ����hmac_add_and_clear_repeat_op_rates��澯��extended supported ratesͬ������ */
        if(MAC_MIN_RATES_LEN > uc_num_rates)
        {
            OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY,
                             "{hmac_ie_proc_assoc_user_legacy_rate::supported rates length less then MAC_MIN_RATES_LEN vap mode[%d] num_rates[%d]}",
                             pst_mac_vap->en_vap_mode, uc_num_rates);
        }
        else
        {
            hmac_add_and_clear_repeat_op_rates(puc_ie + MAC_IE_HDR_LEN, uc_num_rates, &(pst_hmac_user->st_op_rates));
        }
    }
    else
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY,
                         "{hmac_ie_proc_assoc_user_legacy_rate::unsupport basic rates vap mode[%d]}", pst_mac_vap->en_vap_mode);
    }

    /* 2.������չ���ʼ� */
    puc_ie = mac_find_ie_etc(MAC_EID_XRATES, puc_payload, (oal_int32)us_rx_len);

    if (OAL_PTR_NULL != puc_ie)
    {
        uc_num_ex_rates = puc_ie[1];
        /* APģʽ�£�Э�����11a��С��11Gʱ���������Զ�Я����չ���ʼ���STAģʽ�²������� */
        if ((WLAN_VAP_MODE_BSS_AP == pst_mac_vap->en_vap_mode) &&
            ((WLAN_LEGACY_11G_MODE >= pst_mac_vap->en_protocol) && (WLAN_LEGACY_11A_MODE < pst_mac_vap->en_protocol)))
        {
            OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY,
                             "{hmac_ie_proc_assoc_user_legacy_rate::invaild xrates rate vap protocol[%d] num_ex_rates[%d]}",
                              pst_mac_vap->en_protocol, uc_num_ex_rates);
        }
        else
        {
            /* ���ȸ澯����ͬ�� */
            if (MAC_MIN_XRATE_LEN > uc_num_ex_rates)
            {
                OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY,
                             "{hmac_ie_proc_assoc_user_legacy_rate::extended supported rates length less then MAC_MIN_RATES_LEN vap mode[%d] num_rates[%d]}",
                             pst_mac_vap->en_vap_mode, uc_num_ex_rates);
            }
            else
            {
                hmac_add_and_clear_repeat_op_rates(puc_ie + MAC_IE_HDR_LEN, uc_num_ex_rates, &(pst_hmac_user->st_op_rates));
            }
        }
    }
    else
    {
        if(WLAN_BAND_2G == pst_mac_vap->st_channel.en_band)
        {
            OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY,
                             "{hmac_ie_proc_assoc_user_legacy_rate::unsupport xrates vap mode[%d]}", pst_mac_vap->en_vap_mode);
        }
    }

    if (0 == pst_hmac_user->st_op_rates.uc_rs_nrates)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_ie_proc_assoc_user_legacy_rate::rate is 0.}");
        return OAL_FAIL;
    }

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_HS20

oal_uint32  hmac_interworking_check(hmac_vap_stru *pst_hmac_vap,  oal_uint8 *puc_param)
{
    oal_uint8           *puc_extend_cap_ie;
    mac_bss_dscr_stru   *pst_bss_dscr;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_vap) || OAL_UNLIKELY(OAL_PTR_NULL == puc_param))
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hmac_interworking_check:: check failed, null ptr!}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_bss_dscr = (mac_bss_dscr_stru *)puc_param;
    if (pst_bss_dscr->ul_mgmt_len < (MAC_80211_FRAME_LEN + MAC_SSID_OFFSET))
    {
        OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_CFG, "{hmac_interworking_check:: mgmt_len(%d) < (80211_FRAME_LEN+SSID_OFFSET).}", pst_bss_dscr->ul_mgmt_len);
        return OAL_FAIL;
    }

    /* ����interworking ie */
    /*lint -e416*/
    puc_extend_cap_ie = mac_find_ie_etc(MAC_EID_EXT_CAPS, pst_bss_dscr->auc_mgmt_buff + MAC_80211_FRAME_LEN + MAC_SSID_OFFSET, (oal_int32)(pst_bss_dscr->ul_mgmt_len - MAC_80211_FRAME_LEN - MAC_SSID_OFFSET));
    /*lint +e416*/

    if (OAL_PTR_NULL == puc_extend_cap_ie)
    {
        OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_CFG, "{hmac_interworking_check:: puc_extend_cap_ie is NULL, the ul_mgmt_len is %d.}", pst_bss_dscr->ul_mgmt_len);
        return OAL_FAIL;
    }

    /*  δ��⵽interworking����λ������fail */
    if (puc_extend_cap_ie[1] < 4 || !(puc_extend_cap_ie[5] & 0x80))
    {
        pst_hmac_vap->st_vap_base_info.st_cap_flag.bit_is_interworking = OAL_FALSE;
        return OAL_FAIL;
    }

    pst_hmac_vap->st_vap_base_info.st_cap_flag.bit_is_interworking = OAL_TRUE;
    return OAL_SUCC;
}
#endif //_PRE_WLAN_FEATURE_HS20

/*lint -e19*/
oal_module_symbol(hmac_config_send_deauth_etc);
oal_module_symbol(hmac_wpas_mgmt_tx_etc);
/*lint +e19*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

