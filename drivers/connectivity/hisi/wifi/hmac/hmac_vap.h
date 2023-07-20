

#ifndef __HMAC_VAP_H__
#define __HMAC_VAP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "oal_ext_if.h"
#include "mac_vap.h"
#include "hmac_ext_if.h"
#include "hmac_user.h"
#include "hmac_main.h"
#include "mac_resource.h"
#ifdef _PRE_WLAN_TCP_OPT
#include "hmac_tcp_opt_struc.h"
#include "oal_hcc_host_if.h"
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
#include "hmac_btcoex.h"
#endif
#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_VAP_H

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#ifdef _PRE_WLAN_DFT_STAT
#define   HMAC_VAP_DFT_STATS_PKT_INCR(_member, _cnt)        ((_member) += (_cnt))
#else
#define   HMAC_VAP_DFT_STATS_PKT_INCR(_member, _cnt)
#endif
#define   HMAC_VAP_STATS_PKT_INCR(_member, _cnt)            ((_member) += (_cnt))

#ifdef _PRE_WLAN_FEATURE_HS20
#define MAX_QOS_UP_RANGE  8
#define MAX_DSCP_EXCEPT   21  /* maximum of DSCP Exception fields for QoS Map set */
#endif

#ifdef _PRE_WLAN_FEATURE_EQUIPMENT_TEST
#define  HMAC_HIPRIV_ACK_BUF_SIZE  3
#endif
/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/*****************************************************************************
    ��ʼ��vap����ö��
*****************************************************************************/
typedef enum
{
    HMAC_ADDBA_MODE_AUTO,
    HMAC_ADDBA_MODE_MANUAL,

    HMAC_ADDBA_MODE_BUTT
}hmac_addba_mode_enum;
typedef oal_uint8 hmac_addba_mode_enum_uint8;


/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/


/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
typedef struct
{
    oal_dlist_head_stru st_timeout_head;
}hmac_mgmt_timeout_stru;

typedef struct
{
    oal_uint16                  us_user_index;
    mac_vap_state_enum_uint8    en_state;
    oal_uint8                   uc_vap_id;
    mac_status_code_enum_uint16 en_status_code;
    oal_uint16                  auc_rsv[2];
}hmac_mgmt_timeout_param_stru;

#ifdef _PRE_WLAN_FEATURE_HS20
typedef struct
{
    oal_uint8  auc_up_low[MAX_QOS_UP_RANGE];             /* User Priority */
    oal_uint8  auc_up_high[MAX_QOS_UP_RANGE];
    oal_uint8  auc_dscp_exception_up[MAX_DSCP_EXCEPT];   /* User Priority of DSCP Exception field */
    oal_uint8  uc_valid;
    oal_uint8  uc_num_dscp_except;
    oal_uint8  auc_dscp_exception[MAX_DSCP_EXCEPT];      /* DSCP exception field  */
}hmac_cfg_qos_map_param_stru;
#endif

/*�޸Ĵ˽ṹ����Ҫͬ��֪ͨSDT�������ϱ��޷�����*/
typedef struct
{

    /***************************************************************************
                                ���Ͱ�ͳ��
    ***************************************************************************/
    /* ����lan�����ݰ�ͳ�� */
    oal_uint32  ul_rx_pkt_to_lan;                               /* �������̷�����̫�������ݰ���Ŀ��MSDU */
    oal_uint32  ul_rx_bytes_to_lan;                             /* �������̷�����̫�����ֽ��� */

   /***************************************************************************
                                ���Ͱ�ͳ��
    ***************************************************************************/
   /* ��lan���յ������ݰ�ͳ�� */
   oal_uint32  ul_tx_pkt_num_from_lan;                         /* ��lan�����İ���Ŀ,MSDU */
   oal_uint32  ul_tx_bytes_from_lan;                           /* ��lan�������ֽ��� */

}hmac_vap_query_stats_stru;
/*װ������*/
typedef struct
{
    oal_uint32                       ul_rx_pkct_succ_num;                       /*�������ݰ���*/
    oal_uint32                       ul_dbb_num;                                /*DBB�汾��*/
    oal_uint32                       ul_check_fem_pa_status;                    /*fem��pa�Ƿ��ջٱ�־*/
    oal_int16                        s_rx_rssi;
    oal_bool_enum_uint8              uc_get_dbb_completed_flag;                 /*��ȡDBB�汾�ųɹ��ϱ���־*/
    oal_bool_enum_uint8              uc_check_fem_pa_flag;                      /*fem��pa�Ƿ��ջ��ϱ���־*/
    oal_bool_enum_uint8              uc_get_rx_pkct_flag;                       /*�������ݰ��ϱ���־λ*/
    oal_bool_enum_uint8              uc_lte_gpio_check_flag;                    /*�������ݰ��ϱ���־λ*/
    oal_bool_enum_uint8              uc_report_efuse_reg_flag;              /*efuse �Ĵ�����ȡ*/
    oal_uint8                        uc_ant_status : 4,
                                     uc_get_ant_flag : 4;
}hmac_atcmdsrv_get_stats_stru;

#ifdef _PRE_WLAN_FEATURE_EQUIPMENT_TEST
/*51װ������ hipriv�������*/
typedef struct
{
    oal_uint8                   auc_data[HMAC_HIPRIV_ACK_BUF_SIZE];
    oal_bool_enum_uint8         uc_get_hipriv_ack_flag;
    oal_int8                    *pc_buffer;
    oal_uint8                   auc_reserved[3];
}hmac_hipriv_ack_stats_stru;
#endif
typedef struct
{
    oal_dlist_head_stru           st_entry;
    oal_uint8                     auc_bssid[WLAN_MAC_ADDR_LEN];
    oal_uint8                     uc_reserved[2];
    oal_uint8                     auc_pmkid[WLAN_PMKID_LEN];
}hmac_pmksa_cache_stru;

typedef enum _hmac_tcp_opt_queue_
{
    HMAC_TCP_ACK_QUEUE = 0,
    HMAC_TCP_OPT_QUEUE_BUTT
} hmac_tcp_opt_queue;

#ifdef _PRE_WLAN_TCP_OPT
typedef oal_uint16 (* hmac_trans_cb_func)(void *pst_hmac_device, hmac_tcp_opt_queue type,hcc_chan_type dir, oal_netbuf_head_stru* data);
/*tcp_ack�Ż�*/
typedef struct
{
    struct wlan_perform_tcp      hmac_tcp_ack;
    struct wlan_perform_tcp_list hmac_tcp_ack_list;
    wlan_perform_tcp_impls       filter_info;
    hmac_trans_cb_func           filter[HMAC_TCP_OPT_QUEUE_BUTT];	//���˴������Ӻ���
    oal_uint64                   all_ack_count[HMAC_TCP_OPT_QUEUE_BUTT];	//������TCP ACKͳ��
    oal_uint64                   drop_count[HMAC_TCP_OPT_QUEUE_BUTT];	//������TCP ACKͳ��
    oal_netbuf_head_stru         data_queue[HMAC_TCP_OPT_QUEUE_BUTT];
    oal_spin_lock_stru           data_queue_lock[HMAC_TCP_OPT_QUEUE_BUTT];
}hmac_tcp_ack_stru;
#endif

#ifdef _PRE_WLAN_FEATURE_HILINK
/* ��¼�޳�������Ϣ�ṹ�� */
typedef struct
{
    oal_uint8  auc_user_mac_addr[6];     /* ������ɾ���û���MAC��ַ */
    oal_uint8  auc_rev[2];
}hmac_fbt_disable_user_info_stru;

/* ��¼�����л���Ϣ�Ľṹ�� */
typedef struct
{
    oal_uint8  uc_fbt_mode;              /* ���ÿ����л����е�ģʽ��0����ʾ�رգ�1����ʾ����AC���п��� */
    oal_uint8  uc_disabled_user_cnt;     /* ��¼��ֹ�����б���ǰ�û��ĸ��� */
    oal_uint8  auc_rev[2];
    hmac_fbt_disable_user_info_stru ast_fbt_disable_connect_user_list[HMAC_FBT_MAX_USER_NUM];  /* ����32����ֹ�����û�����Ϣ��32����Դ��������ʱ֧��3���û� */
    frw_timeout_stru st_timer;           /* �ѹ����û��ϱ��������ʹ�õĶ�ʱ�� */
}hmac_fbt_mgmt_stru;
#endif

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
typedef struct hmac_psta_rep
{
    oal_rwlock_stru         st_lock;
    oal_dlist_head_stru     st_vsta_list;   // proxysta device list
    oal_dlist_head_stru     st_msta_list;   // main sta list
    oal_dlist_head_stru     st_pbss_list;   // proxy bss list
    oal_dlist_head_stru     ast_hash[MAC_VAP_PROXY_STA_HASH_MAX_VALUE];
    oal_bool_enum_uint8     en_isolation;
    oal_uint8               uc_use_cnt;
    oal_uint8               uc_resv[2];
} hmac_psta_rep_stru;

typedef struct hmac_psta_mgr
{
    hmac_psta_rep_stru       ast_rep[WLAN_PROXY_STA_MAX_REP];
    oal_proc_dir_entry_stru *pst_proc_entry;
} hmac_psta_mgr_stru;

typedef  struct
{
    oal_dlist_head_stru     st_hash_entry;
    oal_dlist_head_stru     st_xsta_entry;
    oal_uint8               auc_oma[WLAN_MAC_ADDR_LEN];
    oal_uint8               uc_rep_id;
    oal_uint8               auc_resv[1];
} hmac_psta_stru;

#define hmac_vap_psta_oma(vap)  ((vap)->st_psta.auc_oma)
#define hmac_vap_psta_in_rep(vap)   (!oal_dlist_is_empty(&(vap)->st_psta.st_hash_entry))
#endif

/* hmac vap�ṹ�� */
/* ����˽ṹ�������ӳ�Ա��ʱ���뱣�������ṹ��8�ֽڶ���*/
typedef struct hmac_vap_tag
{
    /* ap sta�����ֶ� */
    oal_net_device_stru            *pst_net_device;                             /* VAP��Ӧ��net_devices */
    oal_uint8                       auc_name[OAL_IF_NAME_SIZE];                 /* VAP����*/
    hmac_vap_cfg_priv_stru          st_cfg_priv;                                /* wal hmac����ͨ�Žӿ� */

    oal_spin_lock_stru              st_lock_state;                              /* ������Ϳ������VAP״̬���л��� */
    oal_uint16                      us_user_nums_max;                           /* VAP�¿ɹҽӵ�����û����� */
    oal_uint8                       uc_classify_tid;                            /* ���ڻ���vap��������ʹ�ܺ���Ч */
    wlan_auth_alg_enum_uint8        en_auth_mode;                               /* ��֤�㷨 */

    oal_mgmt_tx_stru                st_mgmt_tx;
    frw_timeout_stru                st_mgmt_timer;
    hmac_mgmt_timeout_param_stru    st_mgmt_timetout_param;

    frw_timeout_stru                st_scan_timeout;                            /* vap����ɨ��ʱ����������ʱ��������ʱ�������� */

    hmac_addba_mode_enum_uint8      en_addba_mode;
#ifdef _PRE_WLAN_FEATURE_WMMAC
    oal_uint8                       uc_ts_dialog_token;                         /* TS�Ự����α���ֵ */
#else
    oal_uint8                       uc_resv1;
#endif //_PRE_WLAN_FEATURE_WMMAC
    oal_uint8                       uc_80211i_mode;                             /* ָʾ��ǰ�ķ�ʽʱWPA����WPA2, bit0 = 1,WPA; bit1 = 1, RSN */
    oal_uint8                       uc_ba_dialog_token;                         /* BA�Ự����α���ֵ */
#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    hmac_psta_stru                  st_psta;
#endif
#ifdef _PRE_WLAN_FEATURE_CUSTOM_SECURITY
    mac_blacklist_info_stru         st_blacklist_info;                          /* ��������Ϣ */
    mac_isolation_info_stru         st_isolation_info;                          /* �û�������Ϣ */
#endif
#ifdef _PRE_WLAN_FEATURE_11D
    oal_bool_enum_uint8             en_updata_rd_by_ie_switch;                  /*�Ƿ���ݹ�����ap�����Լ��Ĺ�����*/
    oal_uint8                       auc_resv2[3];
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    oal_net_device_stru            *pst_p2p0_net_device;                        /* ָ��p2p0 net device */
    oal_net_device_stru            *pst_del_net_device;                         /* ָ����Ҫͨ��cfg80211 �ӿ�ɾ���� net device */
    oal_work_stru                   st_del_virtual_inf_worker;                  /* ɾ��net_device �������� */
#endif
#ifdef _PRE_WLAN_FEATURE_HS20
    hmac_cfg_qos_map_param_stru     st_cfg_qos_map_param;
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
    oal_netbuf_head_stru            st_tx_queue_head[2];                        /* 2�����Ͷ��У�2���߳�pinpon���� */
    oal_uint8                       uc_in_queue_id;
    oal_uint8                       uc_out_queue_id;
    oal_uint8                       auc_resv3[2];
    oal_atomic                      ul_tx_event_num;                            /* frw�����¼��ĸ��� */
    oal_uint32                      ul_tx_quata;                                /* �������������� */
    oal_spin_lock_stru              st_smp_lock;
#endif

#ifdef _PRE_WLAN_FEATURE_ALWAYS_TX
    oal_uint8                       bit_init_flag:1;                            /* �����ر��ٴδ򿪱�־ */
    oal_uint8                       bit_ack_policy:1;                           /* ack policy: 0:normal ack 1:normal ack */
    oal_uint8                       bit_reserved:6;
    oal_uint8                       auc_resv4[3];
#endif
#ifdef _PRE_WLAN_FEATURE_ROAM
    oal_uint32                     *pul_roam_info;
#endif  //_PRE_WLAN_FEATURE_ROAM
    /* �鲥ת�����ֶ� */
#ifdef _PRE_WLAN_FEATURE_MCAST
    oal_void                        *pst_m2u;
#endif

#ifdef _PRE_WLAN_DFT_STAT
    oal_uint8                       uc_device_distance;
    oal_uint8                       uc_intf_state_cca;
    oal_uint8                       uc_intf_state_co;
    oal_uint8                       auc_resv[1];
#endif

    /* sta�����ֶ� */
    oal_uint8                       bit_sta_protocol_cfg    :   1;
	oal_uint8                       bit_protocol_fall       :   1;              /* ��Э���־λ */
    oal_uint8                       bit_reassoc_flag        :   1;             /* �����������ж��Ƿ�Ϊ�ع������� */
#ifdef _PRE_WLAN_FEATURE_11K
    oal_uint8                       bit_11k_enable          :   1;
    oal_uint8                       bit_11v_enable          :   1;
    oal_uint8                       bit_11r_enable          :   1;
    oal_uint8                       bit_resv                :   2;
#else
    oal_uint8                       bit_resv                :   5;
#endif //_PRE_WLAN_FEATURE_11K
    oal_int8                        ac_desired_country[3];                      /* Ҫ�����AP�Ĺ����ַ�����ǰ�����ֽ�Ϊ������ĸ��������Ϊ\0 */
    oal_uint32                      ul_asoc_req_ie_len;
    oal_uint8                      *puc_asoc_req_ie_buff;

    oal_uint8                       uc_wmm_cap;                                 /* ������STA������AP�Ƿ�֧��wmm������Ϣ */
#ifdef _PRE_WLAN_FEATURE_HS20
    oal_uint8                       uc_is_interworking;                         /* ������STA������AP�Ƿ�֧��interworking���� */
    oal_uint8                       auc_resv51[3];
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
    oal_uint8                       uc_cfg_sta_pm_manual;                           /* �ֶ�����sta pm mode�ı�־ */
#else
    oal_uint8                       auc_resv5[1];
#endif
    oal_uint16                      us_rx_timeout[WLAN_WME_AC_BUTT];            /* ��ͬҵ��������ʱʱ�� */
    oal_uint16                      us_del_timeout;                             /* �೤ʱ�䳬ʱɾ��ba�Ự �����0��ɾ�� */
    mac_cfg_mode_param_stru         st_preset_para;                             /* STAЭ����ʱ���ǰ��Э��ģʽ */
    oal_uint8                       auc_supp_rates[WLAN_MAX_SUPP_RATES];        /* ֧�ֵ����ʼ� */
    oal_uint8                       uc_rs_nrates;   /* ���ʸ��� */

    oal_uint8                       uc_auth_cnt;                                    /* ��¼STA��������Ĵ��� */
    oal_uint8                       uc_asoc_cnt;
    oal_uint8                       auc_resv56[2];

    oal_dlist_head_stru             st_pmksa_list_head;

	/* ��Ϣ�ϱ� */
    oal_wait_queue_head_stru         query_wait_q;                              /*��ѯ�ȴ�����*/
    oal_station_info_stru            station_info;
    station_info_extend_stru         st_station_info_extend;                    /*CHR2.0ʹ�õ�STAͳ����Ϣ*/
    oal_bool_enum_uint8              station_info_query_completed_flag;         /*��ѯ������־��OAL_TRUE����ѯ������OAL_FALSE����ѯδ����*/
    oal_int16                        s_free_power;                              /* ���� */
    oal_uint8                        auc_resv6[1];
    oal_int32                        center_freq;                               /* ����Ƶ�� */
    hmac_atcmdsrv_get_stats_stru     st_atcmdsrv_get_status;

    oal_proc_dir_entry_stru         *pst_proc_dir;                              /* vap��Ӧ��procĿ¼ */

#ifdef _PRE_WLAN_DFT_STAT
    /*ͳ����Ϣ+��Ϣ�ϱ������ֶΣ��޸�����ֶΣ������޸�SDT���ܽ�����ȷ*/
    hmac_vap_query_stats_stru        st_query_stats;
#endif
#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    frw_timeout_stru                 st_edca_opt_timer;                         /* edca����������ʱ�� */
    oal_uint32                       ul_edca_opt_time_ms;                       /* edca����������ʱ������ */
    oal_uint8                        uc_edca_opt_flag_ap;                       /* apģʽ���Ƿ�ʹ��edca�Ż����� */
    oal_uint8                        uc_edca_opt_flag_sta;                      /* staģʽ���Ƿ�ʹ��edca�Ż����� */
    oal_uint8                        uc_edca_opt_weight_sta;                    /* ����beacon��edca������Ȩ�أ����ֵΪ 3*/
    oal_uint8                        auc_resv7[1];
#endif

    oal_uint32                        aul_40M_intol_user_bitmap[MAC_DEV_MAX_40M_INTOL_USER_BITMAP_LEN];        /* ap�¹�����40M intolerant��sta bitmap */
    frw_timeout_stru                  st_40M_recovery_timer;                    /* 40M�ָ���ʱ�� */
    wlan_channel_bandwidth_enum_uint8 en_40M_bandwidth;                         /* ��¼ap���л���20M֮ǰ������ */

#ifdef _PRE_WLAN_TCP_OPT
    hmac_tcp_ack_stru          st_hamc_tcp_ack[HCC_DIR_COUNT];
#endif

    oal_bool_enum_uint8               en_no_beacon;
    oal_bool_enum_uint8               en_addr_filter;
    oal_bool_enum_uint8               en_amsdu_active;
    oal_bool_enum_uint8               en_amsdu_ampdu_active;
    oal_bool_enum_uint8               en_psm_active;
    oal_bool_enum_uint8               en_wme_active;
    oal_bool_enum_uint8               en_wps_active;
    oal_bool_enum_uint8               en_msdu_defrag_active;
    oal_bool_enum_uint8               en_2040_switch_prohibited;
    oal_bool_enum_uint8               en_tx_aggr_on;
    oal_bool_enum_uint8               en_ampdu_tx_on_switch;
#ifdef _PRE_WLAN_FEATURE_AMPDU_VAP
    oal_uint8                         uc_rx_ba_session_num;                   /* ��vap�£�rx BA�Ự����Ŀ */
    oal_uint8                         uc_tx_ba_session_num;                   /* ��vap�£�tx BA�Ự����Ŀ */
    oal_uint8                         auc_resv9[2];
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    mac_h2d_protection_stru          st_prot;
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
    frw_timeout_stru                 st_ps_sw_timer;                             /* �͹��Ŀ��� */
#endif

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN                                /* ҵ��ʶ���ܿ��� */
    oal_uint8                        uc_tx_traffic_classify_flag;
    oal_uint8                        auc_resv10[3];
#endif

#ifdef _PRE_WLAN_FEATURE_HILINK
    hmac_fbt_mgmt_stru               st_fbt_mgmt;                               /* ��¼fbt������Ϣ����ֹ�����б� */
#endif

#ifdef _PRE_WLAN_FEATURE_EQUIPMENT_TEST
    hmac_hipriv_ack_stats_stru       st_hipriv_ack_stats;
#endif
#ifdef _PRE_WLAN_FEATURE_SMARTANT
    oal_bool_enum_uint8             en_ant_info_query_completed_flag;           /*��ѯ������־��OAL_TRUE����ѯ������OAL_FALSE����ѯδ����*/
    oal_bool_enum_uint8             en_double_ant_switch_query_completed_flag;  /*��ѯ������־��OAL_TRUE����ѯ������OAL_FALSE����ѯδ����*/
    oal_uint8                       auc_rsv[2];
    oal_uint32                      ul_double_ant_switch_ret;
#endif
    mac_vap_stru                    st_vap_base_info;                           /* MAC vap��ֻ�ܷ������! */
}hmac_vap_stru;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/



/*****************************************************************************
  10 ��������
*****************************************************************************/
extern oal_uint32  hmac_vap_destroy(hmac_vap_stru *pst_vap);
extern oal_uint32  hmac_vap_init(
                       hmac_vap_stru              *pst_hmac_vap,
                       oal_uint8                   uc_chip_id,
                       oal_uint8                   uc_device_id,
                       oal_uint8                   uc_vap_id,
                       mac_cfg_add_vap_param_stru *pst_param);

extern oal_uint32  hmac_vap_creat_netdev(hmac_vap_stru *pst_hmac_vap, oal_int8 *puc_netdev_name, oal_int8 *puc_mac_addr);

extern oal_uint16 hmac_vap_check_ht_capabilities_ap(
            hmac_vap_stru                  *pst_hmac_vap,
            oal_uint8                      *puc_payload,
            oal_uint16                      us_info_elem_offset,
            oal_uint32                      ul_msg_len,
            hmac_user_stru                 *pst_hmac_user_sta);
extern  oal_uint32  hmac_search_ht_cap_ie_ap(
                hmac_vap_stru               *pst_hmac_vap,
                hmac_user_stru              *pst_hmac_user_sta,
                oal_uint8                   *puc_payload,
                oal_uint16                   us_index,
                oal_bool_enum                en_prev_asoc_ht);
extern oal_uint16 hmac_vap_check_vht_capabilities_ap(
                hmac_vap_stru                   *pst_hmac_vap,
                oal_uint8                       *puc_payload,
                oal_uint16                       us_info_elem_offset,
                oal_uint32                       ul_msg_len,
                hmac_user_stru                  *pst_hmac_user_sta);
extern oal_bool_enum_uint8 hmac_vap_addba_check(
                hmac_vap_stru      *pst_hmac_vap,
                hmac_user_stru     *pst_hmac_user,
                oal_uint8           uc_tidno);

extern oal_void hmac_vap_net_stopall(oal_void);
extern oal_void hmac_vap_net_startall(oal_void);

#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
extern oal_bool_enum_uint8 hmac_flowctl_check_device_is_sta_mode(oal_void);
extern oal_void hmac_vap_net_start_subqueue(oal_uint16 us_queue_idx);
extern oal_void hmac_vap_net_stop_subqueue(oal_uint16 us_queue_idx);
#endif

#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
extern oal_uint32 hmac_check_opmode_notify(
                hmac_vap_stru                   *pst_hmac_vap,
                oal_uint8                       *puc_mac_hdr,
                oal_uint8                       *puc_payload,
                oal_uint16                       us_info_elem_offset,
                oal_uint32                       ul_msg_len,
                hmac_user_stru                  *pst_hmac_user);
#endif
extern oal_void hmac_handle_disconnect_rsp(hmac_vap_stru *pst_hmac_vap, hmac_user_stru *pst_hmac_user,
                                                  mac_reason_code_enum_uint16  en_disasoc_reason);
extern oal_uint8 * hmac_vap_get_pmksa(hmac_vap_stru *pst_hmac_vap, oal_uint8 *puc_bssid);
oal_uint32 hmac_tx_get_mac_vap(oal_uint8 uc_vap_id, mac_vap_stru **pst_vap_stru);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hmac_vap.h */
