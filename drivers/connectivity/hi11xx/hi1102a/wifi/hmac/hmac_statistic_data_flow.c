


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include  "oal_net.h"
#include  "oal_types.h"
#include  "oam_ext_if.h"
#include  "oal_schedule.h"
#include  "mac_vap.h"
#include  "mac_resource.h"
#include  "hmac_vap.h"
#include  "hmac_statistic_data_flow.h"
#include  "hmac_ext_if.h"
#include  "hmac_blockack.h"
#include  "hmac_tx_data.h"
#include  "hmac_config.h"
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_pm_wlan.h"
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/pm_qos.h>
#endif


#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_MAC_STATISTIC_DATA_FLOW_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
OAL_STATIC wifi_txrx_pkt_statis_stru g_st_wifi_rxtx_statis = {0};
extern hmac_rxdata_thread_stru     g_st_rxdata_thread;
statis_wifi_load_stru g_st_wifi_load = {0};
#define STA_OPEN_TXOPLIMIT_THROUGHTPUT    280
#define STA_CLOSE_TXOPLIMIT_THROUGHTPUT   260
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ

host_speed_freq_level_stru g_host_speed_freq_level[] = {
    /*pps����                   CPU��Ƶ����                     DDR��Ƶ����*/
    {PPS_VALUE_0,          CPU_MIN_FREQ_VALUE_0,            DDR_MIN_FREQ_VALUE_0},
    {PPS_VALUE_1,          CPU_MIN_FREQ_VALUE_1,            DDR_MIN_FREQ_VALUE_1},
    {PPS_VALUE_2,          CPU_MIN_FREQ_VALUE_2,            DDR_MIN_FREQ_VALUE_2},
    {PPS_VALUE_3,          CPU_MIN_FREQ_VALUE_3,            DDR_MIN_FREQ_VALUE_3},
};
device_speed_freq_level_stru g_device_speed_freq_level[] = {
    /*device��Ƶ����*/
    {FREQ_IDLE},
    {FREQ_MIDIUM},
    {FREQ_HIGHEST},
    {FREQ_HIGHEST},
};
thread_bindcpu_stru g_st_thread_bindcpu = {OAL_TRUE, WLAN_BINDCPU_DEFAULT_MASK, WLAN_TX_BUSY_CPU_THROUGHT,WLAN_TX_IDLE_CPU_THROUGHT,WLAN_RX_BUSY_CPU_THROUGHT, WLAN_RX_IDLE_CPU_THROUGHT};
#endif
/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

oal_void hmac_wifi_statistic_rx_packets(oal_uint32 ul_pkt_count)
{
    g_st_wifi_rxtx_statis.ul_rx_pkts += ul_pkt_count;
}


oal_void hmac_wifi_statistic_tx_packets(oal_uint32 ul_pkt_count)
{
    g_st_wifi_rxtx_statis.ul_tx_pkts += ul_pkt_count;
}

oal_void hmac_wifi_statistic_rx_bytes(oal_uint32 ul_pkt_bytes)
{
    g_st_wifi_rxtx_statis.ul_rx_bytes += ul_pkt_bytes;
}

oal_void hmac_wifi_statistic_tx_bytes(oal_uint32 ul_pkt_bytes)
{
    g_st_wifi_rxtx_statis.ul_tx_bytes += ul_pkt_bytes;
}

oal_bool_enum_uint8 hmac_wifi_rx_is_busy(oal_void)
{
    return g_st_wifi_load.en_wifi_rx_busy;
}

oal_bool_enum_uint8 hmac_wifi_alg_txop_opt(oal_uint32 ul_tx_throughput_mbps)
{
    mac_ioctl_alg_param_stru     st_alg_param;
    oal_uint                     us_len          = OAL_SIZEOF(mac_ioctl_alg_param_stru);
    mac_vap_stru                *pst_mac_vap;
    oal_uint8                    uc_txop_limit_en = g_st_wifi_rxtx_statis.uc_txop_limit_en;

    st_alg_param.en_alg_cfg  = MAC_ALG_CFG_TXOP_LIMIT_STA_EN;
    st_alg_param.ul_value    = 0;

    /* UP��VAP����������STA P2P������˳�� */
    if( pst_mac_vap != OAL_PTR_NULL)
    {
        if(ul_tx_throughput_mbps > STA_OPEN_TXOPLIMIT_THROUGHTPUT)
        {
            st_alg_param.ul_value = 1;
            uc_txop_limit_en = OAL_TRUE;

        }
        else if(ul_tx_throughput_mbps < STA_CLOSE_TXOPLIMIT_THROUGHTPUT)
        {
            st_alg_param.ul_value = 0;
            uc_txop_limit_en = OAL_FALSE;
        }
    }

    if ( uc_txop_limit_en != g_st_wifi_rxtx_statis.uc_txop_limit_en)
    {
        /* �·��رն�̬����txop�¼� */
        hmac_config_alg_send_event(pst_mac_vap, WLAN_CFGID_ALG_PARAM, us_len, (oal_uint8 *)&st_alg_param);
        OAM_WARNING_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "hmac_wifi_alg_txop_opt: txop opt open[%d],tx[%d]Mbps", st_alg_param.ul_value,ul_tx_throughput_mbps);
    }

    g_st_wifi_rxtx_statis.uc_txop_limit_en = uc_txop_limit_en;
    return OAL_TRUE;
}

#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1102A_HOST)

OAL_STATIC oal_void hmac_thread_set_allowed_cpu(struct cpumask *pst_cpus_mask)
{
#if !defined(WIN32)
    set_cpus_allowed_ptr(g_st_rxdata_thread.pst_rxdata_thread, pst_cpus_mask);
    set_cpus_allowed_ptr(hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread, pst_cpus_mask);
    set_cpus_allowed_ptr(hcc_get_110x_handler()->bus_dev->cur_bus->pst_rx_tsk, pst_cpus_mask);
#endif
}


OAL_STATIC oal_void hmac_thread_bind_fast_cpu(oal_void)
{
#if defined(CONFIG_ARCH_HISI)
#ifdef CONFIG_NR_CPUS
#if CONFIG_NR_CPUS > OAL_BUS_HPCPU_NUM
    oal_uint8 uc_cpumask;
    oal_uint8 uc_cpuid = 0;
    struct cpumask cpus_mask;

    uc_cpumask = g_st_thread_bindcpu.uc_cpumask;
    cpumask_clear(&cpus_mask);

    while (uc_cpumask)
    {
        if (uc_cpumask & 0x1)
        {
            cpumask_set_cpu(uc_cpuid, &cpus_mask);
        }

        uc_cpuid++;
        uc_cpumask = uc_cpumask >> 1;
    }

    hmac_thread_set_allowed_cpu(&cpus_mask);
#endif
#endif
#endif
}


OAL_STATIC oal_void hmac_thread_bind_slow_cpu(oal_void)
{
#if defined(CONFIG_ARCH_HISI)
#ifdef CONFIG_NR_CPUS
#if CONFIG_NR_CPUS > OAL_BUS_HPCPU_NUM
    struct cpumask cpus_mask;

    hisi_get_slow_cpus(&cpus_mask);
    cpumask_clear_cpu(0, &cpus_mask);

    hmac_thread_set_allowed_cpu(&cpus_mask);
#endif
#endif
#endif
}

oal_void hmac_thread_bindcpu(oal_uint32 ul_tx_throughput_mbps, oal_uint32 ul_rx_throughput_mbps)
{
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ

    oal_uint8 uc_req_irq_cpu;

    if (!g_st_thread_bindcpu.en_irq_bindcpu)
    {
        return;
    }

    /* ���������������ģʽ */
    if ((ul_tx_throughput_mbps > g_st_thread_bindcpu.us_tx_throughput_irq_high) ||
        (ul_rx_throughput_mbps > g_st_thread_bindcpu.us_rx_throughput_irq_high))
    {
        uc_req_irq_cpu = WLAN_IRQ_AFFINITY_BUSY_CPU;
    }
    else if ((ul_tx_throughput_mbps < g_st_thread_bindcpu.us_tx_throughput_irq_low) &&
        (ul_rx_throughput_mbps < g_st_thread_bindcpu.us_rx_throughput_irq_low))
    {
        uc_req_irq_cpu = WLAN_IRQ_AFFINITY_IDLE_CPU;
    }
    else
    {
        return;
    }

    /* ���״̬�䶯ʱ����Ҫִ�а�˶��� */
    if (uc_req_irq_cpu != g_st_wifi_rxtx_statis.uc_req_irq_cpu)
    {
        OAM_WARNING_LOG3(0,OAM_SF_ANY, "{hmac_rxdata_thread_bindcpu:rxdata_thread bind cpu:[%d] rx = %d , tx = %d}",
            uc_req_irq_cpu, ul_rx_throughput_mbps, ul_tx_throughput_mbps);

        /* �����ϴ�ʱ���շ��̰߳��ڴ�� */
        if (WLAN_IRQ_AFFINITY_BUSY_CPU == uc_req_irq_cpu)
        {
            hmac_thread_bind_fast_cpu();
        }
        else
        {
            hmac_thread_bind_slow_cpu();
        }

        /* ����״̬ */
        g_st_wifi_rxtx_statis.uc_req_irq_cpu = uc_req_irq_cpu;
    }
#endif
}
#endif


oal_void hmac_wifi_calculate_throughput(oal_void)
{
    oal_uint32          ul_trx_total = 0;
    oal_uint32          ul_cur_time;
    oal_uint32          ul_dur_ms;
    oal_uint32          ul_rx_throughput_mbps = 0;
    oal_uint32          ul_tx_throughput_mbps = 0;

    /* ��ʱ��ѭ������ͳ�� */
    g_st_wifi_rxtx_statis.uc_timer_cycles++;

    /* ���շ�������ͳ�� */
    ul_trx_total = g_st_wifi_rxtx_statis.ul_rx_pkts + g_st_wifi_rxtx_statis.ul_tx_pkts;

    ul_cur_time = (oal_uint32)OAL_TIME_GET_STAMP_MS();
    ul_dur_ms = OAL_TIME_GET_RUNTIME(g_st_wifi_rxtx_statis.ul_pre_time, ul_cur_time);

    /* �����ǰͳ��ʱ�䲻�㶨ʱ�����ڵ�һ��,�ᵼ��ͳ��PPSֵƫ�󷵻� */
    if ((0 == ul_dur_ms) || (ul_dur_ms < (WLAN_STATIS_DATA_TIMER_PERIOD >> 1)))
    {
        return ;
    }

    /* ����һ��ʱ����ͳ�ƣ��ų���ʱ���쳣 */
    if(ul_dur_ms > (WLAN_STATIS_DATA_TIMER_PERIOD * WLAN_THROUGHPUT_STA_PERIOD) << 2)
    {
        g_st_wifi_rxtx_statis.ul_pre_time = (oal_uint32)OAL_TIME_GET_STAMP_MS();
        g_st_wifi_rxtx_statis.uc_timer_cycles = 0;
        g_st_wifi_rxtx_statis.ul_rx_pkts = 0;
        g_st_wifi_rxtx_statis.ul_tx_pkts = 0;
        g_st_wifi_rxtx_statis.ul_rx_bytes = 0;
        g_st_wifi_rxtx_statis.ul_tx_bytes = 0;

        return;
    }

    /* �������ݰ�����������PPS */
    g_st_wifi_rxtx_statis.ul_total_sdio_pps = (ul_trx_total * 1000) / ul_dur_ms;
    g_st_wifi_rxtx_statis.ul_tx_pps         = (g_st_wifi_rxtx_statis.ul_tx_pkts * 1000) / ul_dur_ms;
    g_st_wifi_rxtx_statis.ul_rx_pps         = (g_st_wifi_rxtx_statis.ul_rx_pkts * 1000) / ul_dur_ms;

    /* �������ֽ�������mbps */
    ul_rx_throughput_mbps = (g_st_wifi_rxtx_statis.ul_rx_bytes >> 7) / ul_dur_ms;
    ul_tx_throughput_mbps = (g_st_wifi_rxtx_statis.ul_tx_bytes >> 7) / ul_dur_ms;

    /* Wi-Fi ҵ���ر�� */
    if(ul_rx_throughput_mbps <= WLAN_THROUGHPUT_LOAD_LOW)
    {
        g_st_wifi_load.en_wifi_rx_busy = OAL_FALSE;
    }
    else
    {
        g_st_wifi_load.en_wifi_rx_busy = OAL_TRUE;
    }

#ifdef _PRE_WLAN_FEATURE_MULTI_NETBUF_AMSDU
    /* AMSDU+AMPDU�ۺ��л���� */
    hmac_tx_amsdu_ampdu_switch(ul_tx_throughput_mbps);
#endif
    hmac_tx_small_amsdu_switch(ul_rx_throughput_mbps, g_st_wifi_rxtx_statis.ul_tx_pps);

#ifdef _PRE_WLAN_FEATURE_TCP_ACK_BUFFER
    hmac_tx_tcp_ack_buf_switch(ul_rx_throughput_mbps);
#endif

#ifdef _PRE_WLAN_TCP_OPT
    /* ����rx����Mbps ������ ����/�ر� TCP_ACK filter�Ż����� */
    hmac_tcp_ack_opt_switch_ctrol(ul_rx_throughput_mbps);
#endif

#ifdef _PRE_WLAN_FEATURE_DYN_BYPASS_EXTLNA
    /* �������������ж��Ƿ���Ҫbypass ����LNA */
    hmac_rx_dyn_bypass_extlna_switch(ul_tx_throughput_mbps, ul_rx_throughput_mbps);
#endif

#if(_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1102A_HOST)
    hmac_thread_bindcpu(ul_tx_throughput_mbps, ul_rx_throughput_mbps);
#endif

    //hmac_wifi_alg_txop_opt(ul_tx_throughput_mbps);

    /* irq 2s ̽��һ�� */
    if(g_st_wifi_rxtx_statis.uc_timer_cycles < WLAN_THROUGHPUT_STA_PERIOD)
    {
        return;
    }

    /* ƽ��������ÿ2s���һ���Ƿ���Ҫ�޸ĵ͹��Ķ�ʱ����ֵ */
    hmac_set_psm_activity_timer(g_st_wifi_rxtx_statis.ul_total_sdio_pps);

    /* 2s��������һ�� */
    g_st_wifi_rxtx_statis.uc_timer_cycles = 0;
    g_st_wifi_rxtx_statis.ul_rx_bytes = 0;
    g_st_wifi_rxtx_statis.ul_tx_bytes = 0;
    g_st_wifi_rxtx_statis.ul_rx_pkts  = 0;
    g_st_wifi_rxtx_statis.ul_tx_pkts  = 0;

    g_st_wifi_rxtx_statis.ul_pre_time = (oal_uint32)OAL_TIME_GET_STAMP_MS();

    return;
}


oal_void hmac_wifi_statis_data_timeout(oal_void)
{
    hmac_wifi_calculate_throughput();
}


oal_void  hmac_wifi_statis_data_timer_init(oal_void)
{

    /*��������ʱ���Ѿ�ע��ɹ��������ٴα�ע�ᣡ*/
    if (OAL_TRUE == g_st_wifi_rxtx_statis.st_statis_data_timer.en_is_registerd
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    || OAL_FALSE == wlan_pm_is_poweron()
#endif
    )
    {
         return;
    }

    /* ���ͳ�� */
    OAL_MEMZERO(&g_st_wifi_rxtx_statis, (OAL_SIZEOF(g_st_wifi_rxtx_statis) - OAL_SIZEOF(g_st_wifi_rxtx_statis.st_statis_data_timer)));

    FRW_TIMER_CREATE_TIMER(&g_st_wifi_rxtx_statis.st_statis_data_timer, //pst_timeout
                        (oal_void*)hmac_wifi_statis_data_timeout,   //p_timeout_func
                        WLAN_STATIS_DATA_TIMER_PERIOD,             //ul_timeout
                        OAL_PTR_NULL,                          // p_timeout_arg
                        OAL_TRUE,                             //en_is_periodic
                        OAM_MODULE_ID_HMAC,0 );             //en_module_id && ul_core_id

}


oal_void  hmac_wifi_statis_data_timer_deinit(oal_void)
{
    if (OAL_FALSE == g_st_wifi_rxtx_statis.st_statis_data_timer.en_is_registerd)
    {
        return;
    }

    FRW_TIMER_IMMEDIATE_DESTROY_TIMER(&g_st_wifi_rxtx_statis.st_statis_data_timer);

}

oal_void  hmac_wifi_pm_state_notify(oal_bool_enum_uint8 en_wake_up)
{
    if (OAL_TRUE == en_wake_up)
    {
        /* WIFI����,����������ͳ�ƶ�ʱ�� */
        hmac_wifi_statis_data_timer_init();
    }
    else
    {
        /* WIFI˯��,�ر�������ͳ�ƶ�ʱ�� */
        hmac_wifi_statis_data_timer_deinit();
    }

}


oal_void  hmac_wifi_state_notify(oal_bool_enum_uint8 en_wifi_on)
{
    if (OAL_TRUE == en_wifi_on)
    {
        /* WIFI�ϵ�,����������ͳ�ƶ�ʱ��,�򿪵�Ƶ���� */
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
        hmac_set_device_freq_mode(H2D_FREQ_MODE_ENABLE);
#endif
        /* WIFI��ʱ,��ʱ���������� */
        g_st_wifi_rxtx_statis.ul_pre_time = (oal_uint32)OAL_TIME_GET_STAMP_MS();

        hmac_wifi_statis_data_timer_init();
    }
    else
    {
        /* WIFI�µ�,�ر�������ͳ�ƶ�ʱ��,�رյ�Ƶ���� */
        hmac_wifi_statis_data_timer_deinit();

#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
        hmac_set_device_freq_mode(H2D_FREQ_MODE_DISABLE);
#endif
    }

}

#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


