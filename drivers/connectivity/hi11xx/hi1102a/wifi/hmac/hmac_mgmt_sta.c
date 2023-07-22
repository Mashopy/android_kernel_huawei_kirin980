


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "wlan_spec.h"
#include "wlan_mib.h"

#include "mac_frame.h"
#include "mac_ie.h"
#include "mac_regdomain.h"
#include "mac_user.h"
#include "mac_vap.h"

#include "mac_device.h"
#include "hmac_device.h"
#include "hmac_user.h"
#include "hmac_mgmt_sta.h"
#include "hmac_fsm.h"
#include "hmac_rx_data.h"
#include "hmac_chan_mgmt.h"
#include "hmac_mgmt_bss_comm.h"
#include "hmac_encap_frame_sta.h"
#include "hmac_sme_sta.h"
#include "hmac_scan.h"

#include "hmac_tx_amsdu.h"

#include "hmac_11i.h"

#include "hmac_protection.h"

#include "hmac_config.h"
#include "hmac_ext_if.h"
#include "hmac_p2p.h"
#include "hmac_edca_opt.h"
#include "hmac_mgmt_ap.h"

#ifdef _PRE_WLAN_CHIP_TEST
#include "dmac_test_main.h"
#endif
#include "hmac_blockack.h"
#include "frw_ext_if.h"

#ifdef _PRE_WLAN_FEATURE_ROAM
#include "hmac_roam_main.h"
#endif //_PRE_WLAN_FEATURE_ROAM

#ifdef _PRE_WLAN_FEATURE_WAPI
#include "hmac_wapi.h"
#endif

#ifdef _PRE_PLAT_FEATURE_CUSTOMIZE
#include "hisi_customize_wifi.h"
#endif
#ifdef _PRE_WLAN_FEATURE_PROXYSTA
#include "hmac_proxysta.h"
#endif
#ifdef _PRE_WLAN_FEATURE_WMMAC
#include "hmac_wmmac.h"
#endif

#ifdef _PRE_WLAN_FEATURE_11V_ENABLE
#include "hmac_11v.h"
#endif
#ifdef _PRE_WLAN_FEATURE_SNIFFER
#include <hwnet/ipv4/sysctl_sniffer.h>
#endif
#include "plat_pm_wlan.h"

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_MGMT_STA_C
#define MAC_ADDR(_puc_mac)   ((oal_uint32)(((oal_uint32)_puc_mac[2] << 24) |\
                                              ((oal_uint32)_puc_mac[3] << 16) |\
                                              ((oal_uint32)_puc_mac[4] << 8) |\
                                              ((oal_uint32)_puc_mac[5])))

/* _puc_ie��ָ��ht cap�ֶε�ָ�룬��ƫ��5,6,7,8�ֽڷֱ��ӦMCS�����ռ�����֧�ֵ����� */
#define IS_INVALID_HT_RATE_HP(_puc_ie)  ((0x02 == _puc_ie[5]) && (0x00 == _puc_ie[6]) && (0x05 == _puc_ie[7]) && (0x00 == _puc_ie[8]))

/*****************************************************************************
  2 ��̬��������
*****************************************************************************/
//OAL_STATIC oal_void hmac_handle_tbtt_chan_mgmt_sta(hmac_vap_stru *pst_hmac_vap);
#ifdef _PRE_WLAN_FEATURE_BTCOEX
oal_void hmac_btcoex_check_rx_same_baw_start_from_addba_req(hmac_vap_stru *pst_hmac_vap,
                                                        hmac_user_stru *pst_hmac_user,
                                                        mac_ieee80211_frame_stru *pst_frame_hdr,
                                                        oal_uint8 *puc_action);
#endif


/*****************************************************************************
  3 ȫ�ֱ�������
*****************************************************************************/
oal_bool_enum_uint8 g_ht_mcs_set_check = OAL_TRUE;

/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/

OAL_STATIC oal_uint32  hmac_mgmt_timeout_sta(oal_void *p_arg)
{
    hmac_vap_stru *pst_hmac_vap;
    hmac_mgmt_timeout_param_stru    *pst_timeout_param;

    pst_timeout_param = (hmac_mgmt_timeout_param_stru *)p_arg;
    if (OAL_PTR_NULL == pst_timeout_param)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap      = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_timeout_param->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    return hmac_fsm_call_func_sta(pst_hmac_vap, HMAC_FSM_INPUT_TIMER0_OUT, pst_timeout_param);
}
#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST

oal_void  hmac_update_join_req_params_2040(mac_vap_stru *pst_mac_vap, mac_bss_dscr_stru *pst_bss_dscr)
{
    if (((OAL_FALSE == mac_mib_get_HighThroughputOptionImplemented(pst_mac_vap)) && (OAL_FALSE == mac_mib_get_VHTOptionImplemented(pst_mac_vap))) ||
        ((OAL_FALSE == pst_bss_dscr->en_ht_capable) && (OAL_FALSE == pst_bss_dscr->en_vht_capable)))
    {
        pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
        return;
    }

    /* ʹ��40MHz */
    /* (1) �û�����"40MHz����"����(��STA�� dot11FortyMHzOperationImplementedΪtrue) */
    /* (2) AP��40MHz���� */
    if (OAL_TRUE == mac_mib_get_FortyMHzOperationImplemented(pst_mac_vap))
    {
        switch (pst_bss_dscr->en_channel_bandwidth)
        {
            case WLAN_BAND_WIDTH_40PLUS:
            case WLAN_BAND_WIDTH_80PLUSPLUS:
            case WLAN_BAND_WIDTH_80PLUSMINUS:
                pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_40PLUS;
                break;

            case WLAN_BAND_WIDTH_40MINUS:
            case WLAN_BAND_WIDTH_80MINUSPLUS:
            case WLAN_BAND_WIDTH_80MINUSMINUS:
                pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_40MINUS;
                break;

            default:
                pst_mac_vap->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
                break;
        }
    }

    /* ����STA�������APһ�� */
    /* (1) STA AP��֧��11AC */
    /* (2) STA֧��40M����(FortyMHzOperationImplementedΪTRUE)��
           ���ƻ���ֹ2GHT40ʱ��2G��FortyMHzOperationImplemented=FALSE�������´��� */
    /* (3) STA֧��80M����(��STA�� dot11VHTChannelWidthOptionImplementedΪ0) */
    if ((OAL_TRUE == mac_mib_get_VHTOptionImplemented(pst_mac_vap)) &&
        (OAL_TRUE == pst_bss_dscr->en_vht_capable))
    {
        if ((OAL_TRUE == mac_mib_get_FortyMHzOperationImplemented(pst_mac_vap))
            && (WLAN_MIB_VHT_SUPP_WIDTH_80 == mac_mib_get_VHTChannelWidthOptionImplemented(pst_mac_vap)))
        {
#if (_PRE_WLAN_CHIP_ASIC == _PRE_WLAN_CHIP_VERSION)
            pst_mac_vap->st_channel.en_bandwidth = pst_bss_dscr->en_channel_bandwidth;
#else
            OAM_WARNING_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_2040, "{hmac_update_join_req_params_2040::fpga is not support 80M.}");
#endif
        }
    }

    /* ���AP��STAͬʱ֧��20/40����������ܣ���ʹ��STA��Ƶ�׹�������*/
    if ((OAL_TRUE == mac_mib_get_2040BSSCoexistenceManagementSupport(pst_mac_vap)) &&
        (1 == pst_bss_dscr->uc_coex_mgmt_supp))
    {
        mac_mib_set_SpectrumManagementImplemented(pst_mac_vap, OAL_TRUE);
    }
}

#endif

OAL_STATIC oal_void hmac_update_join_req_params_prot_sta(hmac_vap_stru * pst_hmac_vap, hmac_join_req_stru * pst_join_req)
{
    if (WLAN_MIB_DESIRED_BSSTYPE_INFRA  == pst_hmac_vap->st_vap_base_info.pst_mib_info->st_wlan_mib_sta_config.en_dot11DesiredBSSType)
    {
        pst_hmac_vap->uc_wmm_cap   = pst_join_req->st_bss_dscr.uc_wmm_cap;
        mac_vap_set_uapsd_cap(&pst_hmac_vap->st_vap_base_info, pst_join_req->st_bss_dscr.uc_uapsd_cap);
    }

#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    hmac_update_join_req_params_2040(&(pst_hmac_vap->st_vap_base_info), &(pst_join_req->st_bss_dscr));
#endif
}


oal_bool_enum_uint8  hmac_is_rate_support(oal_uint8 *puc_rates, oal_uint8 uc_rate_num, oal_uint8 uc_rate)
{
    oal_bool_enum_uint8  en_rate_is_supp = OAL_FALSE;
    oal_uint8            uc_loop;

    if (OAL_PTR_NULL == puc_rates)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_is_rate_support::puc_rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    for (uc_loop = 0; uc_loop < uc_rate_num; uc_loop++)
    {
        if ((puc_rates[uc_loop] & 0x7F) == uc_rate)
        {
            en_rate_is_supp = OAL_TRUE;
            break;
        }
    }

    return en_rate_is_supp;
}


oal_bool_enum_uint8  hmac_is_support_11grate(oal_uint8 *puc_rates, oal_uint8 uc_rate_num)
{
    if (OAL_PTR_NULL == puc_rates)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_is_rate_support::puc_rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x0C))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x12))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x18))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x24))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x30))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x48))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x60))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x6C)))
    {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}



oal_bool_enum_uint8  hmac_is_support_11brate(oal_uint8 *puc_rates, oal_uint8 uc_rate_num)
{
    if (OAL_PTR_NULL == puc_rates)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_is_support_11brate::puc_rates null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    if ((OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x02))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x04))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x0B))
        || (OAL_TRUE == hmac_is_rate_support(puc_rates, uc_rate_num, 0x16)))
    {
        return OAL_TRUE;
    }

    return OAL_FALSE;
}
#if 0

oal_bool_enum_uint8  hmac_bss_is_rate_support(mac_bss_dscr_stru* pst_bss_dscr, oal_uint8 uc_rate)
{
    oal_bool_enum_uint8  en_rate_is_supp = OAL_FALSE;
    oal_uint8            uc_loop;

    if (OAL_PTR_NULL == pst_bss_dscr)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_bss_is_rate_support::pst_bss_dscr null}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    for (uc_loop = 0; uc_loop < pst_bss_dscr->uc_num_supp_rates; uc_loop++)
    {
        if ((pst_bss_dscr->auc_supp_rates[uc_loop] & 0x7F) == uc_rate)
        {
            en_rate_is_supp = OAL_TRUE;
            break;
        }
    }

    return en_rate_is_supp;
}
#endif

oal_uint32 hmac_sta_get_user_protocol(mac_bss_dscr_stru *pst_bss_dscr, wlan_protocol_enum_uint8  *pen_protocol_mode)
{
    /* ��α��� */
    if (OAL_PTR_NULL == pst_bss_dscr || OAL_PTR_NULL == pen_protocol_mode)
    {
        OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{hmac_sta_get_user_protocol::param null,%d %d.}", pst_bss_dscr, pen_protocol_mode);
        return OAL_ERR_CODE_PTR_NULL;
    }

    if (OAL_TRUE == pst_bss_dscr->en_vht_capable)
    {
        *pen_protocol_mode = WLAN_VHT_MODE;
    }
    else if (OAL_TRUE == pst_bss_dscr->en_ht_capable)
    {
        *pen_protocol_mode = WLAN_HT_MODE;
    }
    else
    {
        if (WLAN_BAND_5G == pst_bss_dscr->st_channel.en_band)            /* �ж��Ƿ���5G */
        {
            *pen_protocol_mode = WLAN_LEGACY_11A_MODE;
        }
        else
        {
            if (OAL_TRUE == hmac_is_support_11grate(pst_bss_dscr->auc_supp_rates, pst_bss_dscr->uc_num_supp_rates))
            {
                *pen_protocol_mode = WLAN_LEGACY_11G_MODE;
                if (OAL_TRUE == hmac_is_support_11brate(pst_bss_dscr->auc_supp_rates, pst_bss_dscr->uc_num_supp_rates))
                {
                    *pen_protocol_mode = WLAN_MIXED_ONE_11G_MODE;
                }
            }
            else if (OAL_TRUE == hmac_is_support_11brate(pst_bss_dscr->auc_supp_rates, pst_bss_dscr->uc_num_supp_rates))
            {
                *pen_protocol_mode = WLAN_LEGACY_11B_MODE;
            }
            else
            {
                OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hmac_sta_get_user_protocol::get user protocol failed.}");
            }
        }
    }

    return OAL_SUCC;
}


oal_uint32  hmac_sta_wait_join(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_join_req_stru                  *pst_join_req;
    hmac_join_rsp_stru                   st_join_rsp;
    oal_uint32                           ul_ret;

    if (OAL_PTR_NULL == pst_hmac_sta || OAL_PTR_NULL == pst_msg)
    {
        OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{hmac_sta_wait_join::param null,%d %d.}", pst_hmac_sta, pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* 1102 P2PSTA���� todo ���²���ʧ�ܵĻ���Ҫ���ض����Ǽ����·�Join���� */
    ul_ret = hmac_p2p_check_can_enter_state(&(pst_hmac_sta->st_vap_base_info), HMAC_FSM_INPUT_ASOC_REQ);
    if (ul_ret != OAL_SUCC)
    {
        /* ���ܽ������״̬�������豸æ */
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                        "{hmac_sta_wait_join fail,device busy: ul_ret=%d}\r\n", ul_ret);
        return OAL_ERR_CODE_CONFIG_BUSY;
    }

    pst_join_req = (hmac_join_req_stru *)pst_msg;

    /* ��Ҫ����proxystaģʽ�� MAIN STA�������ʱDOWN������UP״̬��AP ������proxystaʱ����ֱ�ӷ��� */
#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    hmac_psta_proc_wait_join(pst_hmac_sta, pst_join_req);
#endif
    /* ����JOIN REG params ��MIB��MAC�Ĵ��� */
    ul_ret = hmac_sta_update_join_req_params(pst_hmac_sta, pst_join_req);
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_sta_wait_join::get hmac_sta_update_join_req_params fail[%d]!}", ul_ret);
        return ul_ret;
    }
    OAM_INFO_LOG3(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                  "{hmac_sta_wait_join::Join AP channel=%d Beacon Period=%d DTIM Period=%d.}",
                  pst_join_req->st_bss_dscr.st_channel.uc_chan_number,
                  pst_join_req->st_bss_dscr.us_beacon_period,
                  pst_join_req->st_bss_dscr.uc_dtim_period);

    /* ��proxy staģʽʱ����Ҫ��dtim�������õ�dmac*/
#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    if (mac_vap_is_vsta(&pst_hmac_sta->st_vap_base_info))
    {
        //��proxysta��ǰ���£�����proxystaģʽ�²����������ǿ���proxystaģʽ�²��账��
    }
    else
#endif
    {
        dmac_ctx_set_dtim_tsf_reg_stru      *pst_set_dtim_tsf_reg_params;
        frw_event_mem_stru                  *pst_event_mem;
        frw_event_stru                      *pst_event;

        /* ���¼���DMAC, �����¼��ڴ� */
        pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru));
        if (OAL_PTR_NULL == pst_event_mem)
        {
            OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_join::event_mem alloc null, size[%d].}", OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru));
            return OAL_ERR_CODE_PTR_NULL;
        }

        /* ��д�¼� */
        pst_event = (frw_event_stru *)pst_event_mem->puc_data;

        FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                           FRW_EVENT_TYPE_WLAN_CTX,
                           DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG,
                           OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru),
                           FRW_EVENT_PIPELINE_STAGE_1,
                           pst_hmac_sta->st_vap_base_info.uc_chip_id,
                           pst_hmac_sta->st_vap_base_info.uc_device_id,
                           pst_hmac_sta->st_vap_base_info.uc_vap_id);

        pst_set_dtim_tsf_reg_params = (dmac_ctx_set_dtim_tsf_reg_stru *)pst_event->auc_event_data;

        /* ��Ap bssid��tsf REG ����ֵ�������¼�payload�� */
        pst_set_dtim_tsf_reg_params->ul_dtim_cnt      = pst_join_req->st_bss_dscr.uc_dtim_cnt;
        pst_set_dtim_tsf_reg_params->ul_dtim_period   = pst_join_req->st_bss_dscr.uc_dtim_period;
        pst_set_dtim_tsf_reg_params->us_tsf_bit0      = BIT0;
        oal_memcopy(pst_set_dtim_tsf_reg_params->auc_bssid, pst_hmac_sta->st_vap_base_info.auc_bssid, WLAN_MAC_ADDR_LEN);

        /* �ַ��¼� */
        frw_event_dispatch_event(pst_event_mem);
        FRW_EVENT_FREE(pst_event_mem);
    }

    if(0 == (g_uc_hitalk_status & NBFH_ON_MASK))
    {
        st_join_rsp.en_result_code = HMAC_MGMT_SUCCESS;
    }

    /* �л�STA״̬��JOIN_COMP */
    hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_JOIN_COMP);

    if(0 == (g_uc_hitalk_status & NBFH_ON_MASK))
    {
        /* ����JOIN�ɹ���Ϣ��SME */
        hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP,(oal_uint8 *)&st_join_rsp);
    }
    else
    {
        OAM_WARNING_LOG0(0,0, "hmac_sta_wait_join::change to MAC_VAP_STATE_STA_JOIN_COMP");
    }

    OAM_INFO_LOG3(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                   "{hmac_sta_wait_join::Join AP[%08x] HT=%d VHT=%d SUCC.}",
                   MAC_ADDR(pst_join_req->st_bss_dscr.auc_bssid),
                   pst_join_req->st_bss_dscr.en_ht_capable,
                   pst_join_req->st_bss_dscr.en_vht_capable);

    OAM_WARNING_LOG4(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                   "{hmac_sta_wait_join::Join AP channel=%d idx=%d bandwidth=%d Beacon Period=%d SUCC.}",
                   pst_join_req->st_bss_dscr.st_channel.uc_chan_number,
                   pst_join_req->st_bss_dscr.st_channel.uc_idx,
                   pst_hmac_sta->st_vap_base_info.st_channel.en_bandwidth,
                   pst_join_req->st_bss_dscr.us_beacon_period);

    return OAL_SUCC;
}


oal_uint32  hmac_sta_wait_join_rx(hmac_vap_stru *pst_hmac_sta, oal_void *p_param)
{
    dmac_wlan_crx_event_stru            *pst_mgmt_rx_event;
    dmac_rx_ctl_stru                    *pst_rx_ctrl;
    mac_rx_ctl_stru                     *pst_rx_info;
    oal_uint8                           *puc_mac_hdr;
    frw_event_mem_stru                  *pst_event_mem;
    frw_event_stru                      *pst_event;
    dmac_ctx_set_dtim_tsf_reg_stru       st_set_dtim_tsf_reg_params = {0};
    oal_uint8                           *puc_tim_elm;
    oal_uint16                           us_rx_len;

    oal_uint8                            auc_bssid[6] = {0};

     if ((OAL_PTR_NULL == pst_hmac_sta) || (OAL_PTR_NULL == p_param))
     {
         OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{hmac_sta_wait_join_rx::param null,%d %d.}", pst_hmac_sta, p_param);
         return OAL_ERR_CODE_PTR_NULL;
     }

     pst_mgmt_rx_event   = (dmac_wlan_crx_event_stru *)p_param;
     pst_rx_ctrl         = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_mgmt_rx_event->pst_netbuf);
     pst_rx_info         = (mac_rx_ctl_stru *)(&(pst_rx_ctrl->st_rx_info));
     puc_mac_hdr         = (oal_uint8 *)(pst_rx_info->pul_mac_hdr_start_addr);
     us_rx_len           =  pst_rx_ctrl->st_rx_info.us_frame_len;  /* ��Ϣ�ܳ���,������FCS */

     /* ��WAIT_JOIN״̬�£��������յ���beacon֡ */
    switch (mac_get_frame_sub_type(puc_mac_hdr))
    {
        case WLAN_FC0_SUBTYPE_BEACON:
        {
            /* ��ȡBeacon֡�е�mac��ַ����AP��mac��ַ */
            mac_get_bssid(puc_mac_hdr, auc_bssid);

            /* ���STA�����AP mac��ַ�����beacon֡��mac��ַƥ�䣬�����beacon֡�е�DTIM countֵ��STA����mib���� */
            if (0 == oal_memcmp(auc_bssid, pst_hmac_sta->st_vap_base_info.auc_bssid, WLAN_MAC_ADDR_LEN))
            {
                //puc_tim_elm = mac_get_tim_ie(puc_mac_hdr, us_rx_len, MAC_TAG_PARAM_OFFSET);
                puc_tim_elm = mac_find_ie(MAC_EID_TIM, puc_mac_hdr + MAC_TAG_PARAM_OFFSET, us_rx_len - MAC_TAG_PARAM_OFFSET);

                /* ��tim IE����ȡ DTIM countֵ,д�뵽MAC H/W REG�� */
                if ((OAL_PTR_NULL != puc_tim_elm) && (puc_tim_elm[1] >= MAC_MIN_TIM_LEN))
                {
                    pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_sta_config.ul_dot11DTIMPeriod = puc_tim_elm[3];

                    /* ��dtim_cnt��dtim_period�������¼�payload�� */
                    st_set_dtim_tsf_reg_params.ul_dtim_cnt      = puc_tim_elm[2];
                    st_set_dtim_tsf_reg_params.ul_dtim_period   = puc_tim_elm[3];
                }
                else
                {
                    OAM_WARNING_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN,
                                     "{hmac_sta_wait_join::Do not find Tim ie.}");
                }

                /* ��Ap bssid��tsf REG ����ֵ�������¼�payload�� */
                oal_memcopy (st_set_dtim_tsf_reg_params.auc_bssid, auc_bssid, WLAN_MAC_ADDR_LEN);
                st_set_dtim_tsf_reg_params.us_tsf_bit0 = BIT0;

                 /* ���¼���DMAC, �����¼��ڴ� */
                pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru));
                if (OAL_PTR_NULL == pst_event_mem)
                {
                    OAM_ERROR_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_sta_wait_join::pst_event_mem null.}");
                    return OAL_ERR_CODE_PTR_NULL;
                }

                /* ��д�¼� */
                pst_event = (frw_event_stru *)pst_event_mem->puc_data;

                FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                                   FRW_EVENT_TYPE_WLAN_CTX,
                                   DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG,
                                   OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru),
                                   FRW_EVENT_PIPELINE_STAGE_1,
                                   pst_hmac_sta->st_vap_base_info.uc_chip_id,
                                   pst_hmac_sta->st_vap_base_info.uc_device_id,
                                   pst_hmac_sta->st_vap_base_info.uc_vap_id);

                /* �������� */
                oal_memcopy(frw_get_event_payload(pst_event_mem), (oal_uint8*)&st_set_dtim_tsf_reg_params, OAL_SIZEOF(dmac_ctx_set_dtim_tsf_reg_stru));

                /* �ַ��¼� */
                frw_event_dispatch_event(pst_event_mem);
                FRW_EVENT_FREE(pst_event_mem);
            }
        }
        break;

        case WLAN_FC0_SUBTYPE_ACTION:
        {
            /* do nothing  */
        }
        break;

        default:
        {
            /* do nothing */
        }
        break;
    }

   return OAL_SUCC;
}


oal_uint32  hmac_sta_wait_join_timeout(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{

    hmac_join_rsp_stru      st_join_rsp        = {0};

    if (OAL_PTR_NULL == pst_hmac_sta || OAL_PTR_NULL == pst_msg)
    {
        OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{hmac_sta_wait_join_timeout::param null,%d %d.}", pst_hmac_sta, pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAM_ERROR_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_sta_wait_join_timeout::join timeout.}");
    /* ����timeout����������ʾjoinû�гɹ�����join�Ľ������Ϊtimeout�ϱ���sme */
    st_join_rsp.en_result_code = HMAC_MGMT_TIMEOUT;

    /* ��hmac״̬���л�ΪMAC_VAP_STATE_STA_FAKE_UP */
    hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_FAKE_UP);

    /* ���ͳ�ʱ��Ϣ��SME */
    hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP, (oal_uint8 *)&st_join_rsp);

    return OAL_SUCC;
}


oal_uint32  hmac_sta_wait_join_misc(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_join_rsp_stru      st_join_rsp;
    hmac_misc_input_stru    *st_misc_input = (hmac_misc_input_stru *)pst_msg;

    if((OAL_PTR_NULL == pst_hmac_sta) ||(OAL_PTR_NULL == pst_msg))
    {
        OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{hmac_sta_wait_join_misc::param null,%d %d.}", pst_hmac_sta, pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAM_INFO_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_sta_wait_join_misc::enter func.}");
    switch (st_misc_input->en_type)
    {
        /* ����TBTT�ж�  */
        case HMAC_MISC_TBTT:
        {
           /* ���յ�TBTT�жϣ���ζ��JOIN�ɹ��ˣ�����ȡ��JOIN��ʼʱ���õĶ�ʱ��,����Ϣ֪ͨSME  */
           FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&pst_hmac_sta->st_mgmt_timer);

           st_join_rsp.en_result_code = HMAC_MGMT_SUCCESS;

            OAM_INFO_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_sta_wait_join_misc::join succ.}");
            /* �л�STA״̬��JOIN_COMP */
            hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_JOIN_COMP);

            /* ����JOIN�ɹ���Ϣ��SME */
            hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP,(oal_uint8 *)&st_join_rsp);
        }
        break;

        default:
        {
            /* Do Nothing */
        }
        break;
    }
    return OAL_SUCC;
}




oal_uint32  hmac_sta_wait_auth(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_auth_req_stru  *pst_auth_req;
    oal_netbuf_stru     *pst_auth_frame;
    oal_uint16           us_auth_len;
    mac_tx_ctl_stru     *pst_tx_ctl;
    hmac_user_stru      *pst_hmac_user_ap;
    oal_uint32           ul_ret;

    if (OAL_PTR_NULL == pst_hmac_sta || OAL_PTR_NULL == pst_msg)
    {
        OAM_ERROR_LOG2(0, OAM_SF_AUTH, "{hmac_sta_wait_auth::param null,%d %d.}", pst_hmac_sta, pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_auth_req = (hmac_auth_req_stru *)pst_msg;
    /* ������֤֡�ռ� */
    pst_auth_frame = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);

    if (OAL_PTR_NULL == pst_auth_frame)
    {
        OAM_ERROR_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::puc_auth_frame null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAL_MEM_NETBUF_TRACE(pst_auth_frame, OAL_TRUE);

    OAL_MEMZERO(oal_netbuf_cb(pst_auth_frame), OAL_NETBUF_CB_SIZE());

    OAL_MEMZERO((oal_uint8 *)oal_netbuf_header(pst_auth_frame), MAC_80211_FRAME_LEN);

    /* ��seq = 1 ����֤����֡ */
    us_auth_len = hmac_mgmt_encap_auth_req(pst_hmac_sta, (oal_uint8 *)(OAL_NETBUF_HEADER(pst_auth_frame)));

    if (us_auth_len == 0)
    {
        /* ��֡ʧ�� */
        OAM_WARNING_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::hmac_mgmt_encap_auth_req failed.}");

        oal_netbuf_free(pst_auth_frame);
        hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_FAKE_UP);
        /* TBD: ��ϵͳ����reset MAC ֮��� */
    }
    else
    {
        oal_netbuf_put(pst_auth_frame, us_auth_len);
        pst_hmac_user_ap = mac_res_get_hmac_user((oal_uint16)pst_hmac_sta->st_vap_base_info.uc_assoc_vap_id);
        if (OAL_PTR_NULL == pst_hmac_user_ap)
        {
            oal_netbuf_free(pst_auth_frame);
            OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::pst_hmac_user_ap[%d] null.}", pst_hmac_sta->st_vap_base_info.uc_assoc_vap_id);
            return OAL_ERR_CODE_PTR_NULL;
        }

        /* Ϊ��д����������׼������ */
        pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_auth_frame);                              /* ��ȡcb�ṹ�� */
        pst_tx_ctl->us_mpdu_len   = us_auth_len;                                      /* dmac������Ҫ��mpdu���� */
        pst_tx_ctl->us_tx_user_idx              = pst_hmac_user_ap->st_user_base_info.us_assoc_id;  /* ���������Ҫ��ȡuser�ṹ�� */

        /*�����WEP����Ҫ��ap��mac��ַд��lut*/
        ul_ret = hmac_init_security(&(pst_hmac_sta->st_vap_base_info), pst_hmac_user_ap->st_user_base_info.auc_user_mac_addr);
        if (OAL_SUCC != ul_ret)
        {
            OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                           "{hmac_sta_wait_auth::hmac_init_security failed[%d].}", ul_ret);
        }

        /* ���¼���dmac����֡���� */
        ul_ret = hmac_tx_mgmt_send_event(&pst_hmac_sta->st_vap_base_info, pst_auth_frame, us_auth_len);
        if (OAL_SUCC != ul_ret)
        {
            oal_netbuf_free(pst_auth_frame);
            OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::hmac_tx_mgmt_send_event failed[%d].}", ul_ret);
            return ul_ret;
        }

        /* ����״̬ */
        hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2);

        /* ������֤��ʱ��ʱ�� */
        pst_hmac_sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_AUTH_SEQ2;
        pst_hmac_sta->st_mgmt_timetout_param.us_user_index = pst_hmac_user_ap->st_user_base_info.us_assoc_id;
        pst_hmac_sta->st_mgmt_timetout_param.uc_vap_id = pst_hmac_sta->st_vap_base_info.uc_vap_id;
        FRW_TIMER_CREATE_TIMER(&pst_hmac_sta->st_mgmt_timer,
                               hmac_mgmt_timeout_sta,
                               pst_auth_req->us_timeout,
                               &pst_hmac_sta->st_mgmt_timetout_param,
                               OAL_FALSE,
                               OAM_MODULE_ID_HMAC,
                               pst_hmac_sta->st_vap_base_info.ul_core_id);

    }

    return OAL_SUCC;
}


oal_uint32  hmac_sta_wait_beacon_before_auth(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    hmac_join_rsp_stru                   st_join_rsp;
    dmac_wlan_crx_event_stru            *pst_mgmt_rx_event;
    dmac_rx_ctl_stru                    *pst_rx_ctrl;
    mac_rx_ctl_stru                     *pst_rx_info;
    oal_uint8                           *puc_mac_hdr;

    oal_uint8                            auc_bssid[6] = {0};

    if ((OAL_PTR_NULL == pst_hmac_sta) || (OAL_PTR_NULL == pst_msg))
    {
        OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{hmac_sta_wait_beacon_before_auth::param null,%d %d.}", pst_hmac_sta, pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mgmt_rx_event   = (dmac_wlan_crx_event_stru *)pst_msg;
    pst_rx_ctrl         = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_mgmt_rx_event->pst_netbuf);
    pst_rx_info         = (mac_rx_ctl_stru *)(&(pst_rx_ctrl->st_rx_info));
    puc_mac_hdr         = (oal_uint8 *)pst_rx_info->pul_mac_hdr_start_addr;

    /* ��JOIN COMP״̬�£��������յ���beacon֡ */
    switch (mac_get_frame_type_and_subtype(puc_mac_hdr))
    {
       case WLAN_FC0_SUBTYPE_BEACON|WLAN_FC0_TYPE_MGT:
       {
           /* ��ȡBeacon֡�е�mac��ַ����AP��mac��ַ */
           mac_get_bssid(puc_mac_hdr, auc_bssid);

           /* ���STA�����AP mac��ַ�����beacon֡��mac��ַƥ�䣬�����beacon֡�е�DTIM countֵ��STA����mib���� */
           if (0 == oal_memcmp(auc_bssid, pst_hmac_sta->st_vap_base_info.auc_bssid, WLAN_MAC_ADDR_LEN))
           {
                /* ���յ�TBTT�жϣ���ζ��խ����������Ƶ��ȡ�����õĶ�ʱ��,����Ϣ֪ͨSME  */
                FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&pst_hmac_sta->st_mgmt_timer);


                st_join_rsp.en_result_code = HMAC_MGMT_SUCCESS;
                /* �л�STA״̬��JOIN_COMP */
                hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_STA_NBFH_COMP);

                OAM_WARNING_LOG0(0,0, "hmac_sta_wait_beacon_before_auth called ");

                /* ����JOIN�ɹ���Ϣ��SME */
                hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_JOIN_RSP,(oal_uint8 *)&st_join_rsp);
            }
        }
    }

    return OAL_SUCC;
}


oal_uint32  hmac_sta_wait_auth_seq2_rx(hmac_vap_stru *pst_sta, oal_void *pst_msg)
{
    dmac_wlan_crx_event_stru    *pst_crx_event;
    hmac_rx_ctl_stru            *pst_rx_ctrl;                    /* ÿһ��MPDU�Ŀ�����Ϣ */
    oal_uint8                   *puc_mac_hdr;
    oal_uint16                   us_auth_frame_len = 0;
    oal_uint16                   us_auth_alg;
    oal_netbuf_stru             *pst_auth_frame    = 0;
    hmac_user_stru              *pst_hmac_user_ap;
    hmac_auth_rsp_stru           st_auth_rsp       = {{0},};
    mac_tx_ctl_stru             *pst_tx_ctl;
    oal_uint32                   ul_ret;

    if (OAL_PTR_NULL == pst_sta || OAL_PTR_NULL == pst_msg)
    {
        OAM_ERROR_LOG2(0, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq2_rx::param null,%d %d.}", pst_sta, pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }


    pst_crx_event  = (dmac_wlan_crx_event_stru *)pst_msg;
    pst_rx_ctrl    = (hmac_rx_ctl_stru *)oal_netbuf_cb(pst_crx_event->pst_netbuf);                    /* ÿһ��MPDU�Ŀ�����Ϣ */
    puc_mac_hdr    = (oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if(OAL_TRUE == wlan_pm_wkup_src_debug_get())
    {
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{wifi_wake_src:hmac_sta_wait_auth_seq2_rx_etc::wakeup mgmt type[0x%x]}",mac_get_frame_type_and_subtype(puc_mac_hdr));
    }
#endif
#endif

    if ((WLAN_FC0_SUBTYPE_AUTH) != mac_get_frame_sub_type(puc_mac_hdr) ||
        WLAN_AUTH_TRASACTION_NUM_TWO != mac_get_auth_seq_num(puc_mac_hdr))
    {
        return OAL_SUCC;
    }

#ifdef _PRE_WLAN_FEATURE_SNIFFER
    proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_ctrl->st_rx_info.us_frame_len, 0);
#endif
    /* AUTH alg CHECK*/
    us_auth_alg = mac_get_auth_alg(puc_mac_hdr);
    if ((pst_sta->en_auth_mode != us_auth_alg) &&
        (WLAN_WITP_AUTH_AUTOMATIC != pst_sta->en_auth_mode))
    {
        OAM_WARNING_LOG2(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq2_rx::rcv unexpected auth alg[%d/%d].}", us_auth_alg, pst_sta->en_auth_mode);
    }
    st_auth_rsp.us_status_code = mac_get_auth_status(puc_mac_hdr);
    if (MAC_SUCCESSFUL_STATUSCODE != st_auth_rsp.us_status_code)
    {
        FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&pst_sta->st_mgmt_timer);

        /* �ϱ���SME��֤��� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq2_rx::AP refuse STA auth reason[%d]!}", st_auth_rsp.us_status_code);
        if (MAC_AP_FULL != st_auth_rsp.us_status_code)
        {
            CHR_EXCEPTION(CHR_WIFI_DRV(CHR_WIFI_DRV_EVENT_CONNECT,CHR_WIFI_DRV_ERROR_AUTH_REJECTED));
        }
        return OAL_SUCC;
    }

    /* auth response status_code �ɹ����� */
    if (WLAN_WITP_AUTH_OPEN_SYSTEM == us_auth_alg)
    {
        FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&pst_sta->st_mgmt_timer);

        /* ��״̬����ΪAUTH_COMP */
        hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);
        st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* �ϱ���SME��֤�ɹ� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);
        return OAL_SUCC;
    }
    else if (WLAN_WITP_AUTH_SHARED_KEY == us_auth_alg)
    {
        /* ׼��seq = 3����֤֡ */
        pst_auth_frame = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);

        if(OAL_PTR_NULL == pst_auth_frame)
        {
            /* TBD: ��λmac */
            OAM_ERROR_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::pst_auth_frame null.}");
            return OAL_ERR_CODE_PTR_NULL;
        }
        OAL_MEM_NETBUF_TRACE(pst_auth_frame, OAL_TRUE);

        OAL_MEMZERO(oal_netbuf_cb(pst_auth_frame), OAL_NETBUF_CB_SIZE());

        us_auth_frame_len = hmac_mgmt_encap_auth_req_seq3(pst_sta, (oal_uint8 *)OAL_NETBUF_HEADER(pst_auth_frame), puc_mac_hdr);
        oal_netbuf_put(pst_auth_frame, us_auth_frame_len);

        pst_hmac_user_ap = mac_res_get_hmac_user((oal_uint16)pst_sta->st_vap_base_info.uc_assoc_vap_id);
        if (OAL_PTR_NULL == pst_hmac_user_ap)
        {
            oal_netbuf_free(pst_auth_frame);
            OAM_ERROR_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::pst_hmac_user_ap[%d] null.}", pst_sta->st_vap_base_info.uc_assoc_vap_id);
            return OAL_ERR_CODE_PTR_NULL;
        }

        /* ��д���ͺͷ��������Ҫ�Ĳ��� */
        pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_auth_frame);
        pst_tx_ctl->us_mpdu_len   = us_auth_frame_len;                                 /* ������Ҫ֡���� */
        pst_tx_ctl->us_tx_user_idx              = pst_hmac_user_ap->st_user_base_info.us_assoc_id;   /* �������Ҫ��ȡ�û� */

        /* ���¼���dmac���� */
        ul_ret = hmac_tx_mgmt_send_event(&pst_sta->st_vap_base_info, pst_auth_frame, us_auth_frame_len);
        if (OAL_SUCC != ul_ret)
        {
            oal_netbuf_free(pst_auth_frame);
            OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_wait_auth_sta::hmac_tx_mgmt_send_event failed[%d].}", ul_ret);
            return ul_ret;
        }

        FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&pst_sta->st_mgmt_timer);

        /* ����״̬ΪMAC_VAP_STATE_STA_WAIT_AUTH_SEQ4����������ʱ�� */
        hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_WAIT_AUTH_SEQ4);

        FRW_TIMER_CREATE_TIMER(&pst_sta->st_mgmt_timer,
                               hmac_mgmt_timeout_sta,
                               pst_sta->st_mgmt_timer.ul_timeout,
                               &pst_sta->st_mgmt_timetout_param,
                               OAL_FALSE,
                               OAM_MODULE_ID_HMAC,
                               pst_sta->st_vap_base_info.ul_core_id);
        return OAL_SUCC;
    }
    else
    {
        FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&pst_sta->st_mgmt_timer);

        /* ���յ�AP �ظ���auth response ��֧����֤�㷨��ǰ��֧�ֵ�����£�status code ȴ��SUCC,
            ��Ϊ��֤�ɹ������Ҽ����������� */
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_wait_auth_seq2_rx::AP's auth_alg [%d] not support!}", us_auth_alg);

        /* ��״̬����ΪAUTH_COMP */
        hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);
        st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

        /* �ϱ���SME��֤�ɹ� */
        hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);
    }

    return OAL_SUCC;
}


oal_uint32  hmac_sta_wait_auth_seq4_rx(hmac_vap_stru *pst_sta, oal_void *p_msg)
{
    dmac_wlan_crx_event_stru    *pst_crx_event;
    hmac_rx_ctl_stru            *pst_rx_ctrl; /* ÿһ��MPDU�Ŀ�����Ϣ */
    oal_uint8                   *puc_mac_hdr;
    oal_uint16                   us_auth_status = MAC_UNSPEC_FAIL;
    hmac_auth_rsp_stru           st_auth_rsp    = {{0},};

    if (OAL_PTR_NULL == p_msg || OAL_PTR_NULL == pst_sta)
    {
        OAM_ERROR_LOG2(0, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq2_rx::param null,%d %d.}", p_msg, p_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_crx_event  = (dmac_wlan_crx_event_stru *)p_msg;
    pst_rx_ctrl    = (hmac_rx_ctl_stru *)oal_netbuf_cb(pst_crx_event->pst_netbuf); /* ÿһ��MPDU�Ŀ�����Ϣ */
    puc_mac_hdr    = (oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr;

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if(OAL_TRUE == wlan_pm_wkup_src_debug_get())
    {
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{wifi_wake_src:hmac_sta_wait_auth_seq4_rx_etc::wakeup mgmt type[0x%x]}",mac_get_frame_type_and_subtype(puc_mac_hdr));
    }
#endif
#endif

    if ((WLAN_FC0_SUBTYPE_AUTH|WLAN_FC0_TYPE_MGT) == mac_get_frame_sub_type(puc_mac_hdr))
    {
#ifdef _PRE_WLAN_FEATURE_SNIFFER
        proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_ctrl->st_rx_info.us_frame_len, 0);
#endif
        us_auth_status = mac_get_auth_status(puc_mac_hdr);

        if((WLAN_AUTH_TRASACTION_NUM_FOUR == mac_get_auth_seq_num(puc_mac_hdr)) &&
           (MAC_SUCCESSFUL_STATUSCODE == us_auth_status))
        {
            /* ���յ�seq = 4 ��״̬λΪsucc ȡ����ʱ�� */
            FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&pst_sta->st_mgmt_timer);

            st_auth_rsp.us_status_code = HMAC_MGMT_SUCCESS;

            /* ����sta״̬ΪMAC_VAP_STATE_STA_AUTH_COMP */
            hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_AUTH_COMP);
            OAM_INFO_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq4_rx::auth succ.}");
            /* ����֤����ϱ�SME */
            hmac_send_rsp_to_sme_sta(pst_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);
        }
        else
        {
            OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_sta_wait_auth_seq4_rx::transaction num.status[%d]}", \
              us_auth_status);
            /* �ȴ���ʱ����ʱ */
        }
    }

    return OAL_SUCC;
}



oal_uint32  hmac_sta_wait_asoc(hmac_vap_stru *pst_sta, oal_void *pst_msg)
{
    hmac_asoc_req_stru         *pst_hmac_asoc_req;
    oal_netbuf_stru            *pst_asoc_req_frame;
    mac_tx_ctl_stru            *pst_tx_ctl;
    hmac_user_stru             *pst_hmac_user_ap;
    oal_uint32                  ul_asoc_frame_len;
    oal_uint32                  ul_ret;
    oal_uint8                  *puc_curr_bssid;

    if ((OAL_PTR_NULL == pst_sta) || (OAL_PTR_NULL == pst_msg))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::param null,%d %d.}", pst_sta, pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_asoc_req = (hmac_asoc_req_stru *)pst_msg;

    pst_asoc_req_frame = OAL_MEM_NETBUF_ALLOC(OAL_NORMAL_NETBUF, WLAN_MEM_NETBUF_SIZE2, OAL_NETBUF_PRIORITY_MID);

    if (OAL_PTR_NULL == pst_asoc_req_frame)
    {
        OAM_ERROR_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::pst_asoc_req_frame null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    OAL_MEM_NETBUF_TRACE(pst_asoc_req_frame, OAL_TRUE);

    OAL_MEMZERO(oal_netbuf_cb(pst_asoc_req_frame), OAL_NETBUF_CB_SIZE());

    /* ��mac header���� */
    OAL_MEMZERO((oal_uint8 *)oal_netbuf_header(pst_asoc_req_frame), MAC_80211_FRAME_LEN);

    /* ��֡ (Re)Assoc_req_Frame */
    if (pst_sta->bit_reassoc_flag)
    {
        puc_curr_bssid = pst_sta->st_vap_base_info.auc_bssid;
    }
    else
    {
        puc_curr_bssid = OAL_PTR_NULL;
    }
    ul_asoc_frame_len = hmac_mgmt_encap_asoc_req_sta(pst_sta,(oal_uint8 *)(OAL_NETBUF_HEADER(pst_asoc_req_frame)), puc_curr_bssid);
    oal_netbuf_put(pst_asoc_req_frame, ul_asoc_frame_len);

    if (0 == ul_asoc_frame_len)
    {
        OAM_WARNING_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::hmac_mgmt_encap_asoc_req_sta null.}");
        oal_netbuf_free(pst_asoc_req_frame);

        return OAL_FAIL;
    }

    if (OAL_PTR_NULL != pst_sta->puc_asoc_req_ie_buff)
    {
        OAL_MEM_FREE(pst_sta->puc_asoc_req_ie_buff, OAL_TRUE);
        pst_sta->puc_asoc_req_ie_buff = OAL_PTR_NULL;
        pst_sta->ul_asoc_req_ie_len   = 0;
    }

    if(OAL_UNLIKELY(ul_asoc_frame_len < OAL_ASSOC_REQ_IE_OFFSET))
    {
        OAM_ERROR_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::invalid ul_asoc_req_ie_len[%u].}", ul_asoc_frame_len);
        oam_report_dft_params(BROADCAST_MACADDR, (oal_uint8 *)oal_netbuf_header(pst_asoc_req_frame),(oal_uint16)ul_asoc_frame_len, OAM_OTA_TYPE_80211_FRAME);
        oal_netbuf_free(pst_asoc_req_frame);
        return OAL_FAIL;
    }

    /*Should we change the ie buff from local mem to netbuf ?*/
    /* �˴�������ڴ棬ֻ���ϱ����ں˺��ͷ� */
    pst_sta->ul_asoc_req_ie_len = (pst_sta->bit_reassoc_flag) ? (ul_asoc_frame_len - OAL_ASSOC_REQ_IE_OFFSET - OAL_MAC_ADDR_LEN) :
                                   (ul_asoc_frame_len - OAL_ASSOC_REQ_IE_OFFSET);
    pst_sta->puc_asoc_req_ie_buff = OAL_MEM_ALLOC(OAL_MEM_POOL_ID_LOCAL, (oal_uint16)(pst_sta->ul_asoc_req_ie_len), OAL_TRUE);
    if (OAL_PTR_NULL == pst_sta->puc_asoc_req_ie_buff)
    {
        OAM_ERROR_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::puc_asoc_req_ie_buff null,alloc %u bytes failed}",
                        (oal_uint16)(pst_sta->ul_asoc_req_ie_len));
        oal_netbuf_free(pst_asoc_req_frame);
        return OAL_FAIL;
    }

    if (pst_sta->bit_reassoc_flag)
    {
        oal_memcopy(pst_sta->puc_asoc_req_ie_buff, OAL_NETBUF_HEADER(pst_asoc_req_frame) + OAL_ASSOC_REQ_IE_OFFSET + OAL_MAC_ADDR_LEN, pst_sta->ul_asoc_req_ie_len);
    }
    else
    {
        oal_memcopy(pst_sta->puc_asoc_req_ie_buff, OAL_NETBUF_HEADER(pst_asoc_req_frame) + OAL_ASSOC_REQ_IE_OFFSET, pst_sta->ul_asoc_req_ie_len);
    }

    pst_hmac_user_ap = (hmac_user_stru *)mac_res_get_hmac_user((oal_uint16)pst_sta->st_vap_base_info.uc_assoc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_user_ap)
    {
        OAM_ERROR_LOG0(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::pst_hmac_user_ap null.}");
        oal_netbuf_free(pst_asoc_req_frame);
        OAL_MEM_FREE(pst_sta->puc_asoc_req_ie_buff, OAL_TRUE);
        pst_sta->puc_asoc_req_ie_buff = OAL_PTR_NULL;
        pst_sta->ul_asoc_req_ie_len   = 0;

        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_tx_ctl = (mac_tx_ctl_stru *)oal_netbuf_cb(pst_asoc_req_frame);

    pst_tx_ctl->us_mpdu_len   = (oal_uint16)ul_asoc_frame_len;
    pst_tx_ctl->us_tx_user_idx              = pst_hmac_user_ap->st_user_base_info.us_assoc_id;

    /* ���¼���DMAC����֡���� */
    ul_ret = hmac_tx_mgmt_send_event(&(pst_sta->st_vap_base_info), pst_asoc_req_frame, (oal_uint16)ul_asoc_frame_len);
    if (OAL_SUCC != ul_ret)
    {
        oal_netbuf_free(pst_asoc_req_frame);
        OAL_MEM_FREE(pst_sta->puc_asoc_req_ie_buff, OAL_TRUE);
        pst_sta->puc_asoc_req_ie_buff = OAL_PTR_NULL;
        pst_sta->ul_asoc_req_ie_len   = 0;

        OAM_WARNING_LOG1(pst_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc::hmac_tx_mgmt_send_event failed[%d].}", ul_ret);
        return ul_ret;
    }

    /* ����״̬ */
    hmac_fsm_change_state(pst_sta, MAC_VAP_STATE_STA_WAIT_ASOC);

    /* ����������ʱ��ʱ��, Ϊ�Զ�ap����һ����ʱ���������ʱapû��asoc rsp��������ʱ���� */
    pst_sta->st_mgmt_timetout_param.en_state = MAC_VAP_STATE_STA_WAIT_ASOC;
    pst_sta->st_mgmt_timetout_param.us_user_index = pst_hmac_user_ap->st_user_base_info.us_assoc_id;
    pst_sta->st_mgmt_timetout_param.uc_vap_id = pst_sta->st_vap_base_info.uc_vap_id;
    pst_sta->st_mgmt_timetout_param.en_status_code = MAC_AUTH_TIMEOUT;
    FRW_TIMER_CREATE_TIMER(&(pst_sta->st_mgmt_timer),
                           hmac_mgmt_timeout_sta,
                           pst_hmac_asoc_req->us_assoc_timeout,
                           &(pst_sta->st_mgmt_timetout_param),
                           OAL_FALSE,
                           OAM_MODULE_ID_HMAC,
                           pst_sta->st_vap_base_info.ul_core_id);

    return OAL_SUCC;
}

#ifdef _PRE_WLAN_FEATURE_P2P


oal_void hmac_p2p_listen_comp_cb(void *p_arg)
{
    hmac_vap_stru                      *pst_hmac_vap;
    mac_device_stru                    *pst_mac_device;
    hmac_scan_record_stru              *pst_scan_record;

    pst_scan_record = (hmac_scan_record_stru *)p_arg;

    /* �ж�listen���ʱ��״̬ */
    if (MAC_SCAN_SUCCESS != pst_scan_record->en_scan_rsp_status)
    {
        OAM_WARNING_LOG1(0, OAM_SF_P2P, "{hmac_p2p_listen_comp_cb::listen failed, listen rsp status: %d.}",
                         pst_scan_record->en_scan_rsp_status);
    }

    pst_hmac_vap   = mac_res_get_hmac_vap(pst_scan_record->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(0, OAM_SF_P2P, "{hmac_p2p_listen_comp_cb::pst_hmac_vap is null:vap_id %d.}",
                                 pst_scan_record->uc_vap_id);
        return;
    }

    pst_mac_device = mac_res_get_dev(pst_scan_record->uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_ERROR_LOG1(0, OAM_SF_P2P, "{hmac_p2p_listen_comp_cb::pst_mac_device is null:vap_id %d.}",
                                 pst_scan_record->uc_device_id);
        return;
    }

    
    if (pst_scan_record->ull_cookie == pst_mac_device->st_p2p_info.ull_last_roc_id)
    {
        /* ״̬������: hmac_p2p_listen_timeout */
        if (OAL_SUCC != hmac_fsm_call_func_sta(pst_hmac_vap, HMAC_FSM_INPUT_LISTEN_TIMEOUT, &(pst_hmac_vap->st_vap_base_info)))
            {
            OAM_WARNING_LOG0(0,OAM_SF_P2P,"{hmac_p2p_listen_comp_cb::hmac_fsm_call_func_sta fail.}");
            }
    }
    else
    {
        OAM_WARNING_LOG3(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
                        "{hmac_p2p_listen_comp_cb::ignore listen complete.scan_report_cookie[%x], current_listen_cookie[%x], ull_last_roc_id[%x].}",
                        pst_scan_record->ull_cookie,
                        pst_mac_device->st_scan_params.ull_cookie,
                        pst_mac_device->st_p2p_info.ull_last_roc_id);
    }


    return;
}


OAL_STATIC oal_void  hmac_cfg80211_prepare_listen_req_param(mac_scan_req_stru *pst_scan_params, oal_int8 *puc_param)
{
    mac_remain_on_channel_param_stru *pst_remain_on_channel;
    mac_channel_stru                 *pst_channel_tmp;

    pst_remain_on_channel = (mac_remain_on_channel_param_stru *)puc_param;

    OAL_MEMZERO(pst_scan_params, OAL_SIZEOF(mac_scan_req_stru));

    /* ���ü����ŵ���Ϣ��ɨ������� */
    pst_scan_params->ast_channel_list[0].en_band        = pst_remain_on_channel->en_band;
    pst_scan_params->ast_channel_list[0].en_bandwidth   = pst_remain_on_channel->en_listen_channel_type;
    pst_scan_params->ast_channel_list[0].uc_chan_number = pst_remain_on_channel->uc_listen_channel;
    pst_scan_params->ast_channel_list[0].uc_idx         = 0;
    pst_channel_tmp = &(pst_scan_params->ast_channel_list[0]);
    if (OAL_SUCC != mac_get_channel_idx_from_num(pst_channel_tmp->en_band, pst_channel_tmp->uc_chan_number, &(pst_channel_tmp->uc_idx)))
    {
        OAM_WARNING_LOG2(0,OAM_SF_P2P,"{hmac_cfg80211_prepare_listen_req_param::mac_get_channel_idx_from_num fail.band[%u]  channel[%u]}",pst_channel_tmp->en_band, pst_channel_tmp->uc_chan_number);
    }

    /* ���������������� */
    pst_scan_params->uc_max_scan_count_per_channel      = 1;
    pst_scan_params->uc_channel_nums = 1;
    pst_scan_params->uc_scan_func    = MAC_SCAN_FUNC_P2P_LISTEN;
    pst_scan_params->us_scan_time    = (oal_uint16)pst_remain_on_channel->ul_listen_duration;
    pst_scan_params->p_fn_cb         = hmac_p2p_listen_comp_cb;
    pst_scan_params->ull_cookie      = pst_remain_on_channel->ull_cookie;
    pst_scan_params->bit_is_p2p0_scan = OAL_TRUE;
    pst_scan_params->uc_p2p0_listen_channel = pst_remain_on_channel->uc_listen_channel;

    return;
}


oal_uint32  hmac_p2p_listen_timeout(hmac_vap_stru *pst_hmac_vap_sta, oal_void *p_param)
{
    mac_device_stru                    *pst_mac_device;
    hmac_vap_stru                      *pst_hmac_vap;
    mac_vap_stru                       *pst_mac_vap;

    pst_mac_vap    = (mac_vap_stru *)p_param;
    pst_hmac_vap   = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG1(0,OAM_SF_P2P,"{hmac_p2p_listen_timeout::mac_res_get_hmac_vap fail.vap_id[%u]!}",pst_mac_vap->uc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_WARNING_LOG1(0,OAM_SF_P2P,"{hmac_p2p_listen_timeout::mac_res_get_dev fail.device_id[%u]!}",pst_mac_vap->uc_device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    OAM_INFO_LOG2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
                  "{hmac_p2p_listen_timeout::current pst_mac_vap channel is [%d] state[%d]}",
                  pst_mac_vap->st_channel.uc_chan_number,
                  pst_hmac_vap->st_vap_base_info.en_vap_state);

    OAM_INFO_LOG2(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_P2P,
                  "{hmac_p2p_listen_timeout::next pst_mac_vap channel is [%d] state[%d]}",
                  pst_mac_vap->st_channel.uc_chan_number,
                  pst_mac_device->st_p2p_info.en_last_vap_state);

    /* ����P2P0 ��P2P_CL ����vap �ṹ�壬������ʱ�����ؼ���ǰ�����״̬ */
    mac_vap_state_change(&pst_hmac_vap->st_vap_base_info, pst_mac_device->st_p2p_info.en_last_vap_state);
    hmac_set_rx_filter_value(&pst_hmac_vap->st_vap_base_info);

    /* 3.1 ���¼���WAL ���ϱ��������� */
    hmac_p2p_send_listen_expired_to_host(pst_hmac_vap);

    /* 3.2 ���¼���DMAC �����ؼ����ŵ� */
    hmac_p2p_send_listen_expired_to_device(pst_hmac_vap);

    return OAL_SUCC;
}

#if 0

oal_uint32 hmac_p2p_listen_timeout_fn(void *p_arg)
{
    hmac_vap_stru                      *pst_hmac_vap;
    mac_vap_stru                       *pst_mac_vap;

    pst_mac_vap    = (mac_vap_stru *)p_arg;
    pst_hmac_vap   = mac_res_get_hmac_vap(pst_mac_vap->uc_vap_id);

    /* ״̬������: hmac_p2p_listen_timeout */
    hmac_fsm_call_func_sta(pst_hmac_vap, HMAC_FSM_INPUT_LISTEN_TIMEOUT, pst_mac_vap);

    return OAL_SUCC;
}
#endif


oal_uint32 hmac_p2p_remain_on_channel(hmac_vap_stru *pst_hmac_vap_sta, oal_void *p_param)
{
    mac_device_stru                     *pst_mac_device;
    mac_vap_stru                        *pst_mac_vap;
    mac_remain_on_channel_param_stru    *pst_remain_on_channel;
    mac_scan_req_stru                    st_scan_params;
    oal_uint32                           ul_ret;

    pst_remain_on_channel = (mac_remain_on_channel_param_stru*)p_param;

    pst_mac_vap     = mac_res_get_mac_vap(pst_hmac_vap_sta->st_vap_base_info.uc_vap_id);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG1(0,OAM_SF_P2P,"{hmac_p2p_remain_on_channel::mac_res_get_mac_vap fail.vap_id[%u]!}",pst_hmac_vap_sta->st_vap_base_info.uc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_mac_device  = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_device))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hmac_p2p_listen_timeout::pst_mac_device[%d](%p) null!}", pst_mac_vap->uc_device_id);
        return OAL_ERR_CODE_PTR_NULL;
    }


    
    if (MAC_VAP_STATE_STA_LISTEN == pst_hmac_vap_sta->st_vap_base_info.en_vap_state)
    {
        hmac_p2p_send_listen_expired_to_host(pst_hmac_vap_sta);
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_P2P,
                        "{hmac_p2p_remain_on_channel::listen nested, send remain on channel expired to host!curr_state[%d]\r\n}",
                        pst_hmac_vap_sta->st_vap_base_info.en_vap_state);
    }

    /* �޸�P2P_DEVICE ״̬Ϊ����״̬ */
    mac_vap_state_change((mac_vap_stru *)&pst_hmac_vap_sta->st_vap_base_info, MAC_VAP_STATE_STA_LISTEN);
    hmac_set_rx_filter_value((mac_vap_stru *)&pst_hmac_vap_sta->st_vap_base_info);

    OAM_INFO_LOG4(pst_mac_vap->uc_vap_id, OAM_SF_P2P,
                  "{hmac_p2p_remain_on_channel : get in listen state!last_state %d, channel %d, duration %d, curr_state %d}\r\n",
                  pst_mac_device->st_p2p_info.en_last_vap_state,
                  pst_remain_on_channel->uc_listen_channel,
                  pst_remain_on_channel->ul_listen_duration,
                  pst_hmac_vap_sta->st_vap_base_info.en_vap_state);

    /* ׼���������� */
    hmac_cfg80211_prepare_listen_req_param(&st_scan_params, (oal_int8 *)pst_remain_on_channel);

    /* ����ɨ����ڣ�׼�����м������������ܼ�������ִ�гɹ���ʧ�ܣ������ؼ����ɹ� */
    /* ״̬������: hmac_scan_proc_scan_req_event */
    ul_ret = hmac_fsm_call_func_sta(pst_hmac_vap_sta, HMAC_FSM_INPUT_SCAN_REQ, (oal_void *)(&st_scan_params));
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(0, OAM_SF_SCAN, "{hmac_p2p_remain_on_channel::hmac_fsm_call_func_sta fail[%d].}", ul_ret);
    }

    return OAL_SUCC;
}


#endif /* _PRE_WLAN_FEATURE_P2P */

#if defined(_PRE_WLAN_FEATURE_HS20) || defined(_PRE_WLAN_FEATURE_P2P) || defined(_PRE_WLAN_FEATURE_HILINK)


oal_uint32  hmac_sta_not_up_rx_mgmt(hmac_vap_stru *pst_hmac_vap_sta, oal_void *p_param)
{
    dmac_wlan_crx_event_stru   *pst_mgmt_rx_event;
    mac_vap_stru               *pst_mac_vap;
    mac_rx_ctl_stru            *pst_rx_info;
    oal_uint8                  *puc_mac_hdr;
#ifdef _PRE_WLAN_FEATURE_LOCATION_RAM
    oal_uint8                  *puc_data;
#endif
    oal_uint8                   uc_mgmt_frm_type;

    if (OAL_PTR_NULL == pst_hmac_vap_sta || OAL_PTR_NULL == p_param)
    {
        OAM_ERROR_LOG2(0, OAM_SF_RX, "{hmac_sta_not_up_rx_mgmt::PTR_NULL, hmac_vap_addr[%p], param[%p].}",
                       pst_hmac_vap_sta, p_param);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_mac_vap        = &(pst_hmac_vap_sta->st_vap_base_info);
    pst_mgmt_rx_event  =  (dmac_wlan_crx_event_stru *)p_param;
    pst_rx_info        =  (mac_rx_ctl_stru *)oal_netbuf_cb(pst_mgmt_rx_event->pst_netbuf);
    puc_mac_hdr        =  (oal_uint8 *)(pst_rx_info->pul_mac_hdr_start_addr);
    if(OAL_PTR_NULL == puc_mac_hdr)
    {
        OAM_ERROR_LOG3(pst_rx_info->uc_mac_vap_id, OAM_SF_RX, "{hmac_sta_not_up_rx_mgmt::puc_mac_hdr null, vap_id %d,us_frame_len %d, uc_mac_header_len %d}",
                       pst_rx_info->bit_vap_id,
                       pst_rx_info->us_frame_len,
                       pst_rx_info->uc_mac_header_len);
        return OAL_ERR_CODE_PTR_NULL;
    }
#ifdef _PRE_WLAN_FEATURE_LOCATION_RAM
    /* ��ȡ֡��ָ�� */
    puc_data = (oal_uint8 *)pst_rx_info->pul_mac_hdr_start_addr + pst_rx_info->uc_mac_header_len;
#endif
    /* STA��NOT UP״̬�½��յ����ֹ���֡����*/
    uc_mgmt_frm_type = mac_get_frame_sub_type(puc_mac_hdr);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if(OAL_TRUE == wlan_pm_wkup_src_debug_get())
    {
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
        OAM_WARNING_LOG1(pst_hmac_vap_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{wifi_wake_src:hmac_sta_not_up_rx_mgmt_etc::wakeup mgmt type[0x%x]}",uc_mgmt_frm_type);
    }
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_SNIFFER
    proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_info->us_frame_len, 0);
#endif
    switch (uc_mgmt_frm_type)
    {
        /* �жϽ��յ��Ĺ���֡����*/
        case WLAN_FC0_SUBTYPE_PROBE_REQ:
            #ifdef _PRE_WLAN_FEATURE_P2P
            /* �ж�ΪP2P�豸,���ϱ�probe req֡��wpa_supplicant*/
            if (!IS_LEGACY_VAP(pst_mac_vap))
            {
                hmac_rx_mgmt_send_to_host(pst_hmac_vap_sta, pst_mgmt_rx_event->pst_netbuf);
            }
            break;
            #endif
        case WLAN_FC0_SUBTYPE_ACTION|WLAN_FC0_TYPE_MGT:
            /* �����Action ֡����ֱ���ϱ�wpa_supplicant*/
#ifdef _PRE_WLAN_FEATURE_LOCATION_RAM
            if (0 == oal_memcmp(puc_data  + 2, g_auc_huawei_oui, MAC_OUI_LEN))
            {
                hmac_huawei_action_process(pst_hmac_vap_sta, pst_mgmt_rx_event->pst_netbuf, puc_data[2 + MAC_OUI_LEN]);
            }
            else
#endif  /* _PRE_WLAN_FEATURE_LOCATION */
            /* �����Action ֡����ֱ���ϱ�wpa_supplicant*/
            {
                hmac_rx_mgmt_send_to_host(pst_hmac_vap_sta, pst_mgmt_rx_event->pst_netbuf);
            }
            break;
        case WLAN_FC0_SUBTYPE_PROBE_RSP:
            /* �����probe response֡����ֱ���ϱ�wpa_supplicant*/
            hmac_rx_mgmt_send_to_host(pst_hmac_vap_sta, pst_mgmt_rx_event->pst_netbuf);
            break;
        default:
            break;
    }
    return OAL_SUCC;
}
#endif /* _PRE_WLAN_FEATURE_HS20 and _PRE_WLAN_FEATURE_P2P and _PRE_WLAN_FEATURE_HILINK */


OAL_STATIC oal_uint32  hmac_update_vht_opern_ie_sta(
                    mac_vap_stru            *pst_mac_vap,
                    hmac_user_stru          *pst_hmac_user,
                    oal_uint8               *puc_payload,
                    oal_uint16               us_msg_idx)
{
    if (OAL_UNLIKELY((OAL_PTR_NULL == pst_mac_vap) || (OAL_PTR_NULL == pst_hmac_user) || (OAL_PTR_NULL == puc_payload)))
    {
        OAM_ERROR_LOG3(0, OAM_SF_ASSOC, "{hmac_update_vht_opern_ie_sta::param null,%d %d %d.}", pst_mac_vap, pst_hmac_user, puc_payload);
        return MAC_NO_CHANGE;
    }

    /* ֧��11ac���Ž��к����Ĵ��� */
    if (OAL_FALSE == mac_mib_get_VHTOptionImplemented(pst_mac_vap))
    {
        return MAC_NO_CHANGE;
    }

    return mac_ie_proc_vht_opern_ie(pst_mac_vap, puc_payload, &(pst_hmac_user->st_user_base_info));
}


oal_uint32 hmac_sta_up_update_edca_params_machw(
                      hmac_vap_stru                          *pst_hmac_sta,
                      mac_wmm_set_param_type_enum_uint8      en_type)
{
    frw_event_mem_stru                       *pst_event_mem;
    frw_event_stru                           *pst_event;
    dmac_ctx_sta_asoc_set_edca_reg_stru       st_asoc_set_edca_reg_param = {0};

    /* ���¼���dmacд�Ĵ��� */
    /* �����¼��ڴ� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_ctx_sta_asoc_set_edca_reg_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_update_vht_opern_ie_sta::event_mem alloc null, size[%d].}",
                OAL_SIZEOF(dmac_ctx_sta_asoc_set_edca_reg_stru));
        return OAL_ERR_CODE_PTR_NULL;
    }

    st_asoc_set_edca_reg_param.uc_vap_id = pst_hmac_sta->st_vap_base_info.uc_vap_id;
    st_asoc_set_edca_reg_param.en_set_param_type = en_type;

    /* ��д�¼� */
    pst_event = (frw_event_stru *)pst_event_mem->puc_data;
    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                   FRW_EVENT_TYPE_WLAN_CTX,
                   DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_EDCA_REG,
                   OAL_SIZEOF(dmac_ctx_sta_asoc_set_edca_reg_stru),
                   FRW_EVENT_PIPELINE_STAGE_1,
                   pst_hmac_sta->st_vap_base_info.uc_chip_id,
                   pst_hmac_sta->st_vap_base_info.uc_device_id,
                   pst_hmac_sta->st_vap_base_info.uc_vap_id);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if (MAC_WMM_SET_PARAM_TYPE_DEFAULT != en_type)
    {
        oal_memcopy((oal_uint8 *)&st_asoc_set_edca_reg_param.ast_wlan_mib_qap_edac, (oal_uint8 *)&pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac, (OAL_SIZEOF(wlan_mib_Dot11QAPEDCAEntry_stru) * WLAN_WME_AC_BUTT));
    }
#endif

    /* �������� */
    oal_memcopy(frw_get_event_payload(pst_event_mem), (oal_uint8 *)&st_asoc_set_edca_reg_param, OAL_SIZEOF(dmac_ctx_sta_asoc_set_edca_reg_stru));

    /* �ַ��¼� */
    frw_event_dispatch_event(pst_event_mem);
    FRW_EVENT_FREE(pst_event_mem);

    return OAL_SUCC;
}


OAL_STATIC oal_void hmac_sta_up_update_edca_params_mib(hmac_vap_stru  *pst_hmac_sta, oal_uint8  *puc_payload)
{
    oal_uint8               uc_aifsn;
    oal_uint8               uc_aci;
    oal_uint8               uc_ecwmin;
    oal_uint8               uc_ecwmax;
    oal_uint16              us_txop_limit;
    oal_bool_enum_uint8     en_acm;
    /******** AC Parameters Record Format *********/
    /* ------------------------------------------ */
    /* |     1     |       1       |      2     | */
    /* ------------------------------------------ */
    /* | ACI/AIFSN | ECWmin/ECWmax | TXOP Limit | */
    /* ------------------------------------------ */

    /************* ACI/AIFSN Field ***************/
    /*     ---------------------------------- */
    /* bit |   4   |  1  |  2  |    1     |   */
    /*     ---------------------------------- */
    /*     | AIFSN | ACM | ACI | Reserved |   */
    /*     ---------------------------------- */

    uc_aifsn = puc_payload[0] & MAC_WMM_QOS_PARAM_AIFSN_MASK;
    en_acm   = (puc_payload[0] & BIT4)?OAL_TRUE:OAL_FALSE;
    uc_aci   = (puc_payload[0] >> MAC_WMM_QOS_PARAM_ACI_BIT_OFFSET) & MAC_WMM_QOS_PARAM_ACI_MASK;

    /* ECWmin/ECWmax Field */
    /*     ------------------- */
    /* bit |   4    |   4    | */
    /*     ------------------- */
    /*     | ECWmin | ECWmax | */
    /*     ------------------- */
    uc_ecwmin = (puc_payload[1] & MAC_WMM_QOS_PARAM_ECWMIN_MASK);
    uc_ecwmax = ((puc_payload[1] & MAC_WMM_QOS_PARAM_ECWMAX_MASK) >> MAC_WMM_QOS_PARAM_ECWMAX_BIT_OFFSET);

    /* ��mib���кͼĴ����ﱣ���TXOPֵ������usΪ��λ�ģ����Ǵ����ʱ������32usΪ
       ��λ���д���ģ�����ڽ�����ʱ����Ҫ����������ֵ����32
    */
    us_txop_limit = puc_payload[2] |
                    ((puc_payload[3] & MAC_WMM_QOS_PARAM_TXOPLIMIT_MASK) << MAC_WMM_QOS_PARAM_BIT_NUMS_OF_ONE_BYTE);
    us_txop_limit = (oal_uint16)(us_txop_limit << MAC_WMM_QOS_PARAM_TXOPLIMIT_SAVE_TO_TRANS_TIMES);

    /* ������Ӧ��MIB����Ϣ */
    if (uc_aci < WLAN_WME_AC_BUTT)
    {
        pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[uc_aci].ul_dot11QAPEDCATableIndex     = uc_aci + 1;
        pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[uc_aci].ul_dot11QAPEDCATableCWmin     = uc_ecwmin;
        pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[uc_aci].ul_dot11QAPEDCATableCWmax     = uc_ecwmax;
        pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[uc_aci].ul_dot11QAPEDCATableAIFSN     = uc_aifsn;
        pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[uc_aci].ul_dot11QAPEDCATableTXOPLimit = us_txop_limit;
        pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[uc_aci].en_dot11QAPEDCATableMandatory = en_acm;
    }
}
#if 0

OAL_STATIC oal_void hmac_sta_up_process_erp_ie(
                    oal_uint8               *puc_payload,
                    oal_uint16               us_msg_len,
                    oal_uint16               us_info_elem_offset,
                    hmac_user_stru          *pst_hmac_user)
{
    oal_uint16              us_msg_offset;
    mac_erp_params_stru    *pst_erp_params;

    /* ����ƫ�� */
    us_msg_offset = us_info_elem_offset;

    /************************ ERP Element *************************************
    --------------------------------------------------------------------------
    |EID  |Len  |NonERP_Present|Use_Protection|Barker_Preamble_Mode|Reserved|
    --------------------------------------------------------------------------
    |B0-B7|B0-B7|B0            |B1            |B2                  |B3-B7   |
    --------------------------------------------------------------------------
    ***************************************************************************/

    while (us_msg_offset < us_msg_len)
    {
        /* �жϵ�ǰ��ie�Ƿ���ERP ie��������ǣ����������һ��ie������ǣ�����use protection ��Ϣ */
        if (MAC_EID_ERP == puc_payload[us_msg_offset])
        {
            us_msg_offset += MAC_IE_HDR_LEN;
            pst_erp_params = (mac_erp_params_stru *)(&puc_payload[us_msg_offset]);

            /* ����use_protect ��Ϣ */
            pst_hmac_user->st_user_base_info.st_cap_info.bit_erp_use_protect = pst_erp_params->bit_use_protection;
            /*����preamble mode*/
            mac_user_set_barker_preamble_mode(&pst_hmac_user->st_user_base_info, pst_erp_params->bit_preamble_mode);
            return ;
        }

        us_msg_offset += (puc_payload[us_msg_offset + 1] + MAC_IE_HDR_LEN);
    }

}


OAL_STATIC oal_void hmac_sta_up_process_ht_operation_ie(
                    oal_uint8               *puc_payload,
                    oal_uint16               us_msg_len,
                    oal_uint16               us_info_elem_offset,
                    hmac_user_stru          *pst_hmac_user)
{
    oal_uint16              us_msg_offset;
    mac_ht_opern_stru      *pst_ht_opern;

    /* ����ƫ�� */
    us_msg_offset = us_info_elem_offset;

    /************************ HT Operation Element *************************************
      ----------------------------------------------------------------------
      |EID |Length |PrimaryChannel |HT Operation Information |Basic MCS Set|
      ----------------------------------------------------------------------
      |1   |1      |1              |5                        |16           |
      ----------------------------------------------------------------------
    ***************************************************************************/

    /************************ HT Information Field ****************************
     |--------------------------------------------------------------------|
     | Primary | Seconday  | STA Ch | RIFS |           reserved           |
     | Channel | Ch Offset | Width  | Mode |                              |
     |--------------------------------------------------------------------|
     |    1    | B0     B1 |   B2   |  B3  |    B4                     B7 |
     |--------------------------------------------------------------------|

     |----------------------------------------------------------------|
     |     HT     | Non-GF STAs | resv      | OBSS Non-HT  | Reserved |
     | Protection |   Present   |           | STAs Present |          |
     |----------------------------------------------------------------|
     | B0     B1  |     B2      |    B3     |     B4       | B5   B15 |
     |----------------------------------------------------------------|

     |-------------------------------------------------------------|
     | Reserved |  Dual  |  Dual CTS  | Seconday | LSIG TXOP Protn |
     |          | Beacon | Protection |  Beacon  | Full Support    |
     |-------------------------------------------------------------|
     | B0    B5 |   B6   |     B7     |     B8   |       B9        |
     |-------------------------------------------------------------|

     |---------------------------------------|
     |  PCO   |  PCO  | Reserved | Basic MCS |
     | Active | Phase |          |    Set    |
     |---------------------------------------|
     |  B10   |  B11  | B12  B15 |    16     |
     |---------------------------------------|
    **************************************************************************/

    while (us_msg_offset < us_msg_len)
    {
        /* �жϵ�ǰ��ie�Ƿ���ht operation ie��������ǣ����������һ��ie������ǣ�����HT��Ϣ */
        if (MAC_EID_HT_OPERATION == puc_payload[us_msg_offset])
        {
            us_msg_offset += MAC_IE_HDR_LEN;
            pst_ht_opern  = (mac_ht_opern_stru *)(&puc_payload[us_msg_offset]);

            /*��֡����ȡֵ*/
            pst_hmac_user->st_user_base_info.st_ht_hdl.bit_rifs_mode                         = pst_ht_opern->bit_rifs_mode;/*������������дʱ����Ҫ��ֵ*/
            pst_hmac_user->st_user_base_info.st_ht_hdl.bit_HT_protection                     = pst_ht_opern->bit_HT_protection;
            pst_hmac_user->st_user_base_info.st_ht_hdl.bit_nongf_sta_present                 = pst_ht_opern->bit_nongf_sta_present;
            pst_hmac_user->st_user_base_info.st_ht_hdl.bit_obss_nonht_sta_present            = pst_ht_opern->bit_obss_nonht_sta_present;
            pst_hmac_user->st_user_base_info.st_ht_hdl.bit_lsig_txop_protection_full_support = pst_ht_opern->bit_lsig_txop_protection_full_support;
            return ;
        }

        us_msg_offset += (puc_payload[us_msg_offset + 1] + MAC_IE_HDR_LEN);
    }

}
#endif


oal_void hmac_sta_up_update_edca_params(
                    oal_uint8               *puc_payload,
                    oal_uint16               us_msg_len,
                    oal_uint16               us_info_elem_offset,
                    hmac_vap_stru           *pst_hmac_sta,
                    oal_uint8                uc_frame_sub_type,
                    hmac_user_stru          *pst_hmac_user)
{
    oal_uint16              us_msg_offset = 0;
    oal_uint8               uc_param_set_cnt;
    oal_uint8               uc_ac_num_loop;
    oal_uint32              ul_ret;
    mac_device_stru        *pst_mac_device;
    oal_uint8               uc_edca_param_set;
    oal_uint8              *puc_ie     = OAL_PTR_NULL;

    pst_mac_device = (mac_device_stru *)mac_res_get_dev(pst_hmac_sta->st_vap_base_info.uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_WARNING_LOG1(0,OAM_SF_ASSOC,"{hmac_sta_up_update_edca_params::mac_res_get_dev fail.device_id[%u]!}",pst_hmac_sta->st_vap_base_info.uc_device_id);
        return;
    }

    /************************ WMM Parameter Element ***************************/
    /* ------------------------------------------------------------------------------ */
    /* | EID | LEN | OUI |OUI Type |OUI Subtype |Version |QoS Info |Resd |AC Params | */
    /* ------------------------------------------------------------------------------ */
    /* |  1  |  1  |  3  |    1    |     1      |    1   |    1    |  1  |    16    | */
    /* ------------------------------------------------------------------------------ */

    /******************* QoS Info field when sent from WMM AP *****************/
    /*        --------------------------------------------                    */
    /*          | Parameter Set Count | Reserved | U-APSD |                   */
    /*          --------------------------------------------                  */
    /*   bit    |        0~3          |   4~6    |   7    |                   */
    /*          --------------------------------------------                  */
    /**************************************************************************/
    if (us_info_elem_offset < us_msg_len)
    {
        us_msg_len  -= us_info_elem_offset;
        puc_payload += us_info_elem_offset;

        puc_ie = mac_get_wmm_ie(puc_payload, us_msg_len);
        if (OAL_PTR_NULL != puc_ie)
        {
            /* ����wmm ie�Ƿ�Я��EDCA���� */
            uc_edca_param_set = puc_ie[MAC_OUISUBTYPE_WMM_PARAM_OFFSET];
            uc_param_set_cnt  = puc_ie[HMAC_WMM_QOS_PARAMS_HDR_LEN] & 0x0F;

            /* ����յ�����beacon֡������param_set_countû�иı䣬˵��AP��WMM����û�б�
               ��STAҲ�������κθı䣬ֱ�ӷ��ؼ��� */
            if ((WLAN_FC0_SUBTYPE_BEACON == uc_frame_sub_type)
                && (uc_param_set_cnt == pst_hmac_sta->st_vap_base_info.uc_wmm_params_update_count))
            {
                return;
            }

            pst_mac_device->en_wmm = OAL_TRUE;

            if(WLAN_FC0_SUBTYPE_BEACON == uc_frame_sub_type)
            {
                /* ����QoS Info */
                mac_vap_set_wmm_params_update_count(&pst_hmac_sta->st_vap_base_info, uc_param_set_cnt);
            }

            if (puc_ie[HMAC_WMM_QOS_PARAMS_HDR_LEN] & BIT7)
            {
                mac_user_set_apsd(&(pst_hmac_user->st_user_base_info), OAL_TRUE);
            }
            else
            {
                mac_user_set_apsd(&(pst_hmac_user->st_user_base_info), OAL_FALSE);
            }

            us_msg_offset = (HMAC_WMM_QOSINFO_AND_RESV_LEN + HMAC_WMM_QOS_PARAMS_HDR_LEN);

            /* wmm ie�в�Я��edca���� ֱ�ӷ��� */
            if(MAC_OUISUBTYPE_WMM_PARAM != uc_edca_param_set)
            {
                return;
            }

            /* ���ÿһ��AC������EDCA���� */
            for (uc_ac_num_loop = 0; uc_ac_num_loop < WLAN_WME_AC_BUTT; uc_ac_num_loop++)
            {
                hmac_sta_up_update_edca_params_mib(pst_hmac_sta, &puc_ie[us_msg_offset]);
#ifdef _PRE_WLAN_FEATURE_WMMAC
                if (pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[uc_ac_num_loop].en_dot11QAPEDCATableMandatory == 1)
                {
                    pst_hmac_user->st_user_base_info.st_ts_info[uc_ac_num_loop].en_ts_status = MAC_TS_INIT;
                    pst_hmac_user->st_user_base_info.st_ts_info[uc_ac_num_loop].uc_tsid      = 0xFF;
                }
                else
                {
                    pst_hmac_user->st_user_base_info.st_ts_info[uc_ac_num_loop].en_ts_status = MAC_TS_NONE;
                    pst_hmac_user->st_user_base_info.st_ts_info[uc_ac_num_loop].uc_tsid      = 0xFF;
                }

                OAM_INFO_LOG2(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_up_update_edca_params::ac num[%d], ts status[%d].}",
                                 uc_ac_num_loop, pst_hmac_user->st_user_base_info.st_ts_info[uc_ac_num_loop].en_ts_status);
#endif  // _PRE_WLAN_FEATURE_WMMAC
                us_msg_offset += HMAC_WMM_AC_PARAMS_RECORD_LEN;
            }

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
            hcc_host_update_vi_flowctl_param(pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[WLAN_WME_AC_BE].ul_dot11QAPEDCATableCWmin,
                pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[WLAN_WME_AC_VI].ul_dot11QAPEDCATableCWmin);
#endif
            /* ����EDCA��ص�MAC�Ĵ��� */
            hmac_sta_up_update_edca_params_machw(pst_hmac_sta, MAC_WMM_SET_PARAM_TYPE_UPDATE_EDCA);

            return;
        }

        puc_ie = mac_find_ie(MAC_EID_HT_CAP, puc_payload, us_msg_len);
        if (OAL_PTR_NULL != puc_ie)
        {
            mac_vap_init_wme_param(&pst_hmac_sta->st_vap_base_info);
            pst_mac_device->en_wmm = OAL_TRUE;
#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
            hcc_host_update_vi_flowctl_param(pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[WLAN_WME_AC_BE].ul_dot11QAPEDCATableCWmin,
            pst_hmac_sta->st_vap_base_info.pst_mib_info->st_wlan_mib_qap_edac[WLAN_WME_AC_VI].ul_dot11QAPEDCATableCWmin);
#endif
            /* ����EDCA��ص�MAC�Ĵ��� */
            hmac_sta_up_update_edca_params_machw(pst_hmac_sta, MAC_WMM_SET_PARAM_TYPE_UPDATE_EDCA);

            return;
        }
    }

    if (WLAN_FC0_SUBTYPE_ASSOC_RSP == uc_frame_sub_type)
    {
        /* ����STA������AP����QoS�ģ�STA��ȥʹ��EDCA�Ĵ�������Ĭ������VO���������� */
        ul_ret = hmac_sta_up_update_edca_params_machw(pst_hmac_sta, MAC_WMM_SET_PARAM_TYPE_DEFAULT);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                            "{hmac_sta_up_update_edca_params::hmac_sta_up_update_edca_params_machw failed[%d].}", ul_ret);
        }

        pst_mac_device->en_wmm = OAL_FALSE;
    }
}

/* TBD 1102��ʱע��STA TXOPPS����,���������ٴ�*/
#ifdef _PRE_WLAN_FEATURE_TXOPPS

oal_uint32  hmac_sta_set_txopps_partial_aid(mac_vap_stru *pst_mac_vap)
{
    oal_uint16              us_temp_aid;
    oal_uint8               uc_temp_bssid;
    oal_uint32               ul_ret;
    mac_cfg_txop_sta_stru    st_txop_info;

    /* �˴���Ҫע��:����Э��涨(802.11ac-2013.pdf,9.17a)��ap�����sta��aid��������
       ʹ���������partial aidΪ0���������ap֧�ֵ��������û���Ŀ����512������Ҫ
       ��aid������Ϸ��Լ��!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    */

    if (WLAN_VHT_MODE != pst_mac_vap->en_protocol && WLAN_VHT_ONLY_MODE != pst_mac_vap->en_protocol)
    {
        return OAL_SUCC;
    }


    us_temp_aid   = pst_mac_vap->us_sta_aid & 0x1FF;
    uc_temp_bssid = (pst_mac_vap->auc_bssid[5] & 0x0F) ^ ((pst_mac_vap->auc_bssid[5] & 0xF0) >> 4);
    st_txop_info.us_partial_aid = (us_temp_aid + (uc_temp_bssid << 5) ) & ((1 << 9) - 1);
    st_txop_info.en_protocol = pst_mac_vap->en_protocol;

    /***************************************************************************
    ���¼���DMAC��, ͬ��DMAC����
    ***************************************************************************/
    ul_ret = hmac_config_send_event(pst_mac_vap, WLAN_CFGID_STA_TXOP_AID, OAL_SIZEOF(mac_cfg_txop_sta_stru), (oal_uint8 *)&st_txop_info);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_TXOP, "{hmac_sta_set_txopps_partial_aid::hmac_config_send_event failed[%d].}", ul_ret);
    }

    return OAL_SUCC;
}
#endif


oal_void hmac_sta_update_mac_user_info(hmac_user_stru *pst_hmac_user_ap,oal_uint16 us_user_idx)
{
    mac_vap_stru                   *pst_mac_vap;
    mac_user_stru                  *pst_mac_user_ap;
    oal_uint32                      ul_ret;

    if (OAL_PTR_NULL == pst_hmac_user_ap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_RX, "{hmac_sta_update_mac_user_info::param null.}");
        return;
    }

    pst_mac_vap = (mac_vap_stru *)mac_res_get_mac_vap(pst_hmac_user_ap->st_user_base_info.uc_vap_id);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_mac_vap)) {
        OAM_ERROR_LOG1(0, OAM_SF_RX, "{hmac_sta_update_mac_user_info::get mac_vap [vap_id:%d] null.}", pst_hmac_user_ap->st_user_base_info.uc_vap_id);
        return;
    }

    pst_mac_user_ap = &(pst_hmac_user_ap->st_user_base_info);


    OAM_WARNING_LOG3(pst_mac_vap->uc_vap_id, OAM_SF_RX,
                       "{hmac_sta_update_mac_user_info::us_user_idx:%d,en_avail_bandwidth:%d,en_cur_bandwidth:%d}",
                         us_user_idx,
                         pst_mac_user_ap->en_avail_bandwidth,
                         pst_mac_user_ap->en_cur_bandwidth);

    ul_ret = hmac_config_user_info_syn(pst_mac_vap, pst_mac_user_ap);
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_RX,
                       "{hmac_sta_update_mac_user_info::hmac_config_user_info_syn failed[%d].}", ul_ret);
    }

    ul_ret = hmac_config_user_rate_info_syn(pst_mac_vap, pst_mac_user_ap);
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_RX,
                       "{hmac_sta_wait_asoc_rx::hmac_syn_rate_info failed[%d].}", ul_ret);
    }
    return;
}



oal_uint8 * hmac_sta_find_ie_in_probe_rsp(mac_vap_stru *pst_mac_vap, oal_uint8 uc_eid, oal_uint16 *pus_index)
{
    hmac_scanned_bss_info              *pst_scanned_bss_info;
    hmac_bss_mgmt_stru                 *pst_bss_mgmt;
    hmac_device_stru                   *pst_hmac_device;
    mac_bss_dscr_stru                  *pst_bss_dscr;
    oal_uint8                          *puc_ie;
    oal_uint8                          *puc_payload;
    oal_uint8                           us_offset;

    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG0(0, OAM_SF_SCAN, "{find ie fail, pst_mac_vap is null.}");
        return OAL_PTR_NULL;
    }

    /* ��ȡhmac device �ṹ */
    pst_hmac_device = hmac_res_get_mac_dev(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_hmac_device)
    {
        OAM_WARNING_LOG1(0, OAM_SF_SCAN, "{find ie fail, pst_hmac_device is null, dev id[%d].}", pst_mac_vap->uc_device_id);
        return OAL_PTR_NULL;
    }

    /* ��ȡ����ɨ���bss����Ľṹ�� */
    pst_bss_mgmt = &(pst_hmac_device->st_scan_mgmt.st_scan_record_mgmt.st_bss_mgmt);

    oal_spin_lock(&(pst_bss_mgmt->st_lock));

    pst_scanned_bss_info = hmac_scan_find_scanned_bss_by_bssid(pst_bss_mgmt, pst_mac_vap->auc_bssid);
    if (OAL_PTR_NULL == pst_scanned_bss_info)
    {
       OAM_WARNING_LOG4(pst_mac_vap->uc_vap_id, OAM_SF_CFG,
                        "{find the bss failed by bssid:%02X:XX:XX:%02X:%02X:%02X}",
                        pst_mac_vap->auc_bssid[0],
                        pst_mac_vap->auc_bssid[3],
                        pst_mac_vap->auc_bssid[4],
                        pst_mac_vap->auc_bssid[5]);

       /* ���� */
       oal_spin_unlock(&(pst_bss_mgmt->st_lock));
       return OAL_PTR_NULL;
    }

    pst_bss_dscr = &(pst_scanned_bss_info->st_bss_dscr_info);
    /* ���� */
    oal_spin_unlock(&(pst_bss_mgmt->st_lock));

    /* ��IE��ͷ��payload�����ع�������ʹ�� */
    us_offset = MAC_80211_FRAME_LEN + MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    /*lint -e416*/
    puc_payload = (oal_uint8*)(pst_bss_dscr->auc_mgmt_buff + us_offset);
    /*lint +e416*/
    if (pst_bss_dscr->ul_mgmt_len < us_offset)
    {
        return OAL_PTR_NULL;
    }

    puc_ie = mac_find_ie(uc_eid, puc_payload, (oal_int32)(pst_bss_dscr->ul_mgmt_len - us_offset));
    if (OAL_PTR_NULL == puc_ie)
    {
        return OAL_PTR_NULL;
    }

    /* IE���ȳ���У�� */
    if (0 == *(puc_ie + 1))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{IE[%d] len in probe rsp is 0, find ie fail.}", uc_eid);
        return OAL_PTR_NULL;
    }

    *pus_index = (oal_uint16)(puc_ie - puc_payload);

    OAM_WARNING_LOG1(0, OAM_SF_ANY, "{found ie[%d] in probe rsp.}", uc_eid);

    return puc_payload;
}


oal_bool_enum_uint8 hmac_is_ht_mcs_set_valid(oal_uint8 *puc_ht_cap_ie, wlan_channel_band_enum_uint8  en_band)
{
    if (g_ht_mcs_set_check == OAL_FALSE)
    {
        return OAL_TRUE;
    }

    /***************************************************************************
    -------------------------------------------------------------------------
    |EID |Length |HT Capa. Info |A-MPDU Parameters |Supported MCS Set|
    -------------------------------------------------------------------------
    |1   |1      |2             |1                 |16               |
    -------------------------------------------------------------------------
    |HT Extended Cap. |Transmit Beamforming Cap. |ASEL Cap.          |
    -------------------------------------------------------------------------
    |2                |4                         |1                  |
    -------------------------------------------------------------------------
    ***************************************************************************/
    if (OAL_PTR_NULL == puc_ht_cap_ie)
    {
        OAM_ERROR_LOG0(0,OAM_SF_ANY,"hmac_is_ht_mcs_set_valid:puc_ht_cap_ie is null");
        return OAL_FALSE;
    }

    if (puc_ht_cap_ie[1] < MAC_HT_CAP_LEN)
    {
        return OAL_FALSE;
    }

    if ((WLAN_BAND_2G == en_band) && IS_INVALID_HT_RATE_HP(puc_ht_cap_ie))
    {
        OAM_WARNING_LOG0(0,OAM_SF_ANY,"hmac_is_ht_mcs_set_valid:it is hp printer");
        return OAL_FALSE;
    }
    /* �������mcs_set��Ϊ0ʱ�ŷ���OAL_FALSE,��Ϊ���ڲ���APֻ֧�ֲ������ʵ���� */
    if ((0 == puc_ht_cap_ie[5]) && (0 == puc_ht_cap_ie[6])
        && (0 == puc_ht_cap_ie[7]) && (0 == puc_ht_cap_ie[8]))
    {
        OAM_WARNING_LOG0(0,OAM_SF_ANY,"hmac_is_ht_mcs_set_valid:all mcs_set is 0");
        return OAL_FALSE;
    }
    return OAL_TRUE;
}


oal_void hmac_sta_update_ht_cap(mac_vap_stru    *pst_mac_sta,
                                            oal_uint8       *puc_payload,
                                            mac_user_stru   *pst_mac_user_ap,
                                            oal_uint16      *pus_amsdu_maxsize,
                                            oal_uint16      us_payload_len)
{
    oal_uint8                          *puc_ie;
    oal_uint8                          *puc_payload_for_ht_cap_chk = OAL_PTR_NULL;
    oal_uint16                          us_ht_cap_index;
    oal_uint16                          us_ht_cap_info = 0;

    if ((OAL_PTR_NULL == pst_mac_sta) || (OAL_PTR_NULL == puc_payload) || (OAL_PTR_NULL == pst_mac_user_ap))
    {
        return;
    }

    puc_ie = mac_find_ie(MAC_EID_HT_CAP, puc_payload, us_payload_len);
    if (OAL_PTR_NULL == puc_ie || puc_ie[1] < MAC_HT_CAP_LEN)
    {
        puc_payload_for_ht_cap_chk = hmac_sta_find_ie_in_probe_rsp(pst_mac_sta, MAC_EID_HT_CAP, &us_ht_cap_index);
        if (OAL_PTR_NULL == puc_payload_for_ht_cap_chk)
        {
            OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hmac_sta_update_ht_cap::puc_payload_for_ht_cap_chk is null.}");
            return;
        }

        /*lint -e413*/
        if (puc_payload_for_ht_cap_chk[us_ht_cap_index + 1] < MAC_HT_CAP_LEN)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_sta_update_ht_cap::invalid ht cap len[%d].}", puc_payload_for_ht_cap_chk[us_ht_cap_index + 1]);
            return;
        }
        /*lint +e413*/
        puc_ie = puc_payload_for_ht_cap_chk + us_ht_cap_index;   /* ��ֵHT CAP IE */
    }
    else
    {
        if (puc_ie < puc_payload)
        {
            return;
        }
        us_ht_cap_index = (oal_uint16)(puc_ie - puc_payload);
        puc_payload_for_ht_cap_chk = puc_payload;
    }

    if (OAL_FALSE == hmac_is_ht_mcs_set_valid(puc_ie, pst_mac_sta->st_channel.en_band))
    {
        OAM_WARNING_LOG0(pst_mac_sta->uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_update_ht_cap:: invalid mcs set, disable HT.}");
        mac_user_set_ht_capable(pst_mac_user_ap, OAL_FALSE);
        return;
    }

    mac_user_set_ht_capable(pst_mac_user_ap, OAL_TRUE);

    /* ֧��HT, Ĭ�ϳ�ʼ�� */
    /* hmac_amsdu_init_user(pst_hmac_user_ap); */

    /* ����Э��ֵ�������ԣ�������hmac_amsdu_init_user������� */
    mac_ie_proc_ht_sta(pst_mac_sta, puc_payload_for_ht_cap_chk, &us_ht_cap_index, pst_mac_user_ap, &us_ht_cap_info, pus_amsdu_maxsize);

}


oal_void hmac_sta_update_ext_cap(mac_vap_stru    *pst_mac_sta,
                                    mac_user_stru   *pst_mac_user_ap,
                                    oal_uint8       *puc_payload,
                                    oal_uint16       us_rx_len)
{
    oal_uint8       *puc_ie;
    oal_uint8       *puc_payload_proc = OAL_PTR_NULL;
    oal_uint16       us_index;

    puc_ie = mac_find_ie(MAC_EID_EXT_CAPS, puc_payload, us_rx_len);
    if (OAL_PTR_NULL == puc_ie || puc_ie[1] < MAC_MIN_XCAPS_LEN)
    {
        puc_payload_proc = hmac_sta_find_ie_in_probe_rsp(pst_mac_sta, MAC_EID_EXT_CAPS, &us_index);
        if (OAL_PTR_NULL == puc_payload_proc)
        {
            return;
        }

        /*lint -e413*/
        if (puc_payload_proc[us_index + 1] < MAC_MIN_XCAPS_LEN)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_sta_update_ext_cap::invalid ext cap len[%d].}", puc_payload_proc[us_index + 1]);
            return;
        }
        /*lint +e413*/
    }
    else
    {
        puc_payload_proc = puc_payload;
        if (puc_ie < puc_payload)
        {
            return;
        }

        us_index = (oal_uint16)(puc_ie - puc_payload);
    }

    /* ���� Extended Capabilities IE */
    /*lint -e613*/
    mac_ie_proc_ext_cap_ie(pst_mac_user_ap, &puc_payload_proc[us_index]);
    /*lint +e613*/
}


oal_uint32 hmac_sta_update_ht_opern(mac_vap_stru    *pst_mac_sta,
                                    mac_user_stru   *pst_mac_user_ap,
                                    oal_uint8       *puc_payload,
                                    oal_uint16       us_rx_len)
{
    oal_uint8       *puc_ie;
    oal_uint8       *puc_payload_proc = OAL_PTR_NULL;
    oal_uint16       us_index;
    oal_uint32       ul_change = MAC_NO_CHANGE;

    puc_ie = mac_find_ie(MAC_EID_HT_OPERATION, puc_payload, us_rx_len);
    if (OAL_PTR_NULL == puc_ie || puc_ie[1] < MAC_HT_OPERN_LEN)
    {
        puc_payload_proc = hmac_sta_find_ie_in_probe_rsp(pst_mac_sta, MAC_EID_HT_OPERATION, &us_index);
        if (OAL_PTR_NULL == puc_payload_proc)
        {
            return ul_change;
        }

        /*lint -e413*/
        if (puc_payload_proc[us_index + 1] < MAC_HT_OPERN_LEN)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_sta_update_ht_opern::invalid ht cap len[%d].}", puc_payload_proc[us_index + 1]);
            return ul_change;
        }
        /*lint +e413*/
    }
    else
    {
        puc_payload_proc = puc_payload;
        if (puc_ie < puc_payload)
        {
            return ul_change;
        }

        us_index = (oal_uint16)(puc_ie - puc_payload);
    }

    ul_change |= mac_proc_ht_opern_ie(pst_mac_sta, &puc_payload_proc[us_index], pst_mac_user_ap);
    /* ֻ��� HT Operation�е�Secondary Channel Offset���д��� */
    ul_change |= mac_ie_proc_sec_chan_offset_2040(pst_mac_sta, puc_payload[us_index + MAC_IE_HDR_LEN + 1] & 0x3);

    return ul_change;
}


oal_uint32 hmac_update_ht_sta(mac_vap_stru    *pst_mac_sta,
                               oal_uint8       *puc_payload,
                               oal_uint16       us_offset,
                               oal_uint16       us_rx_len,
                               mac_user_stru   *pst_mac_user_ap,
                               oal_uint16      *pus_amsdu_maxsize)
{
    oal_uint32              ul_change = MAC_NO_CHANGE;
    oal_uint8              *puc_ie_payload_start;
    oal_uint16              us_ie_payload_len;

    if ((OAL_PTR_NULL == pst_mac_sta) || (OAL_PTR_NULL == puc_payload) || (OAL_PTR_NULL == pst_mac_user_ap))
    {
        return ul_change;
    }

    /* ��ʼ��HT capΪFALSE������ʱ��ѱ�����������AP���� */
    mac_user_set_ht_capable(pst_mac_user_ap, OAL_FALSE);

     /* ����֧��11n�Ž��к����Ĵ��� */
    if (OAL_FALSE == mac_mib_get_HighThroughputOptionImplemented(pst_mac_sta))
    {
        return ul_change;
    }

    puc_ie_payload_start = puc_payload + us_offset;
    us_ie_payload_len    = us_rx_len - us_offset;

    hmac_sta_update_ht_cap(pst_mac_sta, puc_ie_payload_start, pst_mac_user_ap, pus_amsdu_maxsize, us_ie_payload_len);

    hmac_sta_update_ext_cap(pst_mac_sta, pst_mac_user_ap, puc_ie_payload_start, us_ie_payload_len);

    ul_change = hmac_sta_update_ht_opern(pst_mac_sta, pst_mac_user_ap, puc_ie_payload_start, us_ie_payload_len);

    return ul_change;
}


oal_void hmac_add_and_clear_repeat_op_rates(oal_uint8 *puc_ie_rates, oal_uint8 uc_ie_num_rates, mac_rate_stru *pst_op_rates)
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
            if(WLAN_MAX_SUPP_RATES == pst_op_rates->uc_rs_nrates)
            {
                OAM_WARNING_LOG0(0, OAM_SF_ANY, "{hmac_add_and_clear_repeat_op_rates::user option rates more then WLAN_USER_MAX_SUPP_RATES.}");
                break;
            }
            pst_op_rates->auc_rs_rates[pst_op_rates->uc_rs_nrates++] = puc_ie_rates[uc_ie_rates_idx];
        }
    }
}



OAL_STATIC oal_uint32 hmac_ie_proc_assoc_user_legacy_rate(
                                                                oal_uint8           *puc_payload,
                                                                oal_uint16           us_offset,
                                                                oal_uint16           us_rx_len,
                                                                hmac_user_stru      *pst_hmac_user)
{
    oal_uint8   *puc_ie;
    oal_uint8    uc_num_rates    = 0;
    oal_uint8    uc_num_ex_rates = 0;

    //puc_ie = mac_get_supported_rates_ie(puc_payload, us_rx_len,us_offset);
    puc_ie = mac_find_ie(MAC_EID_RATES, puc_payload + us_offset, us_rx_len - us_offset);
    if (OAL_PTR_NULL != puc_ie)
    {
        uc_num_rates = puc_ie[1];

        if(uc_num_rates < MAC_MIN_RATES_LEN)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_ie_proc_assoc_user_legacy_rate:: invaild rates:%d}", uc_num_rates);
        }
        else
        {
            hmac_add_and_clear_repeat_op_rates(puc_ie + MAC_IE_HDR_LEN, uc_num_rates, &(pst_hmac_user->st_op_rates));
        }
    }
    else
    {
        OAM_WARNING_LOG0(0, OAM_SF_ANY,"{hmac_ie_proc_assoc_user_legacy_rate::unsupport basic rates}");
    }
    //puc_ie = mac_get_exsup_rates_ie(puc_payload, us_rx_len, us_offset);
    puc_ie = mac_find_ie(MAC_EID_XRATES, puc_payload + us_offset, us_rx_len - us_offset);
    if (OAL_PTR_NULL != puc_ie)
    {
        uc_num_ex_rates = puc_ie[1];

        if (uc_num_ex_rates < MAC_MIN_XRATE_LEN)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_ie_proc_assoc_user_legacy_rate:: invaild xrates:%d}", uc_num_ex_rates);
            return OAL_EFAIL;
        }
        else
        {
            hmac_add_and_clear_repeat_op_rates(puc_ie + MAC_IE_HDR_LEN, uc_num_ex_rates, &(pst_hmac_user->st_op_rates));
        }

    }

    if (0 == pst_hmac_user->st_op_rates.uc_rs_nrates)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_ie_proc_assoc_user_legacy_rate::rate is 0.}");
        return OAL_FAIL;
    }
    return OAL_SUCC;
}

oal_uint32 hmac_process_assoc_rsp(hmac_vap_stru *pst_hmac_sta, hmac_user_stru *pst_hmac_user, oal_uint8 *puc_mac_hdr, oal_uint8 *puc_payload, oal_uint16 us_msg_len)
{
    oal_uint32                      ul_rslt;
    oal_uint16                      us_offset;
    oal_uint16                      us_aid;
    oal_uint8                      *puc_tmp_ie;
    wlan_bw_cap_enum_uint8          en_bandwidth_cap;
    oal_uint32                      ul_ret;
    mac_vap_stru                   *pst_mac_vap;
    wlan_bw_cap_enum_uint8          en_bwcap;
    oal_uint32                      ul_change = MAC_NO_CHANGE;
    oal_uint8                      *puc_vendor_vht_ie;
    oal_uint32                      ul_vendor_vht_ie_offset = MAC_WLAN_OUI_VENDOR_VHT_HEADER + MAC_IE_HDR_LEN;

    pst_mac_vap     = &(pst_hmac_sta->st_vap_base_info);
    /* ���¹���ID */
    us_aid = mac_get_asoc_id(puc_payload);
    if ((us_aid > 0) && (us_aid <= 2007))
    {
        mac_vap_set_aid(&pst_hmac_sta->st_vap_base_info, us_aid);
/* TBD 1102��ʱע��STA TXOPPS����,���������ٴ�*/
#ifdef _PRE_WLAN_FEATURE_TXOPPS
        /* sta����������partial aid�����浽vap�ṹ�У���д�뵽mac�Ĵ��� */
        hmac_sta_set_txopps_partial_aid(&pst_hmac_sta->st_vap_base_info);
#endif
    }
    else
    {
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                        "{hmac_process_assoc_rsp::invalid us_sta_aid[%d].}", us_aid);
    }
    us_offset =  MAC_CAP_INFO_LEN + MAC_STATUS_CODE_LEN + MAC_AID_LEN;

    /* ��ʼ����ȫ�˿ڹ��˲��� */
#if defined (_PRE_WLAN_FEATURE_WPA) || defined(_PRE_WLAN_FEATURE_WPA2)
    ul_rslt = hmac_init_user_security_port(&(pst_hmac_sta->st_vap_base_info), &(pst_hmac_user->st_user_base_info));
    if (OAL_SUCC != ul_rslt)
    {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_process_assoc_rsp::hmac_init_user_security_port failed[%d].}", ul_rslt);
    }

#endif

#if (_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT)
    /* STAģʽ�µ�pmf������Դ��WPA_supplicant��ֻ������pmf�Ͳ�����pmf�������� */
    mac_user_set_pmf_active(&pst_hmac_user->st_user_base_info, pst_mac_vap->en_user_pmf_cap);
#endif /* #if(_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT) */

    /* sta����������edca parameters */
    hmac_sta_up_update_edca_params(puc_payload, us_msg_len, us_offset, pst_hmac_sta, mac_get_frame_sub_type(puc_mac_hdr), pst_hmac_user);

    /* ���¹����û��� QoS protocol table */
    hmac_mgmt_update_assoc_user_qos_table(puc_payload, us_msg_len, us_offset, pst_hmac_user);

    /* ���¹����û���legacy���ʼ��� */
    hmac_user_init_rates(pst_hmac_user, &pst_hmac_sta->st_vap_base_info);
    hmac_ie_proc_assoc_user_legacy_rate(puc_payload, us_offset, us_msg_len, pst_hmac_user);

    /* ��ʼ�����öԶ� HT ������ʹ�� */
    mac_user_set_ht_capable(&(pst_hmac_user->st_user_base_info), OAL_FALSE);
    /* ��ʼ�����öԶ�VHT ������ʹ�� */
    mac_user_set_vht_capable(&(pst_hmac_user->st_user_base_info), OAL_FALSE);
    /* ���� HT ����  */
    ul_change |= hmac_update_ht_sta(&pst_hmac_sta->st_vap_base_info, puc_payload, us_offset, us_msg_len, &pst_hmac_user->st_user_base_info, &pst_hmac_user->us_amsdu_maxsize);
    if (OAL_TRUE == hmac_user_ht_support(pst_hmac_user))
    {
#ifdef _PRE_WLAN_FEATURE_TXBF
        /* ����11n txbf���� */
        puc_tmp_ie = mac_find_vendor_ie(MAC_HUAWEI_VENDER_IE, MAC_EID_11NTXBF, puc_payload + us_offset, us_msg_len - us_offset);
        hmac_mgmt_update_11ntxbf_cap(puc_tmp_ie, pst_hmac_user);
#endif

        /* ����11ac VHT capabilities ie */
        oal_memset(&(pst_hmac_user->st_user_base_info.st_vht_hdl), 0, OAL_SIZEOF(mac_vht_hdl_stru));
        puc_tmp_ie = mac_find_ie(MAC_EID_VHT_CAP, puc_payload + us_offset, us_msg_len - us_offset);
        if (OAL_PTR_NULL != puc_tmp_ie)
        {
            hmac_proc_vht_cap_ie(pst_mac_vap, pst_hmac_user, puc_tmp_ie);
        }

        /* ����11ac VHT operation ie */
        puc_tmp_ie = mac_find_ie(MAC_EID_VHT_OPERN, puc_payload + us_offset, us_msg_len - us_offset);
        if (OAL_PTR_NULL != puc_tmp_ie)
        {
            ul_change |= hmac_update_vht_opern_ie_sta(pst_mac_vap, pst_hmac_user, puc_tmp_ie, us_offset);
        }

        /* ����BRCM VENDOR OUI ����2G 11AC */
        if (pst_hmac_user->st_user_base_info.st_vht_hdl.en_vht_capable == OAL_FALSE)
        {
            puc_vendor_vht_ie = mac_find_vendor_ie(MAC_WLAN_OUI_BROADCOM_EPIGRAM, MAC_WLAN_OUI_VENDOR_VHT_TYPE, puc_payload + us_offset, us_msg_len - us_offset);

            if ((OAL_PTR_NULL != puc_vendor_vht_ie) && (puc_vendor_vht_ie[1] >= ul_vendor_vht_ie_offset))
            {
                OAM_WARNING_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                    "{hmac_process_assoc_rsp::find broadcom/epigram vendor ie, enable hidden bit_11ac2g}");

                /* ����˺�������user֧��2G 11ac */
                puc_tmp_ie = mac_find_ie(MAC_EID_VHT_CAP, puc_vendor_vht_ie + ul_vendor_vht_ie_offset, (oal_int32)(puc_vendor_vht_ie[1] - MAC_WLAN_OUI_VENDOR_VHT_HEADER));
                if (OAL_PTR_NULL != puc_tmp_ie)
                {
                    hmac_proc_vht_cap_ie(pst_mac_vap, pst_hmac_user, puc_tmp_ie);
                }

                /* ����11ac VHT operation ie */
                puc_tmp_ie = mac_find_ie(MAC_EID_VHT_OPERN, puc_vendor_vht_ie + ul_vendor_vht_ie_offset, (oal_int32)(puc_vendor_vht_ie[1] - MAC_WLAN_OUI_VENDOR_VHT_HEADER));
                if (OAL_PTR_NULL != puc_tmp_ie)
                {
                    ul_change |= hmac_update_vht_opern_ie_sta(pst_mac_vap, pst_hmac_user, puc_tmp_ie,0);
                }
            }
        }

        /* �����Ƿ���Ҫ���д����л� */
        //hmac_chan_reval_bandwidth_sta(pst_mac_vap, ul_change);
        if (MAC_BW_CHANGE & ul_change)
        {
            OAM_WARNING_LOG3(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                           "{hmac_process_assoc_rsp::change BW. ul_change[0x%x], uc_channel[%d], en_bandwidth[%d].}",
                           ul_change,
                           pst_mac_vap->st_channel.uc_chan_number,
                           pst_mac_vap->st_channel.en_bandwidth);
            hmac_chan_sync(pst_mac_vap,
                            pst_mac_vap->st_channel.uc_chan_number,
                            pst_mac_vap->st_channel.en_bandwidth,
                            OAL_TRUE);
        }
    }
#ifdef _PRE_WLAN_FEATURE_HISTREAM
    /* ����histream���� */
    pst_hmac_user->st_user_base_info.st_cap_info.bit_histream_cap = OAL_FALSE;
    puc_tmp_ie = mac_find_vendor_ie(MAC_HUAWEI_VENDER_IE, MAC_HISI_HISTREAM_IE, puc_payload + us_offset, us_msg_len - us_offset);
    if (OAL_PTR_NULL != puc_tmp_ie)
    {
        pst_hmac_user->st_user_base_info.st_cap_info.bit_histream_cap = OAL_TRUE;
    }
#endif //_PRE_WLAN_FEATURE_HISTREAM

    /* ��ȡ�û���Э��ģʽ */
    hmac_set_user_protocol_mode(pst_mac_vap, pst_hmac_user);

    if (WLAN_BAND_2G == pst_mac_vap->st_channel.en_band)
    {
        pst_hmac_sta->en_rx_ampduplusamsdu_active = OAL_FALSE;
    }
    else
    {
        pst_hmac_sta->en_rx_ampduplusamsdu_active = g_uc_host_rx_ampdu_amsdu;
    }

    /* ��ȡ�û���VAPЭ��ģʽ���� */
    mac_user_set_avail_protocol_mode(&pst_hmac_user->st_user_base_info, g_auc_avail_protocol_mode[pst_mac_vap->en_protocol][pst_hmac_user->st_user_base_info.en_protocol_mode]);
    mac_user_set_cur_protocol_mode(&pst_hmac_user->st_user_base_info, pst_hmac_user->st_user_base_info.en_avail_protocol_mode);

    /* ��ȡ�û���VAP ��֧�ֵ�11a/b/g ���ʽ��� */
    hmac_vap_set_user_avail_rates(&(pst_hmac_sta->st_vap_base_info), pst_hmac_user);

    /* ��ȡ�û���VAP������������ */
    /* ��ȡ�û��Ĵ������� */
    mac_user_get_ap_opern_bandwidth(&(pst_hmac_user->st_user_base_info), &en_bandwidth_cap);

    mac_vap_get_bandwidth_cap(pst_mac_vap, &en_bwcap);
    en_bwcap = OAL_MIN(en_bwcap, en_bandwidth_cap);
    mac_user_set_bandwidth_info(&pst_hmac_user->st_user_base_info, en_bwcap, en_bwcap);

    OAM_WARNING_LOG3(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_process_assoc_rsp::mac user[%d] en_bandwidth_cap:%d,en_avail_bandwidth:%d}",
                       pst_hmac_user->st_user_base_info.us_assoc_id,
                       en_bandwidth_cap,
                       pst_hmac_user->st_user_base_info.en_avail_bandwidth);

    /* ��ȡ�û���VAP�ռ������� */
    ul_ret = hmac_user_set_avail_num_space_stream(&(pst_hmac_user->st_user_base_info), pst_hmac_sta->st_vap_base_info.en_vap_rx_nss);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_process_assoc_rsp::mac_user_set_avail_num_space_stream failed[%d].}", ul_ret);
    }

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY

    /* ����Operating Mode Notification ��ϢԪ�� */
    ul_ret = hmac_check_opmode_notify(pst_hmac_sta, puc_mac_hdr, puc_payload, us_offset, us_msg_len, pst_hmac_user);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_process_assoc_rsp::hmac_check_opmode_notify failed[%d].}", ul_ret);
    }

#endif

    mac_user_set_asoc_state(&pst_hmac_user->st_user_base_info, MAC_USER_STATE_ASSOC);

    /* dmac offload�ܹ��£�ͬ��STA USR��Ϣ��dmac */
    ul_ret = hmac_config_user_cap_syn(&(pst_hmac_sta->st_vap_base_info), &pst_hmac_user->st_user_base_info);
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_process_assoc_rsp::hmac_config_usr_cap_syn failed[%d].}", ul_ret);
    }

    ul_ret = hmac_config_user_info_syn(&(pst_hmac_sta->st_vap_base_info), &pst_hmac_user->st_user_base_info);
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_process_assoc_rsp::hmac_syn_vap_state failed[%d].}", ul_ret);
    }

    ul_ret = hmac_config_user_rate_info_syn(&(pst_hmac_sta->st_vap_base_info), &pst_hmac_user->st_user_base_info);
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_process_assoc_rsp::hmac_syn_rate_info failed[%d].}", ul_ret);
    }

    /* dmac offload�ܹ��£�ͬ��STA USR��Ϣ��dmac */
    ul_ret = hmac_config_sta_vap_info_syn(&(pst_hmac_sta->st_vap_base_info));
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                       "{hmac_process_assoc_rsp::hmac_syn_vap_state failed[%d].}", ul_ret);
    }

    return OAL_SUCC;
}
#ifndef _PRE_WLAN_FEATURE_P2P

OAL_STATIC oal_uint32 hmac_sta_sync_bss_freq(hmac_vap_stru *pst_hmac_vap, mac_channel_stru *pst_channel, wlan_protocol_enum_uint8 en_protocol)
{
    if (OAL_SUCC == hmac_ap_clean_bss(pst_hmac_vap))
    {
        /* ����AP��״̬��Ϊ WAIT_START */
        hmac_fsm_change_state(pst_hmac_vap, MAC_VAP_STATE_AP_WAIT_START);

        return hmac_chan_start_bss(pst_hmac_vap, pst_channel, en_protocol);
    }

    return OAL_FAIL;
}

OAL_STATIC oal_uint32 hmac_sta_sync_bss_freq_all(hmac_vap_stru *pst_hmac_sta)
{
    mac_device_stru *pst_mac_dev;
    hmac_vap_stru   *pst_hmac_vap;
    oal_uint8        uc_idx;

    mac_channel_stru *pst_channel = &pst_hmac_sta->st_vap_base_info.st_channel;

    pst_mac_dev = mac_res_get_dev(pst_hmac_sta->st_vap_base_info.uc_device_id);
    if (!pst_mac_dev)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ASSOC, "{hmac_sta_sync_bss_freq_all::null pst_mac_dev}");
        return OAL_ERR_CODE_PTR_NULL;
    }

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    if (mac_is_proxysta_enabled(pst_mac_dev))
    {
        // channel sync is handled by proxysta itself
        return OAL_SUCC;
    }
#endif

    for (uc_idx = 0; uc_idx < pst_mac_dev->uc_vap_num; uc_idx++)
    {
        pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_dev->auc_vap_id[uc_idx]);
        if (!pst_hmac_vap || (pst_hmac_vap == pst_hmac_sta) || (WLAN_VAP_MODE_BSS_AP != pst_hmac_vap->st_vap_base_info.en_vap_mode))
        {
            continue;
        }

        if (MAC_VAP_STATE_INIT == pst_hmac_vap->st_vap_base_info.en_vap_state)
        {
            continue;
        }

        // sync channel if on same band while channel or bandwidth not same
        if ((pst_hmac_vap->st_vap_base_info.st_channel.en_band == pst_channel->en_band)
        && ((pst_hmac_vap->st_vap_base_info.st_channel.en_bandwidth != pst_channel->en_bandwidth)
            || (pst_hmac_vap->st_vap_base_info.st_channel.uc_chan_number != pst_channel->uc_chan_number)))
        {
            if (OAL_SUCC != hmac_sta_sync_bss_freq(pst_hmac_vap, pst_channel, pst_hmac_sta->st_vap_base_info.en_protocol))
            {
                OAM_WARNING_LOG1(0, OAM_SF_ASSOC, "{hmac_sta_sync_bss_freq_all::sync vap%d failed}", pst_mac_dev->auc_vap_id[uc_idx]);
            }
        }
    }

    return OAL_SUCC;
}
#endif

oal_uint32 hmac_sta_wait_asoc_rx(hmac_vap_stru *pst_hmac_sta, oal_void *pst_msg)
{
    mac_status_code_enum_uint16     en_asoc_status;
    oal_uint8                       uc_frame_sub_type;
    dmac_wlan_crx_event_stru       *pst_mgmt_rx_event;
    dmac_rx_ctl_stru               *pst_rx_ctrl;
    mac_rx_ctl_stru                *pst_rx_info;
    oal_uint8                      *puc_mac_hdr;
    oal_uint8                      *puc_payload;
    oal_uint16                      us_msg_len;
    hmac_asoc_rsp_stru              st_asoc_rsp;
    oal_uint8                       auc_addr_sa[WLAN_MAC_ADDR_LEN];
    oal_uint16                      us_user_idx;
    oal_uint32                      ul_rslt;
    hmac_user_stru                 *pst_hmac_user_ap;
    oal_uint32                      ul_ret;
    mac_vap_stru                   *pst_mac_vap;
    mac_cfg_user_info_param_stru    st_hmac_user_info_event;

    if ((OAL_PTR_NULL == pst_hmac_sta) || (OAL_PTR_NULL == pst_msg))
    {
        OAM_ERROR_LOG2(0, OAM_SF_ASSOC, "{hmac_sta_wait_asoc_rx::param null,%d %d.}", pst_hmac_sta, pst_msg);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap     = &(pst_hmac_sta->st_vap_base_info);

    pst_mgmt_rx_event   = (dmac_wlan_crx_event_stru *)pst_msg;
    pst_rx_ctrl         = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_mgmt_rx_event->pst_netbuf);
    pst_rx_info         = (mac_rx_ctl_stru *)(&(pst_rx_ctrl->st_rx_info));
    puc_mac_hdr         = (oal_uint8 *)(pst_rx_info->pul_mac_hdr_start_addr);
    puc_payload         = (oal_uint8 *)(puc_mac_hdr) + pst_rx_info->uc_mac_header_len;
    us_msg_len          = pst_rx_info->us_frame_len - pst_rx_info->uc_mac_header_len;   /* ��Ϣ�ܳ���,������FCS */

    uc_frame_sub_type = mac_get_frame_sub_type(puc_mac_hdr);
    en_asoc_status    = mac_get_asoc_status(puc_payload);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
    if(OAL_TRUE == wlan_pm_wkup_src_debug_get())
    {
        wlan_pm_wkup_src_debug_set(OAL_FALSE);
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{wifi_wake_src:hmac_sta_wait_asoc_rx_etc::wakeup mgmt type[0x%x]}",uc_frame_sub_type);
    }
#endif
#endif

    if (WLAN_FC0_SUBTYPE_ASSOC_RSP      != uc_frame_sub_type &&
        WLAN_FC0_SUBTYPE_REASSOC_RSP    != uc_frame_sub_type)
    {
        return OAL_FAIL;
    }

#ifdef _PRE_WLAN_FEATURE_SNIFFER
    proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_info->us_frame_len, 0);
#endif
    if (MAC_SUCCESSFUL_STATUSCODE != en_asoc_status)
    {
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                        "{hmac_sta_wait_asoc_rx:: AP refuse STA assoc reason=%d.}", en_asoc_status);
        if (MAC_AP_FULL != en_asoc_status)
        {
            CHR_EXCEPTION(CHR_WIFI_DRV(CHR_WIFI_DRV_EVENT_CONNECT,CHR_WIFI_DRV_ERROR_ASSOC_REJECTED));
        }

        pst_hmac_sta->st_mgmt_timetout_param.en_status_code = en_asoc_status;

        return OAL_FAIL;
    }

    if (us_msg_len < OAL_ASSOC_RSP_FIXED_OFFSET)
    {
        OAM_ERROR_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC, "{hmac_sta_wait_asoc_rx::asoc_rsp_body is too short(%d) to going on!}",
                    us_msg_len);
        return OAL_FAIL;
    }

    /* ��ȡSA ��ַ */
    mac_get_address2(puc_mac_hdr, auc_addr_sa);

    /* ����SA �ҵ���ӦAP USER�ṹ */
    ul_rslt = mac_vap_find_user_by_macaddr(&(pst_hmac_sta->st_vap_base_info), auc_addr_sa, &us_user_idx);

    if (OAL_SUCC != ul_rslt)
    {
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                        "{hmac_sta_wait_asoc_rx:: mac_vap_find_user_by_macaddr failed[%d].}", ul_rslt);

        return ul_rslt;
    }

    /* ��ȡSTA������AP���û�ָ�� */
    pst_hmac_user_ap = mac_res_get_hmac_user(us_user_idx);
    if (OAL_PTR_NULL == pst_hmac_user_ap)
    {
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_AUTH, "{hmac_sta_wait_asoc_rx::pst_hmac_user_ap[%d] null.}", us_user_idx);
        return OAL_FAIL;
    }

    /* ȡ����ʱ�� */
    FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&(pst_hmac_sta->st_mgmt_timer));

    ul_ret = hmac_process_assoc_rsp(pst_hmac_sta, pst_hmac_user_ap, puc_mac_hdr, puc_payload, us_msg_len);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_ASSOC,
                         "{hmac_sta_wait_asoc_rx::hmac_process_assoc_rsp failed[%d].}", ul_ret);
        return ul_ret;
    }
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    /* �������û�֮�󣬳�ʼ������������ */
    hmac_btcoex_blacklist_handle_init(pst_hmac_user_ap);

    if (OAL_TRUE == hmac_btcoex_check_exception_in_list(pst_hmac_sta, auc_addr_sa))
    {
        if(BTCOEX_BLACKLIST_TPYE_FIX_BASIZE == HMAC_BTCOEX_GET_BLACKLIST_TYPE(pst_hmac_user_ap))
        {
            OAM_WARNING_LOG0(pst_hmac_sta->st_vap_base_info.uc_vap_id, OAM_SF_COEX,
                       "{hmac_sta_wait_asoc_rx::mac_addr in blacklist.}");
            pst_hmac_user_ap->st_hmac_user_btcoex.st_hmac_btcoex_addba_req.en_ba_handle_allow = OAL_FALSE;
        }
        else
        {
            pst_hmac_user_ap->st_hmac_user_btcoex.st_hmac_btcoex_addba_req.en_ba_handle_allow = OAL_TRUE;
        }
    }
    else
    {
        /* ��ʼ���������ۺϣ������������ֶ��� */
        pst_hmac_user_ap->st_hmac_user_btcoex.st_hmac_btcoex_addba_req.en_ba_handle_allow = OAL_TRUE;
    }
#endif

    /* STA�л���UP״̬ */
    hmac_fsm_change_state(pst_hmac_sta, MAC_VAP_STATE_UP);

    /* user�Ѿ������ϣ����¼���DMAC����DMAC����û��㷨���� */
    hmac_user_add_notify_alg(&(pst_hmac_sta->st_vap_base_info), us_user_idx);

#ifdef _PRE_WLAN_FEATURE_ROAM
    hmac_roam_info_init(pst_hmac_sta);
#endif //_PRE_WLAN_FEATURE_ROAM

    /* ���û�(AP)�ڱ��ص�״̬��Ϣ����Ϊ�ѹ���״̬ */
#ifdef _PRE_DEBUG_MODE_USER_TRACK
    mac_user_change_info_event(pst_hmac_user_ap->st_user_base_info.auc_user_mac_addr,
                               pst_hmac_sta->st_vap_base_info.uc_vap_id,
                               pst_hmac_user_ap->st_user_base_info.en_user_asoc_state,
                               MAC_USER_STATE_ASSOC, OAM_MODULE_ID_HMAC,
                               OAM_USER_INFO_CHANGE_TYPE_ASSOC_STATE);
#endif

    /* ��80211��������֡���أ��۲�������̣������ɹ��˾͹ر� */
    hmac_config_set_mgmt_log(&pst_hmac_sta->st_vap_base_info, &pst_hmac_user_ap->st_user_base_info, OAL_FALSE);

    /* ׼����Ϣ���ϱ���APP */
    st_asoc_rsp.en_result_code = HMAC_MGMT_SUCCESS;
    st_asoc_rsp.en_status_code = MAC_SUCCESSFUL_STATUSCODE;

    /* ��¼������Ӧ֡�Ĳ������ݣ������ϱ����ں� */
    st_asoc_rsp.ul_asoc_rsp_ie_len = us_msg_len - OAL_ASSOC_RSP_FIXED_OFFSET;  /* ��ȥMAC֡ͷ24�ֽں�FIXED����6�ֽ� */
    st_asoc_rsp.puc_asoc_rsp_ie_buff = puc_mac_hdr + OAL_ASSOC_RSP_IE_OFFSET;

    /* ��ȡAP��mac��ַ */
    mac_get_bssid(puc_mac_hdr, st_asoc_rsp.auc_addr_ap);

    /* ��ȡ��������֡��Ϣ */
    st_asoc_rsp.puc_asoc_req_ie_buff = pst_hmac_sta->puc_asoc_req_ie_buff;
    st_asoc_rsp.ul_asoc_req_ie_len   = pst_hmac_sta->ul_asoc_req_ie_len;

    hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_ASOC_RSP, (oal_uint8 *)(&st_asoc_rsp));

    /* 1102 STA �������ϱ�VAP ��Ϣ���û���Ϣ */
    st_hmac_user_info_event.us_user_idx = us_user_idx;

    hmac_config_vap_info(pst_mac_vap, OAL_SIZEOF(oal_uint32), (oal_uint8 *)&ul_ret);
    hmac_config_user_info(pst_mac_vap, OAL_SIZEOF(mac_cfg_user_info_param_stru), (oal_uint8 *)&st_hmac_user_info_event);

    // will not do any sync if proxysta enabled
    // allow running DBAC on different channels of same band while P2P defined
#ifndef _PRE_WLAN_FEATURE_P2P
    hmac_sta_sync_bss_freq_all(pst_hmac_sta);
#endif

    /*��Ҫ����proxystaģʽ�£�MAIN STA�����ɹ���UP��������ǰ�汻DOWN����AP����proxystaģʽ�¸ú�����ֱ�ӷ��سɹ� */
#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    hmac_psta_proc_join_result(pst_hmac_sta, OAL_TRUE);
#endif

    return OAL_SUCC;
}


oal_uint32  hmac_sta_auth_timeout(hmac_vap_stru *pst_hmac_sta, oal_void *p_param)
{
    hmac_auth_rsp_stru            st_auth_rsp = {{0,},};

    /* and send it to the host.                                          */
    st_auth_rsp.us_status_code = HMAC_MGMT_TIMEOUT;

    /* Send the response to host now. */
    hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_AUTH_RSP, (oal_uint8 *)&st_auth_rsp);

    return OAL_SUCC;
}


wlan_channel_bandwidth_enum_uint8 hmac_sta_get_band(wlan_bw_cap_enum_uint8 en_dev_cap, wlan_channel_bandwidth_enum_uint8 en_bss_cap)
{
    wlan_channel_bandwidth_enum_uint8       en_band;

    en_band = WLAN_BAND_WIDTH_20M;

    if((WLAN_BW_CAP_80M == en_dev_cap) &&
       (WLAN_BAND_WIDTH_80PLUSPLUS <= en_bss_cap))
    {
        /* ���AP��STAUT��֧��80M��������ΪAPһ�� */
        en_band = en_bss_cap;
        return en_band;
    }

    switch (en_bss_cap)
    {
        case WLAN_BAND_WIDTH_40PLUS:
        case WLAN_BAND_WIDTH_80PLUSPLUS:
        case WLAN_BAND_WIDTH_80PLUSMINUS:
            if(WLAN_BW_CAP_40M <= en_dev_cap)
            {
                en_band = WLAN_BAND_WIDTH_40PLUS;
            }
            break;

        case WLAN_BAND_WIDTH_40MINUS:
        case WLAN_BAND_WIDTH_80MINUSPLUS:
        case WLAN_BAND_WIDTH_80MINUSMINUS:
            if(WLAN_BW_CAP_40M <= en_dev_cap)
            {
                en_band = WLAN_BAND_WIDTH_40MINUS;
            }
            break;

        default:
            en_band = WLAN_BAND_WIDTH_20M;
            break;
    }

    return en_band;
}


OAL_STATIC oal_void hmac_sta_update_join_bss_info(mac_bss_dscr_stru *pst_bss_dscr)
{
     /* 165 �ŵ�ֻ����20MHz ���� */
     if ((pst_bss_dscr->st_channel.uc_chan_number == 165) && (pst_bss_dscr->en_channel_bandwidth != WLAN_BAND_WIDTH_20M))
     {
          OAM_WARNING_LOG4(0, OAM_SF_ASSOC, "{hmac_sta_update_join_bss_info::BSS [XX:XX:XX:XX:%02X:%02X] set wrong bandwidth [%d] at channel [%d]}",
                           pst_bss_dscr->auc_bssid[4],
                           pst_bss_dscr->auc_bssid[5],
                           pst_bss_dscr->en_channel_bandwidth,
                           pst_bss_dscr->st_channel.uc_chan_number);

          pst_bss_dscr->st_channel.en_bandwidth = WLAN_BAND_WIDTH_20M;
          pst_bss_dscr->en_channel_bandwidth    = WLAN_BAND_WIDTH_20M;
          pst_bss_dscr->en_bw_cap               = WLAN_BW_CAP_20M;
     }

     return;
}




oal_uint32 hmac_sta_update_join_req_params(hmac_vap_stru *pst_hmac_vap,hmac_join_req_stru *pst_join_req)
{
    mac_vap_stru                   *pst_mac_vap;
    dmac_ctx_join_req_set_reg_stru *pst_reg_params;
    frw_event_mem_stru             *pst_event_mem;
    frw_event_stru                 *pst_event;
    oal_uint32                      ul_ret;
    mac_device_stru                *pst_mac_device;
    wlan_mib_ieee802dot11_stru     *pst_mib_info;
    mac_cfg_mode_param_stru         st_cfg_mode;

    pst_mac_vap      = &(pst_hmac_vap->st_vap_base_info);
    pst_mib_info     = pst_mac_vap->pst_mib_info;
    if (OAL_PTR_NULL == pst_mib_info)
    {
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_device = mac_res_get_dev(pst_mac_vap->uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        return OAL_ERR_CODE_MAC_DEVICE_NULL;
    }

    /* ����BSSID */
    mac_vap_set_bssid(pst_mac_vap, pst_join_req->st_bss_dscr.auc_bssid);

    /* ����mib���Ӧ��dot11BeaconPeriodֵ */
    pst_mib_info->st_wlan_mib_sta_config.ul_dot11BeaconPeriod = (oal_uint32)(pst_join_req->st_bss_dscr.us_beacon_period);


    /* ����mib���Ӧ��ul_dot11CurrentChannelֵ*/
    mac_vap_set_current_channel(pst_mac_vap, pst_join_req->st_bss_dscr.st_channel.en_band, pst_join_req->st_bss_dscr.st_channel.uc_chan_number);

#ifdef _PRE_WLAN_FEATURE_11D
    /* ����sta��������Ĺ����ַ��� */
    pst_hmac_vap->ac_desired_country[0] = pst_join_req->st_bss_dscr.ac_country[0];
    pst_hmac_vap->ac_desired_country[1] = pst_join_req->st_bss_dscr.ac_country[1];
    pst_hmac_vap->ac_desired_country[2] = pst_join_req->st_bss_dscr.ac_country[2];
#endif

    /* ����mib���Ӧ��ssid */
    oal_memcopy(pst_mib_info->st_wlan_mib_sta_config.auc_dot11DesiredSSID, pst_join_req->st_bss_dscr.ac_ssid, WLAN_SSID_MAX_LEN);
    pst_mib_info->st_wlan_mib_sta_config.auc_dot11DesiredSSID[WLAN_SSID_MAX_LEN - 1] = '\0';

    hmac_sta_update_join_bss_info(&(pst_join_req->st_bss_dscr));


    /* ����Ƶ������20MHz�ŵ��ţ���APͨ�� DMAC�л��ŵ�ʱֱ�ӵ��� */
    pst_mac_vap->st_channel.en_bandwidth   = hmac_sta_get_band(pst_mac_device->en_bandwidth_cap, pst_join_req->st_bss_dscr.en_channel_bandwidth);
    pst_mac_vap->st_channel.uc_chan_number = pst_join_req->st_bss_dscr.st_channel.uc_chan_number;
    pst_mac_vap->st_channel.en_band        = pst_join_req->st_bss_dscr.st_channel.en_band;

    /* ��STAδ����Э��ģʽ����£�����Ҫ������AP������mib���ж�Ӧ��HT/VHT���� */
    if (OAL_SWITCH_OFF == pst_hmac_vap->bit_sta_protocol_cfg)
    {
        oal_memset(&st_cfg_mode, 0, OAL_SIZEOF(mac_cfg_mode_param_stru));

        //l00311403TODO
        pst_mib_info->st_wlan_mib_sta_config.en_dot11HighThroughputOptionImplemented = pst_join_req->st_bss_dscr.en_ht_capable;
        pst_mib_info->st_wlan_mib_sta_config.en_dot11VHTOptionImplemented = pst_join_req->st_bss_dscr.en_vht_capable;
        pst_mib_info->st_phy_ht.en_dot11LDPCCodingOptionActivated = (pst_join_req->st_bss_dscr.en_ht_ldpc && pst_mac_vap->pst_mib_info->st_phy_ht.en_dot11LDPCCodingOptionImplemented);
        pst_mib_info->st_phy_ht.en_dot11TxSTBCOptionActivated = (pst_join_req->st_bss_dscr.en_ht_stbc && pst_mac_vap->pst_mib_info->st_phy_ht.en_dot11TxSTBCOptionImplemented);

        /* ����2G AP����2ght40��ֹλΪ1ʱ����ѧϰAP��HT 40���� */
        if (!(WLAN_BAND_2G == pst_mac_vap->st_channel.en_band && pst_mac_vap->st_cap_flag.bit_disable_2ght40))
        {
            if(WLAN_BW_CAP_20M == pst_join_req->st_bss_dscr.en_bw_cap)
            {
                mac_mib_set_FortyMHzOperationImplemented(pst_mac_vap, OAL_FALSE);
            }
            else
            {
                mac_mib_set_FortyMHzOperationImplemented(pst_mac_vap, OAL_TRUE);
            }
        }

        /*����Ҫ����AP��Э��ģʽ����STA�����ʼ� */
        ul_ret = hmac_sta_get_user_protocol(&(pst_join_req->st_bss_dscr), &(st_cfg_mode.en_protocol));
        if (OAL_SUCC != ul_ret)
        {
            OAM_ERROR_LOG1(0, OAM_SF_SCAN, "{hmac_sta_update_join_req_params::hmac_sta_get_user_protocol fail %d.}", ul_ret);
            return ul_ret;
        }

        st_cfg_mode.en_band         = pst_join_req->st_bss_dscr.st_channel.en_band;
        st_cfg_mode.en_bandwidth    =  hmac_sta_get_band(pst_mac_device->en_bandwidth_cap, pst_join_req->st_bss_dscr.en_channel_bandwidth);
        st_cfg_mode.en_channel_idx  = pst_join_req->st_bss_dscr.st_channel.uc_chan_number;
        ul_ret = hmac_config_sta_update_rates(pst_mac_vap, &st_cfg_mode, (oal_void *)&pst_join_req->st_bss_dscr);
        if (OAL_SUCC != ul_ret)
        {
            OAM_ERROR_LOG1(0, OAM_SF_SCAN, "{hmac_sta_update_join_req_params::hmac_config_sta_update_rates fail %d.}", ul_ret);
            return ul_ret;
        }
    }

    /* ��Щ����Э��ֻ�ܹ�����legacy */
    hmac_sta_protocol_down_by_chipher(pst_mac_vap, &pst_join_req->st_bss_dscr);
#if defined (_PRE_WLAN_FEATURE_WPA) || defined (_PRE_WLAN_FEATURE_WPA2)
#ifndef _PRE_WIFI_DMT
    st_cfg_mode.en_protocol  = pst_mac_vap->en_protocol;
    st_cfg_mode.en_band      = pst_mac_vap->st_channel.en_band;
    st_cfg_mode.en_bandwidth = pst_mac_vap->st_channel.en_bandwidth;
    st_cfg_mode.en_channel_idx  = pst_join_req->st_bss_dscr.st_channel.uc_chan_number;
    hmac_config_sta_update_rates(pst_mac_vap, &st_cfg_mode, (oal_void *)&pst_join_req->st_bss_dscr);
#endif
#endif

    /* STA������20MHz���У����Ҫ�л���40 or 80MHz���У���Ҫ����һ������: */
    /* (1) �û�֧��40 or 80MHz���� */
    /* (2) AP֧��40 or 80MHz����(HT Supported Channel Width Set = 1 && VHT Supported Channel Width Set = 0) */
    /* (3) AP��40 or 80MHz����(SCO = SCA or SCB && VHT Channel Width = 1) */
#ifdef _PRE_WLAN_FEATURE_20_40_80_COEXIST
    pst_mac_vap->st_channel.en_bandwidth   = WLAN_BAND_WIDTH_20M;
#endif
    ul_ret = mac_get_channel_idx_from_num(pst_mac_vap->st_channel.en_band, pst_mac_vap->st_channel.uc_chan_number, &(pst_mac_vap->st_channel.uc_idx));
    if (OAL_SUCC != ul_ret)
    {
        OAM_ERROR_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_SCAN, "{hmac_sta_update_join_req_params::band and channel_num are not compatible.band[%d], channel_num[%d]}",
                        pst_mac_vap->st_channel.en_band, pst_mac_vap->st_channel.uc_chan_number);
        return ul_ret;
    }

    /* ����Э�������Ϣ������WMM P2P 11I 20/40M�� */
    hmac_update_join_req_params_prot_sta(pst_hmac_vap, pst_join_req);
    /* �����Ż�����ͬƵ���µ�������һ�� */
    if (WLAN_BAND_2G == pst_mac_vap->st_channel.en_band)
    {
        mac_mib_set_ShortPreambleOptionImplemented(pst_mac_vap, WLAN_LEGACY_11B_MIB_SHORT_PREAMBLE);
        mac_mib_set_SpectrumManagementRequired(pst_mac_vap, OAL_FALSE);
    }
    else
    {
        mac_mib_set_ShortPreambleOptionImplemented(pst_mac_vap, WLAN_LEGACY_11B_MIB_LONG_PREAMBLE);
        mac_mib_set_SpectrumManagementRequired(pst_mac_vap, OAL_TRUE);
    }

    if (0 == hmac_calc_up_ap_num(pst_mac_device))
    {
        pst_mac_device->uc_max_channel   = pst_mac_vap->st_channel.uc_chan_number;
        pst_mac_device->en_max_band      = pst_mac_vap->st_channel.en_band;
        pst_mac_device->en_max_bandwidth = pst_mac_vap->st_channel.en_bandwidth;
    }

    /* ���¼���DMAC, �����¼��ڴ� */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_ctx_join_req_set_reg_stru));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_SCAN, "{hmac_sta_update_join_req_params::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��д�¼� */
    pst_event = (frw_event_stru *)pst_event_mem->puc_data;

    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_WLAN_CTX,
                       DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_SET_REG,
                       OAL_SIZEOF(dmac_ctx_join_req_set_reg_stru),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_hmac_vap->st_vap_base_info.uc_chip_id,
                       pst_hmac_vap->st_vap_base_info.uc_device_id,
                       pst_hmac_vap->st_vap_base_info.uc_vap_id);

    pst_reg_params = (dmac_ctx_join_req_set_reg_stru *)pst_event->auc_event_data;

    /* ������Ҫд��Ĵ�����BSSID��Ϣ */
    oal_set_mac_addr(pst_reg_params->auc_bssid, pst_join_req->st_bss_dscr.auc_bssid);

    /* ��д�ŵ������Ϣ */
    pst_reg_params->st_current_channel.uc_chan_number = pst_mac_vap->st_channel.uc_chan_number;
    pst_reg_params->st_current_channel.en_band        = pst_mac_vap->st_channel.en_band;
    pst_reg_params->st_current_channel.en_bandwidth   = pst_mac_vap->st_channel.en_bandwidth;
    pst_reg_params->st_current_channel.uc_idx         = pst_mac_vap->st_channel.uc_idx;

    /* ����beaocn period��Ϣ */
    pst_reg_params->us_beacon_period      = (pst_join_req->st_bss_dscr.us_beacon_period);

    /* ͬ��FortyMHzOperationImplemented */
    pst_reg_params->en_dot11FortyMHzOperationImplemented   = mac_mib_get_FortyMHzOperationImplemented(pst_mac_vap);

    /* ����beacon filter�ر� */
    pst_reg_params->ul_beacon_filter    = OAL_FALSE;

    /* ����no frame filter�� */
    pst_reg_params->ul_non_frame_filter = OAL_TRUE;

    /* �·�ssid */
    oal_memcopy(pst_reg_params->auc_ssid, pst_join_req->st_bss_dscr.ac_ssid, WLAN_SSID_MAX_LEN);
    pst_reg_params->auc_ssid[WLAN_SSID_MAX_LEN - 1] = '\0';

    /* �ַ��¼� */
    frw_event_dispatch_event(pst_event_mem);
    FRW_EVENT_FREE(pst_event_mem);

    return OAL_SUCC;
}


oal_uint32 hmac_sta_wait_asoc_timeout(hmac_vap_stru *pst_hmac_sta, oal_void *p_param)
{
    hmac_asoc_rsp_stru            st_asoc_rsp = {0};
    hmac_mgmt_timeout_param_stru *pst_timeout_param;

    if ((OAL_PTR_NULL == pst_hmac_sta)|| (OAL_PTR_NULL == p_param))
    {
        OAM_ERROR_LOG2(0, OAM_SF_SCAN, "{hmac_sta_wait_asoc_timeout::param null,%d %d.}", pst_hmac_sta,p_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_timeout_param = (hmac_mgmt_timeout_param_stru *)p_param;

    /* ��д������� */
    st_asoc_rsp.en_result_code = HMAC_MGMT_TIMEOUT;

    /* ������ʱʧ��,ԭ�����ϱ�wpa_supplicant */
    st_asoc_rsp.en_status_code = pst_timeout_param->en_status_code;

    /* ���͹��������SME */
    hmac_send_rsp_to_sme_sta(pst_hmac_sta, HMAC_SME_ASOC_RSP, (oal_uint8 *)&st_asoc_rsp);

    return OAL_SUCC;
}


oal_void  hmac_sta_handle_disassoc_rsp(hmac_vap_stru *pst_hmac_vap, oal_uint16 us_disasoc_reason_code)
{
    frw_event_mem_stru  *pst_event_mem;
    frw_event_stru      *pst_event;

    /* �׼�������¼���WAL */
    pst_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(oal_uint16));
    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_SCAN, "{hmac_sta_handle_disassoc_rsp::pst_event_mem null.}");
        return;
    }

    /* ��д�¼� */
    pst_event = (frw_event_stru *)pst_event_mem->puc_data;

    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                     FRW_EVENT_TYPE_HOST_CTX,
                     HMAC_HOST_CTX_EVENT_SUB_TYPE_DISASOC_COMP_STA,
                     OAL_SIZEOF(oal_uint16),
                     FRW_EVENT_PIPELINE_STAGE_0,
                     pst_hmac_vap->st_vap_base_info.uc_chip_id,
                     pst_hmac_vap->st_vap_base_info.uc_device_id,
                     pst_hmac_vap->st_vap_base_info.uc_vap_id);

    *((oal_uint16 *)pst_event->auc_event_data) = us_disasoc_reason_code;      /* �¼�payload��д���Ǵ����� */

    /* �ַ��¼� */
    frw_event_dispatch_event(pst_event_mem);
    FRW_EVENT_FREE(pst_event_mem);

    return;
}


OAL_STATIC oal_uint32 hmac_sta_rx_deauth_req(hmac_vap_stru *pst_hmac_vap, oal_uint8 *puc_mac_hdr, oal_bool_enum_uint8 en_is_protected)
{
    oal_uint8       auc_bssid[6]            = {0};
    hmac_user_stru *pst_hmac_user_vap       = OAL_PTR_NULL;
    oal_uint16      us_user_idx             = 0xffff;
    oal_uint32      ul_ret;
    oal_uint8      *puc_da;
    oal_uint8      *puc_sa = OAL_PTR_NULL;
    oal_uint32      ul_ret_del_user;


    if (OAL_PTR_NULL == pst_hmac_vap || OAL_PTR_NULL == puc_mac_hdr)
    {
        OAM_ERROR_LOG2(0, OAM_SF_AUTH, "{hmac_sta_rx_deauth_req::param null,%d %d.}", pst_hmac_vap, puc_mac_hdr);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ���ӽ��յ�ȥ��֤֡����ȥ����֡ʱ��ά����Ϣ */
    mac_rx_get_sa((mac_ieee80211_frame_stru *)puc_mac_hdr, &puc_sa);
    OAM_WARNING_LOG4(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                     "{hmac_sta_rx_deauth_req::Because of err_code[%d], received deauth or disassoc frame frome source addr, sa xx:xx:xx:%2x:%2x:%2x.}",
                     *((oal_uint16 *)(puc_mac_hdr + MAC_80211_FRAME_LEN)), puc_sa[3], puc_sa[4], puc_sa[5]);

    mac_get_address2(puc_mac_hdr, auc_bssid);

    ul_ret = mac_vap_find_user_by_macaddr(&pst_hmac_vap->st_vap_base_info, auc_bssid, &us_user_idx);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                         "{hmac_sta_rx_deauth_req::find user failed[%d],other bss deauth frame!}", ul_ret);
        /* û�в鵽��Ӧ��USER,����ȥ��֤��Ϣ */
        //hmac_mgmt_send_deauth_frame(&(pst_hmac_vap->st_vap_base_info), auc_bssid, MAC_NOT_AUTHED, OAL_FALSE);
        return ul_ret;
    }
    pst_hmac_user_vap = mac_res_get_hmac_user(us_user_idx);
    if (OAL_PTR_NULL == pst_hmac_user_vap)
    {
        OAM_ERROR_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                       "{hmac_sta_rx_deauth_req::pst_hmac_user_vap[%d] null.}", us_user_idx);
        /* û�в鵽��Ӧ��USER,����ȥ��֤��Ϣ */
        hmac_mgmt_send_deauth_frame(&(pst_hmac_vap->st_vap_base_info), auc_bssid, MAC_NOT_AUTHED, OAL_FALSE);

        hmac_fsm_change_state(pst_hmac_vap, MAC_VAP_STATE_STA_FAKE_UP);
        /* �ϱ��ں�sta�Ѿ���ĳ��apȥ���� */
	    hmac_sta_handle_disassoc_rsp(pst_hmac_vap, *((oal_uint16 *)(puc_mac_hdr + MAC_80211_FRAME_LEN)));
        return OAL_FAIL;
    }

#if (_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT)
    /*����Ƿ���Ҫ����SA query request*/
    if ((MAC_USER_STATE_ASSOC == pst_hmac_user_vap->st_user_base_info.en_user_asoc_state) &&
        (OAL_SUCC == hmac_pmf_check_err_code(&pst_hmac_user_vap->st_user_base_info, en_is_protected, puc_mac_hdr)))
    {
       /*�ڹ���״̬���յ�δ���ܵ�ReasonCode 6/7��Ҫ����SA Query����*/
       ul_ret = hmac_start_sa_query(&pst_hmac_vap->st_vap_base_info, pst_hmac_user_vap, pst_hmac_user_vap->st_user_base_info.st_cap_info.bit_pmf_active);
       if(OAL_SUCC != ul_ret)
       {
           return OAL_ERR_CODE_PMF_SA_QUERY_START_FAIL;
       }

       return OAL_SUCC;
    }

#endif

    /*������û��Ĺ���֡�������Բ�һ�£������ñ���*/
    mac_rx_get_da((mac_ieee80211_frame_stru *)puc_mac_hdr, &puc_da);
    if ((OAL_TRUE != ETHER_IS_MULTICAST(puc_da)) &&
       (en_is_protected != pst_hmac_user_vap->st_user_base_info.st_cap_info.bit_pmf_active))
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                       "{hmac_sta_rx_deauth_req::PMF check failed.}");

        return OAL_FAIL;
    }


    /* TBD �ϱ�system error ��λ mac */

    /* ɾ��user */
    ul_ret_del_user = hmac_user_del(&pst_hmac_vap->st_vap_base_info, pst_hmac_user_vap);
    if ( OAL_SUCC != ul_ret_del_user)
    {
        OAM_ERROR_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_AUTH,
                       "{hmac_sta_rx_deauth_req::hmac_user_del failed.}");

        /* �ϱ��ں�sta�Ѿ���ĳ��apȥ���� */
    	hmac_sta_handle_disassoc_rsp(pst_hmac_vap, *((oal_uint16 *)(puc_mac_hdr + MAC_80211_FRAME_LEN)));
        return OAL_FAIL;
    }

    /* �ϱ��ں�sta�Ѿ���ĳ��apȥ���� */
    hmac_sta_handle_disassoc_rsp(pst_hmac_vap, *((oal_uint16 *)(puc_mac_hdr + MAC_80211_FRAME_LEN)));


    return OAL_SUCC;
}


OAL_STATIC oal_uint32  hmac_sta_up_rx_beacon(hmac_vap_stru *pst_hmac_vap_sta, oal_netbuf_stru *pst_netbuf)
{
    dmac_rx_ctl_stru           *pst_rx_ctrl;
    mac_rx_ctl_stru            *pst_rx_info;
    mac_ieee80211_frame_stru   *pst_mac_hdr;
    oal_uint32                  ul_ret;
    oal_uint16                  us_frame_len;
    oal_uint16                  us_frame_offset;
    oal_uint8                  *puc_frame_body;
    oal_uint8                   uc_frame_sub_type;
    hmac_user_stru             *pst_hmac_user;
    oal_uint8                   auc_addr_sa[WLAN_MAC_ADDR_LEN];
    oal_uint16                  us_user_idx;
#ifdef _PRE_WLAN_FEATURE_TXBF
    oal_uint8                  *puc_txbf_vendor_ie;
#endif

    pst_rx_ctrl         = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf);
    pst_rx_info         = (mac_rx_ctl_stru *)(&(pst_rx_ctrl->st_rx_info));
    pst_mac_hdr         = (mac_ieee80211_frame_stru *)(pst_rx_info->pul_mac_hdr_start_addr);
    puc_frame_body      = (oal_uint8* )pst_mac_hdr + pst_rx_info->uc_mac_header_len;
    us_frame_len        = pst_rx_info->us_frame_len - pst_rx_info->uc_mac_header_len; /*֡�峤��*/

    us_frame_offset = MAC_TIME_STAMP_LEN + MAC_BEACON_INTERVAL_LEN + MAC_CAP_INFO_LEN;
    uc_frame_sub_type = mac_get_frame_sub_type((oal_uint8 *)pst_mac_hdr);

    /* ��������bss��Beacon�������� */
    ul_ret = oal_compare_mac_addr(pst_hmac_vap_sta->st_vap_base_info.auc_bssid, pst_mac_hdr->auc_address3);
    if(0 != ul_ret)
    {
        return OAL_SUCC;
    }

    /* ��ȡ����֡��Դ��ַSA */
    mac_get_address2((oal_uint8 *)pst_mac_hdr, auc_addr_sa);

    /* ����SA �ص��ҵ���ӦAP USER�ṹ */
    ul_ret = mac_vap_find_user_by_macaddr(&(pst_hmac_vap_sta->st_vap_base_info), auc_addr_sa, &us_user_idx);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_hmac_vap_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX,
                        "{hmac_sta_up_rx_beacon::mac_vap_find_user_by_macaddr failed[%d].}", ul_ret);
        return ul_ret;
    }
    pst_hmac_user = mac_res_get_hmac_user(us_user_idx);
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_ERROR_LOG1(pst_hmac_vap_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX,
                       "{hmac_sta_up_rx_beacon::pst_hmac_user[%d] null.}", us_user_idx);
        return OAL_ERR_CODE_PTR_NULL;
    }
#ifdef _PRE_WLAN_FEATURE_TXBF
    /* ����11n txbf���� */
    puc_txbf_vendor_ie = mac_find_vendor_ie(MAC_HUAWEI_VENDER_IE, MAC_EID_11NTXBF, (oal_uint8 *)pst_mac_hdr + MAC_TAG_PARAM_OFFSET, us_frame_len - us_frame_offset);
    hmac_mgmt_update_11ntxbf_cap(puc_txbf_vendor_ie, pst_hmac_user);
#endif

    /* �����Ƿ���Ҫ���д����л� */
    //hmac_chan_reval_bandwidth_sta(&pst_hmac_vap_sta->st_vap_base_info, ul_change_flag);

    /* ����edca���� */
    hmac_sta_up_update_edca_params(puc_frame_body, us_frame_len, us_frame_offset, pst_hmac_vap_sta, uc_frame_sub_type, pst_hmac_user);

    return OAL_SUCC;
}


#if 0
OAL_STATIC oal_void  hmac_handle_tbtt_chan_mgmt_sta(hmac_vap_stru *pst_hmac_vap)
{
    mac_vap_stru *pst_mac_vap = &(pst_hmac_vap->st_vap_base_info);

    if (OAL_FALSE == mac_mib_get_SpectrumManagementImplemented(pst_mac_vap))
    {
        return;
    }

    /* ���AP���͵�CSA IE�е�"�ŵ��л�����"Ϊ�㣬�������л��ŵ� */
    if (OAL_TRUE == pst_mac_vap->st_ch_switch_info.en_channel_swt_cnt_zero)
    {
        if (pst_hmac_vap->us_link_loss > WLAN_LINK_LOSS_THRESHOLD)
        {
            hmac_chan_sta_switch_channel(pst_mac_vap);
            pst_hmac_vap->us_link_loss = 0;
        }
    }

    /* �ŵ��л��Ѿ���ɡ��������ŵ��ϵȴ�����AP���͵�Beacon֡ */
    if (OAL_TRUE == pst_mac_vap->st_ch_switch_info.en_waiting_for_ap)
    {
        /* �ȴ�һ��ʱ��������ŵ��ϻָ�Ӳ������(������ʱAP�п��ܻ�û���л������ŵ�) */
        if (pst_hmac_vap->us_link_loss > WLAN_LINK_LOSS_THRESHOLD)
        {
            pst_mac_vap->st_ch_switch_info.en_waiting_for_ap = OAL_FALSE;
            hmac_chan_enable_machw_tx(pst_mac_vap);

            pst_hmac_vap->us_link_loss = 0;
        }
    }

    /* ���AP���͵�CSA IE�е�"�ŵ��л�����"��Ϊ�㣬��ÿһ��TBTT�ж��м�һ */
    if (pst_mac_vap->st_ch_switch_info.uc_new_ch_swt_cnt > 0)
    {
        pst_mac_vap->st_ch_switch_info.uc_new_ch_swt_cnt--;

        if (0 == pst_mac_vap->st_ch_switch_info.uc_new_ch_swt_cnt)
        {
            if (OAL_TRUE == pst_mac_vap->st_ch_switch_info.en_waiting_to_shift_channel)
            {
                hmac_chan_sta_switch_channel(pst_mac_vap);
            }
        }
    }
}
#endif

#ifdef _PRE_WLAN_FEATURE_LOCATION_RAM

OAL_STATIC oal_void  hmac_sta_up_rx_action_nonuser(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf)
{
    dmac_rx_ctl_stru               *pst_rx_ctrl;
    oal_uint8                      *puc_data;

    if(OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_vap || OAL_PTR_NULL == pst_netbuf))
    {

        OAM_ERROR_LOG0(0, OAM_SF_FTM,"{hmac_sta_up_rx_action_nonuser::PTR null .}");
        return;
    }

    pst_rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf);

    /* ��ȡ֡��ָ�� */
    puc_data = (oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr + pst_rx_ctrl->st_rx_info.uc_mac_header_len;

    /* Category */
    switch (puc_data[MAC_ACTION_OFFSET_CATEGORY])
    {
        case MAC_ACTION_CATEGORY_PUBLIC:
        {
            /* Action */
            switch (puc_data[MAC_ACTION_OFFSET_ACTION])
            {
                case MAC_PUB_VENDOR_SPECIFIC:
                {
                    if (0 == oal_memcmp(puc_data  + MAC_ACTION_CATEGORY_AND_CODE_LEN, g_auc_huawei_oui, MAC_OUI_LEN))
                    {
                       OAM_WARNING_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{hmac_sta_up_rx_action_nonuser::hmac location get.}");
                       hmac_huawei_action_process(pst_hmac_vap, pst_netbuf, puc_data[MAC_ACTION_CATEGORY_AND_CODE_LEN + MAC_OUI_LEN]);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        break;

        default:
            break;
    }
    return;
}
#endif


OAL_STATIC oal_void  hmac_sta_up_rx_action(hmac_vap_stru *pst_hmac_vap, oal_netbuf_stru *pst_netbuf, oal_bool_enum_uint8 en_is_protected)
{
    dmac_rx_ctl_stru               *pst_rx_ctrl;
    oal_uint8                      *puc_data;
    mac_ieee80211_frame_stru       *pst_frame_hdr;          /* ����mac֡��ָ�� */
    hmac_user_stru                 *pst_hmac_user;
#ifdef _PRE_WLAN_FEATURE_P2P
    oal_uint8                      *puc_p2p0_mac_addr;
#endif
    pst_rx_ctrl = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_netbuf);

    /* ��ȡ֡ͷ��Ϣ */
    pst_frame_hdr = (mac_ieee80211_frame_stru *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr;
#ifdef _PRE_WLAN_FEATURE_P2P
    /* P2P0�豸�����ܵ�actionȫ���ϱ� */
    puc_p2p0_mac_addr = pst_hmac_vap->st_vap_base_info.pst_mib_info->st_wlan_mib_sta_config.auc_p2p0_dot11StationID;
    if (0 == oal_compare_mac_addr(pst_frame_hdr->auc_address1, puc_p2p0_mac_addr))
    {
        hmac_rx_mgmt_send_to_host(pst_hmac_vap, pst_netbuf);
    }
#endif

    /* ��ȡ���Ͷ˵��û�ָ�� */
    pst_hmac_user = mac_vap_get_hmac_user_by_addr(&pst_hmac_vap->st_vap_base_info, pst_frame_hdr->auc_address2);
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_WARNING_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{hmac_sta_up_rx_action::mac_vap_find_user_by_macaddr failed.}");
#ifdef _PRE_WLAN_FEATURE_LOCATION_RAM
        hmac_sta_up_rx_action_nonuser(pst_hmac_vap, pst_netbuf);
#endif
        return;
    }

    /* ��ȡ֡��ָ�� */
    puc_data = (oal_uint8 *)pst_rx_ctrl->st_rx_info.pul_mac_hdr_start_addr + pst_rx_ctrl->st_rx_info.uc_mac_header_len;

    /* Category */
    switch (puc_data[MAC_ACTION_OFFSET_CATEGORY])
    {
        case MAC_ACTION_CATEGORY_BA:
        {
            switch(puc_data[MAC_ACTION_OFFSET_ACTION])
            {
                case MAC_BA_ACTION_ADDBA_REQ:
#ifdef _PRE_WLAN_FEATURE_BTCOEX
                    hmac_btcoex_check_rx_same_baw_start_from_addba_req(pst_hmac_vap, pst_hmac_user, pst_frame_hdr, puc_data);
#endif
                    hmac_mgmt_rx_addba_req(pst_hmac_vap, pst_hmac_user, puc_data);
                    break;

                case MAC_BA_ACTION_ADDBA_RSP:
                    hmac_mgmt_rx_addba_rsp(pst_hmac_vap, pst_hmac_user, puc_data);
                    break;

                case MAC_BA_ACTION_DELBA:
                    hmac_mgmt_rx_delba(pst_hmac_vap, pst_hmac_user, puc_data);
                    break;

                default:
                    break;
            }
        }
        break;

        case MAC_ACTION_CATEGORY_WNM:
            OAM_WARNING_LOG1(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{hmac_sta_up_rx_action::MAC_ACTION_CATEGORY_WNM action=%d.}",
                            puc_data[MAC_ACTION_OFFSET_ACTION]);
            switch (puc_data[MAC_ACTION_OFFSET_ACTION])
            {
            #ifdef _PRE_WLAN_FEATURE_11V_ENABLE
                   /* bss transition request ֡������� */
                    case MAC_WNM_ACTION_BSS_TRANSITION_MGMT_REQUEST:
                        hmac_rx_bsst_req_action(pst_hmac_vap, pst_hmac_user, pst_netbuf);
                    break;
            #endif
                default:
            #ifdef _PRE_WLAN_FEATURE_HS20
                   /* �ϱ�WNM Notification Request Action֡ */
                    hmac_rx_mgmt_send_to_host(pst_hmac_vap, pst_netbuf);
            #endif
                    break;
            }

            break;

        case MAC_ACTION_CATEGORY_PUBLIC:
        {
            /* Action */
            switch (puc_data[MAC_ACTION_OFFSET_ACTION])
            {
                case MAC_PUB_VENDOR_SPECIFIC:
            #ifdef _PRE_WLAN_FEATURE_P2P
                /*����OUI-OUI typeֵΪ 50 6F 9A - 09 (WFA P2P v1.0)  */
                /* ����hmac_rx_mgmt_send_to_host�ӿ��ϱ� */
                    if (OAL_TRUE == mac_ie_check_p2p_action(puc_data + MAC_ACTION_OFFSET_ACTION))
                    {
                       hmac_rx_mgmt_send_to_host(pst_hmac_vap, pst_netbuf);
                    }
            #endif
            #ifdef _PRE_WLAN_FEATURE_LOCATION_RAM
                    if (0 == oal_memcmp(puc_data  + MAC_ACTION_CATEGORY_AND_CODE_LEN, g_auc_huawei_oui, MAC_OUI_LEN))
                    {
                       OAM_WARNING_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{hmac_sta_up_rx_action::hmac location get.}");
                       hmac_huawei_action_process(pst_hmac_vap, pst_netbuf, puc_data[MAC_ACTION_CATEGORY_AND_CODE_LEN + MAC_OUI_LEN]);
                    }
            #endif  /* _PRE_WLAN_FEATURE_LOCATION */
                    break;

                case MAC_PUB_GAS_INIT_RESP:
                case MAC_PUB_GAS_COMBAK_RESP:
            #ifdef _PRE_WLAN_FEATURE_HS20
                /*�ϱ�GAS��ѯ��ACTION֡ */
                 hmac_rx_mgmt_send_to_host(pst_hmac_vap, pst_netbuf);
            #endif
                    break;

                default:
                    break;
            }
        }
        break;
#ifdef _PRE_WLAN_FEATURE_WMMAC
        case MAC_ACTION_CATEGORY_WMMAC_QOS:
        {
            if(OAL_TRUE == g_en_wmmac_switch)
            {
                switch(puc_data[MAC_ACTION_OFFSET_ACTION])
                {
                    case MAC_WMMAC_ACTION_ADDTS_RSP:
                        hmac_mgmt_rx_addts_rsp(pst_hmac_vap, pst_hmac_user, puc_data);
                        break;
                    case MAC_WMMAC_ACTION_DELTS:
                        hmac_mgmt_rx_delts(pst_hmac_vap, pst_hmac_user, puc_data);
                        break;
                    default:
                        break;
                }
            }
        }
        break;
#endif //_PRE_WLAN_FEATURE_WMMAC

#if (_PRE_WLAN_FEATURE_PMF != _PRE_PMF_NOT_SUPPORT)
        case MAC_ACTION_CATEGORY_SA_QUERY:
        {
            switch (puc_data[MAC_ACTION_OFFSET_ACTION])
            {
                case MAC_SA_QUERY_ACTION_REQUEST:
                    hmac_rx_sa_query_req(pst_hmac_vap, pst_netbuf, en_is_protected);
                    break;
                case MAC_SA_QUERY_ACTION_RESPONSE:
                    hmac_rx_sa_query_rsp(pst_hmac_vap, pst_netbuf, en_is_protected);
                    break;

                default:
                    break;
            }
        }
        break;
#endif
        case MAC_ACTION_CATEGORY_VENDOR:
        {
    #ifdef _PRE_WLAN_FEATURE_P2P
        /*����OUI-OUI typeֵΪ 50 6F 9A - 09 (WFA P2P v1.0)  */
        /* ����hmac_rx_mgmt_send_to_host�ӿ��ϱ� */
            if (OAL_TRUE == mac_ie_check_p2p_action(puc_data + MAC_ACTION_OFFSET_CATEGORY))
            {
               hmac_rx_mgmt_send_to_host(pst_hmac_vap, pst_netbuf);
            }
    #endif
        }
        break;
#ifdef _PRE_WLAN_FEATURE_ROAM
#ifdef _PRE_WLAN_FEATURE_11R
        case MAC_ACTION_CATEGORY_FAST_BSS_TRANSITION:
        {
            if(pst_hmac_vap->bit_11r_enable != OAL_TRUE)
            {
                break;
            }
            hmac_roam_rx_ft_action(pst_hmac_vap, pst_netbuf);
            break;
        }
#endif //_PRE_WLAN_FEATURE_11R
#endif //_PRE_WLAN_FEATURE_ROAM

        default:
        break;
    }
}


oal_uint32  hmac_sta_up_rx_mgmt(hmac_vap_stru *pst_hmac_vap_sta, oal_void *p_param)
{
    dmac_wlan_crx_event_stru   *pst_mgmt_rx_event;
    dmac_rx_ctl_stru           *pst_rx_ctrl;
    mac_rx_ctl_stru            *pst_rx_info;
    oal_uint8                  *puc_mac_hdr;
    oal_uint8                   uc_mgmt_frm_type;
    oal_bool_enum_uint8         en_is_protected = OAL_FALSE;

    if (OAL_PTR_NULL == pst_hmac_vap_sta || OAL_PTR_NULL == p_param)
    {
        OAM_ERROR_LOG2(0, OAM_SF_RX, "{hmac_sta_up_rx_mgmt::param null,%d %d.}", pst_hmac_vap_sta, p_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mgmt_rx_event   = (dmac_wlan_crx_event_stru *)p_param;
    pst_rx_ctrl         = (dmac_rx_ctl_stru *)oal_netbuf_cb(pst_mgmt_rx_event->pst_netbuf);
    pst_rx_info         = (mac_rx_ctl_stru *)(&(pst_rx_ctrl->st_rx_info));
    puc_mac_hdr         = (oal_uint8 *)(pst_rx_info->pul_mac_hdr_start_addr);
    //en_is_protected     = (pst_rx_ctrl->st_rx_status.bit_cipher_protocol_type == hal_cipher_suite_to_ctype(WLAN_80211_CIPHER_SUITE_CCMP)) ? OAL_TRUE : OAL_FALSE;
    en_is_protected     = mac_get_protectedframe(puc_mac_hdr);

    /* STA��UP״̬�� ���յ��ĸ��ֹ���֡���� */
    uc_mgmt_frm_type = mac_get_frame_sub_type(puc_mac_hdr);

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_WAKEUP_SRC_PARSE
        if(OAL_TRUE == wlan_pm_wkup_src_debug_get())
        {
            wlan_pm_wkup_src_debug_set(OAL_FALSE);
            OAM_WARNING_LOG1(pst_hmac_vap_sta->st_vap_base_info.uc_vap_id, OAM_SF_RX, "{wifi_wake_src:hmac_sta_up_rx_mgmt_etc::wakeup mgmt type[0x%x]}",uc_mgmt_frm_type);
        }
#endif
#endif

    /*Bar frame proc here*/
    if (WLAN_FC0_TYPE_CTL == mac_get_frame_type(puc_mac_hdr))
    {
        uc_mgmt_frm_type = mac_get_frame_sub_type(puc_mac_hdr);
        if (WLAN_BLOCKACK_REQ == (uc_mgmt_frm_type >> 4))
        {
            hmac_up_rx_bar(pst_hmac_vap_sta, pst_rx_ctrl, pst_mgmt_rx_event->pst_netbuf);
        }
    }



#ifdef _PRE_WLAN_FEATURE_SNIFFER
    proc_sniffer_write_file(NULL, 0, puc_mac_hdr, pst_rx_info->us_frame_len, 0);
#endif
    switch (uc_mgmt_frm_type)
    {
        case WLAN_FC0_SUBTYPE_DEAUTH:
        case WLAN_FC0_SUBTYPE_DISASSOC:
            hmac_sta_rx_deauth_req(pst_hmac_vap_sta, puc_mac_hdr, en_is_protected);
            break;

        case WLAN_FC0_SUBTYPE_BEACON:
            hmac_sta_up_rx_beacon(pst_hmac_vap_sta, pst_mgmt_rx_event->pst_netbuf);
            break;

        case WLAN_FC0_SUBTYPE_ACTION:
            hmac_sta_up_rx_action(pst_hmac_vap_sta, pst_mgmt_rx_event->pst_netbuf, en_is_protected);
            break;
        default:
            break;
    }

    return OAL_SUCC;
}

/*lint -e578*//*lint -e19*/
oal_module_symbol(hmac_check_capability_mac_phy_supplicant);
/*lint +e578*//*lint +e19*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

