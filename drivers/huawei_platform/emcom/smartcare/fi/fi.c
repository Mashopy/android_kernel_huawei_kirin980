

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/udp.h>
#include <net/sock.h>
#include <linux/timer.h>
#include <linux/version.h>
#include "../../emcom_netlink.h"
#include "../../emcom_utils.h"
#include <huawei_platform/emcom/smartcare/fi/fi_utils.h>

fi_ctx g_fi_ctx;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,1)
	static const struct nf_hook_ops fi_nfhooks[] = {
#else
	static struct nf_hook_ops fi_nfhooks[] = {
#endif
	{
		.hook        = fi_hook_out,                 /* �ص����� */
		.pf          = PF_INET,                     /* nfЭ���� */
		.hooknum     = NF_INET_LOCAL_OUT,           /* ��ʾ���������ı��� */
		.priority    = NF_IP_PRI_FILTER - 1,        /* ���ȼ� */
	},
	{
		.hook        = fi_hook_in,                  /* �ص����� */
		.pf          = PF_INET,                     /* nfЭ���� */
		.hooknum     = NF_INET_LOCAL_IN,            /* ��ʾĿ���ַ�Ǳ����ı��� */
		.priority    = NF_IP_PRI_FILTER - 1,        /* ���ȼ� */
	},
};


void fi_mem_used(uint32_t memlen)
{
	g_fi_ctx.memused += memlen;
}


void fi_mem_de_used(uint32_t memlen)
{
	g_fi_ctx.memused -= memlen;
}


void fi_free(void *ptr, uint32_t size)
{
	kfree(ptr);
	fi_mem_de_used(size);
	return;
}


void *fi_malloc(uint32_t size)
{
	void *ptr;

	ptr = kmalloc(size, GFP_ATOMIC);
	if (!ptr)
	{
		return NULL;
	}

	fi_mem_used(size);
	memset(ptr, 0, size);

	return ptr;
}


void fi_register_nf_hook(void)
{
	int ret = 0;

	/* ���� */
	mutex_lock(&(g_fi_ctx.nf_mutex));

	/* ����Ѿ����ڹ��Ӻ������Ͳ���ע�� */
	if (g_fi_ctx.nf_exist == FI_TRUE)
	{
		mutex_unlock(&(g_fi_ctx.nf_mutex));
		return;
	}

	/* ע��netfilter���Ӻ��� */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,1)
	ret = nf_register_net_hooks(&init_net, fi_nfhooks, ARRAY_SIZE(fi_nfhooks));
#else
	ret = nf_register_hooks(fi_nfhooks, ARRAY_SIZE(fi_nfhooks));
#endif
	if (ret == 0)
	{
		/* ���ע��ɹ�����Ҫ���ñ�� */
		g_fi_ctx.nf_exist = FI_TRUE;

		/* �й��Ӿ��ж�ʱ�� */
		g_fi_ctx.tm.expires = jiffies + HZ / FI_TIMER_INTERVAL;
		add_timer(&g_fi_ctx.tm);
	}

	/* �����ͷ��� */
	mutex_unlock(&(g_fi_ctx.nf_mutex));

	/* �ͷ���֮���ټ�¼��־ */
	if (ret)
	{
		FI_LOGE(" : FI register nf hooks failed, ret=%d", ret);
	}
	else
	{
		FI_LOGD(" : FI register nf hooks successfully");
	}

	return;
}


void fi_unregister_nf_hook(void)
{
	/* ���� */
	mutex_lock(&(g_fi_ctx.nf_mutex));

	/* ��������ڹ��Ӻ�����ֱ�ӷ���*/
	if (g_fi_ctx.nf_exist == FI_FALSE)
	{
		mutex_unlock(&(g_fi_ctx.nf_mutex));
		return;
	}

	/* ɾ��netfilter���Ӻ��� */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,1)
	nf_unregister_net_hooks(&init_net, fi_nfhooks, ARRAY_SIZE(fi_nfhooks));
#else
	nf_unregister_hooks(fi_nfhooks, ARRAY_SIZE(fi_nfhooks));
#endif

	/* ɾ����ʱ��, ��ʱ���빳�Ӻ���ͬʱ����ͬʱɾ�� */
	del_timer(&g_fi_ctx.tm);

	g_fi_ctx.nf_exist = FI_FALSE;

	mutex_unlock(&(g_fi_ctx.nf_mutex));

	FI_LOGD(" : FI unregister nf hooks successfully.");

	return;
}


fi_app_info *fi_find_appinfo(uint32_t uid)
{
	int i;

	if (uid == 0)
	{
		return NULL;
	}

	/* ����uid */
	for (i = 0; i < FI_APPID_MAX; i++)
	{
		if (g_fi_ctx.appinfo[i].uid == uid)
		{
			return &(g_fi_ctx.appinfo[i]);
		}
	}

	return NULL;
}


static uint32_t fi_find_appid(uint32_t uid)
{
	int i;

	if (uid == 0)
	{
		return FI_APPID_NULL;
	}

	/* ֻ��appidmin��appidmax�ķ�Χ�ڲ��ң�����ȫ���������������� */
	for (i = g_fi_ctx.appidmin; i <= g_fi_ctx.appidmax; i++)
	{
		if ((g_fi_ctx.appinfo[i].uid == uid) && (g_fi_ctx.appinfo[i].valid))
		{
			return g_fi_ctx.appinfo[i].appid;
		}
	}

	return FI_APPID_NULL;
}


static inline int fi_pkt_check(struct sk_buff *skb, int dir)
{
	struct iphdr *iph = ip_hdr(skb);

	/* ֻ��עip���� */
	if (!iph || skb->len <= FI_MIN_IP_PKT_LEN)
	{
		return FI_FAILURE;
	}

	/* ֻ��עipv4�ı��� */
	if (iph->version != FI_IP_VER_4)
	{
		return FI_FAILURE;
	}

	/* ����ע���ؽӿڵı��� */
	if ((*(uint8_t *)&(iph->saddr) == FI_LOOP_ADDR) ||
	    (*(uint8_t *)&(iph->daddr) == FI_LOOP_ADDR))
	{
		return FI_FAILURE;
	}

	if (iph->protocol == FI_IPPROTO_UDP)
	{
		if (skb->len < sizeof(struct udphdr) + sizeof(struct iphdr))
		{
			return FI_FAILURE;
		}
	}
	else if (iph->protocol == FI_IPPROTO_TCP)
	{
		if (skb->len < sizeof(struct tcphdr) + sizeof(struct iphdr))
		{
			return FI_FAILURE;
		}
	}
	else
	{
		return FI_FAILURE;
	}

	return FI_SUCCESS;
}


static inline int fi_parse_mptcp(fi_pkt *pktinfo, struct tcphdr *tcph, uint32_t hdrlen)
{
	uint8_t   optkind = 0;      /* tcpѡ�͵����� */
	uint8_t   optlen  = 0;      /* tcpѡ��ĳ��� */
	uint32_t  leftlen = hdrlen - sizeof(struct tcphdr);
	uint8_t  *leftdata = (uint8_t *)tcph + sizeof(struct tcphdr);
	uint32_t *seqack;
	fi_mptcp_dss *mptcpdss;

	while (leftlen > FI_TCP_OPT_HDR_LEN)
	{
		optkind = *leftdata;
		optlen  = *(leftdata + 1);

		leftdata += FI_TCP_OPT_HDR_LEN;
		leftlen  -= FI_TCP_OPT_HDR_LEN;

		/* �ҵ�mptcpѡ�� */
		if (optkind == FI_TCP_OPT_MPTCP)
		{
			break;
		}
	}

	/* ����mptcp���� */
	if (optkind != FI_TCP_OPT_MPTCP)
	{
		return FI_SUCCESS;
	}

	/* ��ʼ����mptcpѡ��, ���ȼ�����ݳ��� */
	if ((leftlen + 2 < optlen) || (optlen < FI_MPTCP_DSS_MINLEN))
	{
		/* ����ѡ����� */
		return FI_SUCCESS;
	}

	/* ������dssѡ�� */
	mptcpdss = (fi_mptcp_dss *)leftdata;
	if (mptcpdss->subtype != FI_MPTCP_SUBTYPE_DSS)
	{
		return FI_SUCCESS;
	}

	/* �۳�dss�ײ����� */
	leftdata += sizeof(fi_mptcp_dss);
	leftlen  -= sizeof(fi_mptcp_dss);

	/* �ٴμ��㲢���dssӦ���еĳ��� */
	optlen = sizeof(uint32_t) * (mptcpdss->seq8 + mptcpdss->seqpre +
	                             mptcpdss->ack8 + mptcpdss->ackpre);
	if (leftlen < optlen)
	{
		return FI_SUCCESS;
	}

	/* �����˵��һ��������ȡ��seq��ack�����еı��Ĳ���seq������������ */
	pktinfo->seq = 0;
	pktinfo->ack = 0;
	pktinfo->mptcp = FI_TRUE;

	/* ��ȡack */
	if (mptcpdss->ackpre && (leftlen >= sizeof(uint32_t)))
	{
		seqack = (uint32_t *)leftdata;
		pktinfo->ack = ntohl(*seqack);

		leftdata += sizeof(uint32_t);
		leftlen  -= sizeof(uint32_t);

		/* �����8�ֽڵ�ack��ֻȡ��4�ֽھ����� */
		if (mptcpdss->ack8 && (leftlen >= sizeof(uint32_t)))
		{
			seqack = (uint32_t *)leftdata;
			pktinfo->ack = ntohl(*seqack);

			leftdata += sizeof(uint32_t);
			leftlen  -= sizeof(uint32_t);
		}
	}

	/* ��ȡseq */
	if (mptcpdss->seqpre && (leftlen >= sizeof(uint32_t)))
	{
		seqack = (uint32_t *)leftdata;
		pktinfo->seq = ntohl(*seqack);

		leftdata += sizeof(uint32_t);
		leftlen  -= sizeof(uint32_t);

		/* �����8�ֽڵ�seq��ֻȡ��4�ֽھ����� */
		if (mptcpdss->seq8 && (leftlen >= sizeof(uint32_t)))
		{
			seqack = (uint32_t *)leftdata;
			pktinfo->seq = ntohl(*seqack);
		}
	}

	FI_LOGD(" : FI mptcp, seq=%u, ack=%u, flow: %u,%u.",
	        pktinfo->seq, pktinfo->ack, pktinfo->sport, pktinfo->dport);

	return FI_SUCCESS;
}


static inline int fi_pkt_parse(fi_pkt *pktinfo, struct sk_buff *skb, int dir)
{
	struct iphdr  *iph;
	struct udphdr *udph;
	struct tcphdr *tcph;
	unsigned int   iphlen = 0;      /* ip header len */
	unsigned int   l4hlen = 0;      /* tcp/udp header len */

	/* ������������������飬����һ��ΪIP���ģ�����һ������20 */
	iph    = ip_hdr(skb);
	iphlen = iph->ihl * FI_IP_HDR_LEN_BASE;

	/* UDP���Ľ��� */
	if (iph->protocol == FI_IPPROTO_UDP)
	{
		udph   = udp_hdr(skb);
		l4hlen = sizeof(struct udphdr);

		/* ���skb�б��ĵĳ��� */
		if (skb->len < iphlen + l4hlen + skb->data_len)
		{
			return FI_FAILURE;
		}

		pktinfo->data = (uint8_t *)udph + l4hlen;
		pktinfo->len = skb->len - iphlen - l4hlen;
		pktinfo->bufdatalen = skb->len - skb->data_len - iphlen - l4hlen;
		pktinfo->sport = ntohs(udph->source);
		pktinfo->dport = ntohs(udph->dest);
	}
	/* ����UDP��һ����TCP */
	else
	{
		tcph   = tcp_hdr(skb);
		l4hlen = tcph->doff * FI_TCP_HDR_LEN_BASE;

		/* ���skb�б��ĵĳ��� */
		if (skb->len < iphlen + l4hlen + skb->data_len)
		{
			return FI_FAILURE;
		}

		pktinfo->data = (uint8_t *)tcph + l4hlen;
		pktinfo->len  = skb->len - iphlen - l4hlen;
		pktinfo->bufdatalen = skb->len - skb->data_len - iphlen - l4hlen;
		pktinfo->sport = ntohs(tcph->source);
		pktinfo->dport = ntohs(tcph->dest);
		pktinfo->seq = ntohl(tcph->seq);
		pktinfo->ack = ntohl(tcph->ack_seq);

		if (fi_parse_mptcp(pktinfo, tcph, l4hlen) != FI_SUCCESS)
		{
			return FI_FAILURE;
		}
	}

	/* ֻ��ע�˿ڴ���1023�� */
	if ((pktinfo->sport < FI_BATTLE_START_PORT_MIN) ||
	    (pktinfo->dport < FI_BATTLE_START_PORT_MIN))
	{
		return FI_FAILURE;
	}

	pktinfo->proto = iph->protocol;
	pktinfo->dir   = dir;
	pktinfo->sip   = ntohl(iph->saddr);
	pktinfo->dip   = ntohl(iph->daddr);

	pktinfo->msec = jiffies_to_msecs(jiffies);

	return FI_SUCCESS;
}


void fi_timer_callback(unsigned long arg)
{
	/* �ϻ����� */
	fi_flow_age();

	/* ���ڷ���rtt, ������Ϸ״̬ */
	fi_rtt_timer();

	/* ��ʱ���´δ�����ʱ�� */
	mod_timer(&g_fi_ctx.tm, jiffies + HZ / FI_TIMER_INTERVAL);

	return;
}


static bool fi_streq(char *data, uint32_t datalen, char *str)
{
	uint32_t strLen = strlen(str);

	/* data�ַ������ж����'\0' */
	if (datalen >= strLen + 1)
	{
		if (!memcmp(data, str, strLen + 1))
		{
			return FI_TRUE;
		}
	}
	/* data������'\0'��β */
	else if (datalen == strLen)
	{
		if (!memcmp(data, str, strLen))
		{
			return FI_TRUE;
		}
	}
	else
	{
		return FI_FALSE;
	}

	return FI_FALSE;
}


static void fi_appid_add(uint32_t appid)
{
	if ((g_fi_ctx.appidmin == 0) || (appid < g_fi_ctx.appidmin))
	{
		g_fi_ctx.appidmin = appid;
	}

	if ((g_fi_ctx.appidmax == 0) || (appid > g_fi_ctx.appidmax))
	{
		g_fi_ctx.appidmax = appid;
	}
}


static void fi_appid_shrink(void)
{
	uint32_t i;

	/* appidmin��ǰ�� */
	for (i = g_fi_ctx.appidmin; (i <= g_fi_ctx.appidmax) && (i < FI_APPID_MAX); i++)
	{
		/* validΪ�ٱ�ʾ����Ϸ���˳� */
		if (!(g_fi_ctx.appinfo[i].valid))
		{
			g_fi_ctx.appidmin = i;
		}
		else
		{
			break;
		}
	}

	/* appidmax����� */
	for (i = g_fi_ctx.appidmax; (i >= g_fi_ctx.appidmin) && (i > 0); i--)
	{
		/* validΪ�ٱ�ʾ����Ϸ���˳� */
		if (!(g_fi_ctx.appinfo[i].valid))
		{
			g_fi_ctx.appidmax = i;
		}
		else
		{
			break;
		}
	}

	/* ˵���Ѿ�û��appid�� */
	i = g_fi_ctx.appidmin;
	if ((g_fi_ctx.appidmin == g_fi_ctx.appidmax) && !(g_fi_ctx.appinfo[i].valid))
	{
		g_fi_ctx.appidmin = 0;
		g_fi_ctx.appidmax = 0;
	}

	return;
}


static uint32_t fi_appname_to_appid(char *nameptr, uint32_t datalen)
{
	uint32_t appid = FI_APPID_NULL;

	if (fi_streq(nameptr, datalen, FI_APP_NAME_WZRY))
	{
		appid = FI_APPID_WZRY;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_CJZC))
	{
		appid = FI_APPID_CJZC;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_QJCJ))
	{
		appid = FI_APPID_QJCJ;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_HYXD))
	{
		appid = FI_APPID_HYXD;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_HYXD_HW))
	{
		appid = FI_APPID_HYXD;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_CYHX))
	{
		appid = FI_APPID_CYHX;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_QQFC))
	{
		appid = FI_APPID_QQFC;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_BH3) ||
	         fi_streq(nameptr, datalen, FI_APP_NAME_BH3_2) ||
	         fi_streq(nameptr, datalen, FI_APP_NAME_BH3_3) ||
	         fi_streq(nameptr, datalen, FI_APP_NAME_BH3_4) ||
	         fi_streq(nameptr, datalen, FI_APP_NAME_BH3_5))
	{
		appid = FI_APPID_BH3;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_UU))
	{
		appid = FI_APPID_UU;
	}
	else if (fi_streq(nameptr, datalen, FI_APP_NAME_XUNYOU))
	{
		appid = FI_APPID_XUNYOU;
	}
	else
	{
		appid = FI_APPID_NULL;
	}

	return appid;
}


static void fi_proc_applaunch(void *data, int32_t len)
{
	fi_msg_applaunch *msg;
	fi_app_info *appinfo;
	uint32_t appid;

	/* ������� */
	if ((!data) || (len < sizeof(fi_msg_applaunch)))
	{
		FI_LOGE(" : FI illegal data length %d in FI app launch", len);
		return;
	}

	msg  = (fi_msg_applaunch *)data;
	len -= sizeof(fi_msg_applaunch);

	/* ���ַ�����ʽ��appnameת��Ϊappid */
	appid = fi_appname_to_appid(msg->appname, len);
	if (!FI_APPID_VALID(appid))
	{
		FI_LOGE(" : FI unknown app name %s, len %u.", msg->appname, len);
		return;
	}

	fi_appid_add(appid);

	/* ����nf����, ����Ѿ����ھͲ������� */
	fi_register_nf_hook();

	/* ��ʼ��ֵ */
	appinfo = &g_fi_ctx.appinfo[appid];
	appinfo->appid = appid;
	appinfo->uid   = msg->uid;
	appinfo->valid = FI_TRUE;
	appinfo->switchs = msg->switchs;

	FI_LOGI(" : FI set app info appid=%u, uid=%u, switch=%08x",
			appinfo->appid, appinfo->uid, appinfo->switchs);

	return;
}


static void fi_proc_appstatus(void *data, int32_t len)
{
	fi_msg_appstatus *msg;
	fi_app_info *appinfo;
	uint32_t appid;

	/* ������� */
	if ((!data) || (len < sizeof(fi_msg_appstatus)))
	{
		FI_LOGE(" : FI illegal data length %d in FI set app status", len);
		return;
	}

	msg = (fi_msg_appstatus *)data;
	len -= sizeof(fi_msg_appstatus);

	appid = fi_appname_to_appid(msg->appname, len);
	if (!FI_APPID_VALID(appid))
	{
		FI_LOGE(" : FI unknown app name %s, len %u.", msg->appname, len);
		return;
	}

	FI_LOGI(" : FI recv app status appid=%u, appstatus=%u", appid, msg->appstatus);

	/* ��Ҫ����һ��uid����¼app��ǰ��״̬ */
	appinfo = &g_fi_ctx.appinfo[appid];
	appinfo->uid = msg->uid;
	appinfo->appstatus = msg->appstatus;

	/* �������Ϸ�˳� */
	if (msg->appstatus == GAME_SDK_STATE_DIE)
	{
		fi_gamectx *gamectx;

		/* ��Ҫ���uid, �����ں��߳̿��ܻ����õ�, �������Ϊfalse���� */
		appinfo->valid = FI_FALSE;

		gamectx = g_fi_ctx.gamectx + appid;

		/* ���Ͷ�ս���� */
		if (FI_BATTLING(gamectx->appstatus))
		{
			fi_rtt_status(gamectx->appid, FI_STATUS_BATTLE_STOP);
		}

		/* ���rtt���� */
		memset(gamectx, 0, sizeof(fi_gamectx));

		/* ����appid��Χ */
		fi_appid_shrink();

		/* ���û��appid, ��ɾ��nf���� */
		if (FI_HAS_NO_APPID(g_fi_ctx))
		{
			fi_unregister_nf_hook();
		}

		FI_LOGI(" : FI clear cache because of app exit, uid=%u", msg->uid);
	}

	return;
}


void fi_reflect_status(int32_t event, uint8_t *data, uint16_t len)
{
	fi_msg_appstatus *msg;
	fi_report_status report = {0};
	uint32_t status = 0;

	if ((!data) || (len < sizeof(fi_msg_appstatus)))
	{
		return;
	}

	msg = (fi_msg_appstatus *)data;

	/* �������Ϸ���أ���Ϣ���͹̶�ΪGAME_SDK_STATE_FOREGROUND */
	if (event == NETLINK_EMCOM_DK_SMARTCARE_FI_APP_LAUNCH)
	{
		status = GAME_SDK_STATE_FOREGROUND;
	}
	else if (event == NETLINK_EMCOM_DK_SMARTCARE_FI_APP_STATUS)
	{
		status = msg->appstatus;
	}
	else
	{
		return;
	}

	/* �ϱ���Ϸ״̬ */
	report.uid = msg->uid;
	report.status = status;
	report.apptype = FI_APP_TYPE_GAME;
	emcom_send_msg2daemon(NETLINK_EMCOM_KD_SMARTCARE_FI_REPORT_APP_STATUS, &report, sizeof(report));

	FI_LOGD(" : FI reflect status to daemon, uid=%u, status=%u.", report.uid, status);

	return;
}


void fi_event_process(int32_t event, uint8_t *pdata, uint16_t len)
{
	FI_LOGD(" : FI received cmd %d, datalen %u.", event, len);

	/* ���ݲ�ͬ����Ϣid���в�ͬ�Ĵ��� */
	switch (event)
	{
		/* ��Ϸ���� */
		case NETLINK_EMCOM_DK_SMARTCARE_FI_APP_LAUNCH:
			fi_proc_applaunch(pdata, len);
			break;

		/* ��Ϸ״̬�仯 */
		case NETLINK_EMCOM_DK_SMARTCARE_FI_APP_STATUS:
			fi_proc_appstatus(pdata, len);
			break;

		default:
			FI_LOGE(" : FI received unsupported message");
			break;
	}

	/* ����Ϸ״̬�仯��Ϣͬʱ���͵�framework */
	fi_reflect_status(event, pdata, len);

	return;
}


static void fi_hook(struct sk_buff *skb, int dir)
{
	fi_flow_head *head;
	fi_flow_node *flow;
	fi_pkt pktinfo = {0};
	struct sock *sk;
	struct iphdr *iph = ip_hdr(skb);
	kuid_t   kuid = {0};
	uint32_t hash;
	uint32_t appid = 0;
	uint32_t addflow = FI_FALSE;

	if (unlikely(!skb))
	{
		return;
	}

	/* ���˳�ipv4 udp&tcp */
	if (fi_pkt_check(skb, dir) != FI_SUCCESS)
	{
		return;
	}

	/* ���б���ͨ��uid���ˣ����б��Ļ�ȡ����uid, ͨ���������� */
	if (dir == FI_DIR_UP)
	{
		sk = skb->sk;
		if (sk == NULL)
		{
			return;
		}

		/* tcp���б��Ļ���Ҫ��鵱ǰ����״̬ */
		if ((iph->protocol == FI_IPPROTO_TCP) &&
		    (sk->sk_state != TCP_ESTABLISHED))
		{
			return;
		}

		kuid = sock_i_uid(sk);

		/* ֻ������Ϸ����, ͨ��uid���� */
		appid = fi_find_appid(kuid.val);
		if (!FI_APPID_VALID(appid))
		{
			return;
		}

		addflow = FI_TRUE;
	}

	/* ���Ľ���, ֻ��ע�˿ڴ���1023�� */
	if (fi_pkt_parse(&pktinfo, skb, dir) != FI_SUCCESS)
	{
		return;
	}

	hash = fi_flow_hash(pktinfo.sip, pktinfo.dip, pktinfo.sport, pktinfo.dport);
	head = fi_flow_header(hash);

	fi_flow_lock();

	/* ���б��Ĳ��һ򴴽���, ���б��Ľ��������� */
	flow = fi_flow_get(&pktinfo, head, addflow);
	if (flow == NULL)
	{
		fi_flow_unlock();
		return;
	}

	if (dir == FI_DIR_UP)
	{
		flow->flowctx.appid = appid;
	}
	else
	{
		appid = flow->flowctx.appid;
	}

	/* fi����� */
	fi_rtt_entrance(&pktinfo, &(flow->flowctx), appid);
	fi_flow_unlock();

	return;
}


unsigned int fi_hook_out(void *priv, struct sk_buff *skb,
            const struct nf_hook_state *state)
{
	fi_hook(skb, FI_DIR_UP);

	return NF_ACCEPT;
}


unsigned int fi_hook_in(void *priv, struct sk_buff *skb,
            const struct nf_hook_state *state)
{
	fi_hook(skb, FI_DIR_DOWN);

	return NF_ACCEPT;
}


void fi_para_init(void)
{
	fi_flow_init();
	memset(&g_fi_ctx, 0, sizeof(g_fi_ctx));

	mutex_init(&g_fi_ctx.appinfo_lock);
	mutex_init(&g_fi_ctx.nf_mutex);

	init_timer(&g_fi_ctx.tm);
	g_fi_ctx.tm.function = fi_timer_callback;
	g_fi_ctx.tm.data     = (unsigned long)"fi_timer";
}


void fi_init(void)
{
	/* ��ʼ��ģ�����в��� */
	fi_para_init();

	FI_LOGI(" : FI init kernel module successfully, mem used %lu",
	        sizeof(fi_ctx) + sizeof(fi_flow));

	return;
}


void fi_deinit(void)
{
	/* ɾ��nfע��Ĺ��Ӻ���, ɾ����ʱ�� */
	fi_unregister_nf_hook();

	/* ������� */
	fi_flow_clear();

	FI_LOGI(" : FI deinit kernel module successfully");

	return;
}

EXPORT_SYMBOL(fi_event_process);
EXPORT_SYMBOL(fi_init);
EXPORT_SYMBOL(fi_deinit);

