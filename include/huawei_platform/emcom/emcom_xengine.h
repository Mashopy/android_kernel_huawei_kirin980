

#ifndef __EMCOM_XENGINE_H__
#define __EMCOM_XENGINE_H__
/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include <linux/if.h>
/*****************************************************************************
  2 �궨��
*****************************************************************************/


#define UID_APP                 (10000)
#define UID_INVALID_APP         (0)

#define EMCOM_MAX_MPIP_APP      (5)

#define EMCOM_XENGINE_IsUidValid(uid)	((uid) > 10000)

#define EMCOM_XENGINE_SetAccState(sk, value) \
	{ \
		(sk)->acc_state = (value); \
	}

#define EMCOM_XENGINE_SetSpeedCtrl(speedCtrlInfo, uid, size) \
	{ \
		spin_lock_bh(&(speedCtrlInfo.stLocker)); \
		(speedCtrlInfo).lUid = uid; \
		(speedCtrlInfo).ulSize = size; \
		spin_unlock_bh(&(speedCtrlInfo.stLocker)); \
	}

#define EMCOM_XENGINE_GetSpeedCtrlUid(speedCtrlInfo, uid) \
	{ \
		uid = (speedCtrlInfo).lUid; \
	}

#define EMCOM_XENGINE_GetSpeedCtrlInfo(speedCtrlInfo, uid, size) \
	{ \
		spin_lock_bh(&(speedCtrlInfo.stLocker)); \
		uid = (speedCtrlInfo).lUid; \
		size = (speedCtrlInfo).ulSize; \
		spin_unlock_bh(&(speedCtrlInfo.stLocker)); \
	}

#define HICOM_SOCK_FLAG_FINTORST  0x00000001

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

typedef enum {
	EMCOM_XENGINE_ACC_NORMAL = 0,
	EMCOM_XENGINE_ACC_HIGH,
} Emcom_Xengine_acc_state;

typedef enum {
	EMCOM_XENGINE_MPIP_TYPE_BIND_NEW = 0,
	EMCOM_XENGINE_MPIP_TYPE_BIND_RANDOM,
} Emcom_Xengine_mpip_type;
/*****************************************************************************
  4 �ṹ����
*****************************************************************************/

struct Emcom_Xengine_acc_app_info {
	uid_t     lUid; /* The uid of accelerate Application */
	uint16_t  ulAge;
	uint16_t  reverse;
};
struct Emcom_Xengine_speed_ctrl_info {
	uid_t     lUid; /* The uid of foreground Application */
	uint32_t  ulSize; /* The grade of speed control */
	spinlock_t stLocker; /* The Guard Lock of this unit */
};
struct Emcom_Xengine_speed_ctrl_data {
	uid_t     lUid; /* The uid of foreground Application */
	uint32_t  ulSize; /* The grade of speed control */
};
struct Emcom_Xengine_mpip_config{
	uid_t     lUid; /* The uid of foreground Application */
	uint32_t  ulType; /* The type of mpip speed up*/
};

/*****************************************************************************
  5 �ඨ��
*****************************************************************************/

/*****************************************************************************
  6 UNION����
*****************************************************************************/

/*****************************************************************************
  7 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  8 ��������
*****************************************************************************/
void Emcom_Xengine_Init(void);
int Emcom_Xengine_clear(void);
bool Emcom_Xengine_Hook_Ul_Stub(struct sock *pstSock);
void Emcom_Xengine_SpeedCtrl_WinSize(struct sock *pstSock, uint32_t* win);
void Emcom_Xengine_UdpEnqueue(struct sk_buff *skb);
void Emcom_Xengine_FastSyn(struct sock *pstSock);


void Emcom_Xengine_EvtProc(int32_t event, uint8_t *pdata, uint16_t len);

void Emcom_Xengine_Mpip_Bind2Device(struct sock *pstSock);
int Emcom_Xengine_SetProxyUid(struct sock *sk, char __user *optval, int optlen);
int Emcom_Xengine_GetProxyUid(struct sock *sk, char __user *optval, int __user *optlen, int len);
int Emcom_Xengine_SetSockFlag(struct sock *sk, char __user *optval, int optlen);
void Emcom_Xengine_NotifySockError(struct sock *sk);

#ifdef CONFIG_MPTCP
void Emcom_Xengine_MptcpSocketClosed(void *data, int len);
void Emcom_Xengine_MptcpSocketSwitch(void *data, int len);
void Emcom_Xengine_MptcpProxyFallback(void *data, int len);
void Emcom_Xengine_MptcpFallback(void *data, int len);
#endif

/*****************************************************************************
  9 OTHERS����
*****************************************************************************/
#endif
