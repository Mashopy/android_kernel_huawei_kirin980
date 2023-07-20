


#ifndef _NAS_COMM_PACKET_SER_H
#define _NAS_COMM_PACKET_SER_H




/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
#pragma pack(*)    �����ֽڶ��뷽ʽ
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*******************************************************************************
 2. Macro definitions
*******************************************************************************/
#define PS_IPV4_ADDR_LEN                                    (4)
#define PS_IPV6_ADDR_LEN                                    (16)
#define PS_MAX_APN_LEN                                      (100)
#define PS_MAX_PCSCF_ADDR_NUM                               (3)

#define PS_MAX_ALLOWED_S_NSSAI_NUM                          (8)
#define PS_MAX_REJECT_S_NSSAI_NUM                           (8)
#define PS_MAX_CFG_S_NSSAI_NUM                              (16)
#define PS_MAC_ADDR_LEN                                     (6)

#define PS_MAX_LADN_LIST_NUM                                (8)
#define PS_MAX_NR_TA_NUM                                    (16)     /* TA �б������� */

#define PS_URSP_OS_ID_MAX_LEN                               (4)
#define PS_URSP_APP_ID_MAX_LEN                              (8)
#define PS_URSP_TRAFFIC_MAX_APN_NUM                         (2)
#define PS_URSP_TRAFFIC_MAX_TOS_NUM                         (2)
#define PS_URSP_TRAFFIC_MAX_APP_NUM                         (2)
#define PS_URSP_TRAFFIC_MAX_SPI_NUM                         (2)
#define PS_URSP_TRAFFIC_MAX_PROTOCL_ID_NUM                  (4)
#define PS_URSP_TRAFFIC_MAX_FLOW_LABEL_NUM                  (2)
#define PS_URSP_TRAFFIC_MAX_PORT_RANGE_NUM                  (2)
#define PS_URSP_TRAFFIC_MAX_SINGLE_PORT_NUM                 (2)
#define PS_URSP_TRAFFIC_MAX_IPV4_ADDR_NUM                   (2)
#define PS_URSP_TRAFFIC_MAX_IPV6_ADDR_NUM                   (2)
#define PS_URSP_ROUTE_MAX_APN_NUM                           (2)

#define PS_POLICY_SECTION_CONTENT_MAX_NUM                   (2)
#define PS_POLICY_INSTRUCTION_MAX_NUM                       (2)
#define PS_POLICY_PART_MAX_NUM                              (2)
#define PS_POLICY_URSP_PART_MAX_NUM                         (2)
#define PS_URSP_RULE_IN_PLMN_MAX_NUM                        (PS_POLICY_INSTRUCTION_MAX_NUM * PS_POLICY_PART_MAX_NUM * PS_POLICY_URSP_PART_MAX_NUM)

#define PS_URSP_ROUTE_RULE_MAX_NUM                          (2)

#define PS_PDU_SESSION_ID_MIN                               (5)
#define PS_PDU_SESSION_ID_MAX                               (15)

/* Ŀǰ��Ч��PDU SESSION ID��Χ��1-15, �ܹ�16��, ��APSĿǰ֧�ֵ���СPDU SESSION ID����Ϊ5��, ����ͳһ�˴����Ըĳ� (PS_PDU_SESSION_ID_MAX - PS_PDU_SESSION_ID_MIN + 1) */
#define PS_PDU_SESSION_ID_MAX_NUM                           (16)

#define PS_MAX_QOS_FLOW_NUM                                 (8)
#define PS_MAX_QOS_RULE_NUM                                 (8)             /* TODO:�ݶ�ÿ��PDU Session�������8��QoS rule */
#define PS_MAX_QOS_RULE_NUM_IN_QOS_FLOW                     (4)             /* TODO:�ݶ�ÿ��QoS flow �������4��QoS rule */
#define PS_MAX_PF_NUM_IN_QOS_RULE                           (4)             /* TODO:�ݶ�ÿ��QoS rule �������4��PF,�¸������ٵ���8��  */
#define PS_MAX_PF_NUM_IN_TFT                                (16)
#define PS_MAX_MAP_QOS_FLOW_NUM                             (2)
#define PS_MAX_MAP_QOS_RULE_NUM                             (4)
#define PS_MAX_EPS_BEARER_NUM_IN_PDU                        (8)

#define PS_ONE_KBPS_UNIT_WITH_KBPS                          (1)
#define PS_ONE_MBPS_UNIT_WITH_KBPS                          (1000UL)
#define PS_ONE_GBPS_UNIT_WITH_KBPS                          (1000000UL)
#define PS_ONE_TBPS_UNIT_WITH_KBPS                          (1000000000UL)
#define PS_UE_SUPPORT_MAX_RATE                              (4000000000UL)      /* �ݶ�UE���֧������4Tbps */
#define PS_SIM_FORMAT_PLMN_LEN                              (3)               /* Sim����ʽ��Plmn���� */

#define PS_QFI_UNASSIGNED_VALUE                             (0)
/*******************************************************************************
 3. Enumerations declarations
*******************************************************************************/


enum PS_SSC_MODE_ENUM
{
    PS_SSC_MODE_1                   = 0x01,
    PS_SSC_MODE_2                   = 0x02,
    PS_SSC_MODE_3                   = 0x03,
    PS_SSC_MODE_BUTT
};
typedef VOS_UINT8 PS_SSC_MODE_ENUM_UINT8;


enum PS_IP_ADDR_TYPE_ENUM
{
    PS_IP_ADDR_TYPE_IPV4                = 0x01,
    PS_IP_ADDR_TYPE_IPV6                = 0x02,
    PS_IP_ADDR_TYPE_IPV4V6              = 0x03,
    PS_IP_ADDR_TYPE_BUTT
};
typedef VOS_UINT8 PS_IP_ADDR_TYPE_ENUM_UINT8;


enum PS_PDU_SESSION_TYPE_ENUM
{
    PS_PDU_SESSION_TYPE_IPV4            = 0x01,
    PS_PDU_SESSION_TYPE_IPV6            = 0x02,
    PS_PDU_SESSION_TYPE_IPV4V6          = 0x03,
    PS_PDU_SESSION_TYPE_UNSTRUCT        = 0x04,             /* ��ö��ֵ������5G�� */
    PS_PDU_SESSION_TYPE_ETHERNET        = 0x05,             /* ��ö��ֵ������5G�� */
    PS_PDU_SESSION_TYPE_BUTT
};
typedef VOS_UINT8 PS_PDU_SESSION_TYPE_ENUM_UINT8;


enum PS_INTEGRITY_PROTECTION_MAX_DATA_RATE_ENUM
{
    PS_INTEGRITY_PROTECTION_MAX_DATA_RATE_64K               = 0x00,
    PS_INTEGRITY_PROTECTION_MAX_DATA_RATE_FULL              = 0xFF,
};
typedef VOS_UINT8 PS_INTEGRITY_PROTECTION_MAX_DATA_RATE_ENUM_UINT8;



enum PS_PREF_ACCESS_TYPE_ENUM
{
    PS_PREF_ACCESS_TYPE_3GPP            = 1,
    PS_PREF_ACCESS_TYPE_NON_3GPP        = 2,

    PS_PREF_ACCESS_TYPE_BUTT
};
typedef VOS_UINT8 PS_PREF_ACCESS_TYPE_ENUM_UINT8;


enum PS_PDU_EMC_IND_ENUM
{
    PS_PDU_NOT_FOR_EMC            = 0x00,
    PS_PDU_FOR_EMC                = 0x01,

    PS_PDU_EMC_IND_BUTT           = 0xFF
};
typedef VOS_UINT8 PS_PDU_EMC_IND_ENUM_UINT8;


enum PS_PCSCF_DISCOVERY_ENUM
{
    PS_PCSCF_DISCOVERY_NOT_INFLUENCED     = 0x00,
    PS_PCSCF_DISCOVERY_THROUGH_NAS_SIG    = 0x01,
    PS_PCSCF_DISCOVERY_THROUGH_DHCP       = 0x02,

    PS_PCSCF_DISCOVERY_BUTT               = 0xFF
};
typedef VOS_UINT8 PS_PCSCF_DISCOVERY_ENUM_UINT8;


enum PS_IMS_CN_SIG_FLAG_ENUM
{
    PS_NOT_FOR_IMS_CN_SIG_ONLY    = 0x00,
    PS_FOR_IM_CN_SIG_ONLY         = 0x01,

    PS_IM_CN_SIG_FLAG_BUTT        = 0xFF
};
typedef VOS_UINT8 PS_IMS_CN_SIG_FLAG_ENUM_UINT8;


enum PS_QOS_RULE_TYPE_ENUM
{
    PS_QOS_RULE_TYPE_DEDICATED    = 0x00,
    PS_QOS_RULE_TYPE_DEFAULT      = 0x01,
    PS_QOS_RULE_TYPE_BUTT
};
typedef VOS_UINT8 PS_QOS_RULE_TYPE_ENUM_UINT8;


enum PS_DEB_TYPE_ENUM
{
    PS_DEB_TYPE_DEDICATED    = 0x00,
    PS_DEB_TYPE_DEFAULT      = 0x01,
    PS_DEB_TYPE_BUTT
};
typedef VOS_UINT8 PS_DEB_TYPE_ENUM_UINT8;


enum PS_PF_TRANS_DIRECTION_ENUM
{
    PS_PF_TRANS_DIRECTION_RESERVED     = 0x00,             /* 0 - Reserved */
    PS_PF_TRANS_DIRECTION_DOWNLINK     = 0x01,             /* 1 - Downlink */
    PS_PF_TRANS_DIRECTION_UPLINK       = 0x02,             /* 2 - Uplink */
    PS_PF_TRANS_DIRECTION_BIDIRECTION  = 0x03,             /* 3 - Birectional (Up & Downlink) (default if omitted) */

    PS_PF_TRANS_DIRECTION_BUTT         = 0xFF
};
typedef VOS_UINT8 PS_PF_TRANS_DIRECTION_ENUM_UINT8;



enum PS_URSP_MATCH_ALL_ENUM
{
    PS_URSP_NOT_MATCH_ALL               = 0x00,
    PS_URSP_MATCH_ALL                   = 0x01,

    PS_URSP_MATCH_ALL_BUTT
};
typedef VOS_UINT8 PS_URSP_MATCH_ALL_ENUM_UINT8;



enum PS_PF_MATCH_ALL_ENUM
{
    PS_PF_NOT_MATCH_ALL                 = 0x00,
    PS_PF_MATCH_ALL                     = 0x01,

    PS_PF_MATCH_ALL_BUTT
};
typedef VOS_UINT8 PS_PF_MATCH_ALL_ENUM_UINT8;


enum PS_REJ_S_NSSAI_CAUSE_ENUM
{
    PS_REJ_S_NSSAI_CAUSE_NOT_AVAIL_IN_PLMN      = 0x00,
    PS_REJ_S_NSSAI_CAUSE_NOT_AVAIL_IN_REG_AREA  = 0x01,
    PS_REJ_S_NSSAI_CAUSE_BUTT                   = 0x02
};
typedef VOS_UINT8 PS_REJ_S_NSSAI_CAUSE_ENUM_UINT8;


enum PS_NON_SEAMLESS_NON_3GPP_OFFLOAD_IND_ENUM
{
    PS_NO_OFFLOAD_INDICATION                   = 0x00,
    PS_OFFLOAD_INDICATION                      = 0x01,

    PS_OFFLOAD_INDICATION_BUTT
};
typedef VOS_UINT8 PS_NON_SEAMLESS_NON_3GPP_OFFLOAD_IND_ENUM_UINT8;


enum PS_REFLECT_QOS_IND_ENUM
{
    PS_REFLECT_QOS_IND_NOT_SUPPORT      = 0,
    PS_REFLECT_QOS_IND_SUPPORT          = 1,

    PS_REFLECT_QOS_IND_BUTT
};
typedef VOS_UINT8 PS_REFLECT_QOS_IND_ENUM_UINT8;


enum PS_IPV6_MULTI_HOMED_IND_ENUM
{
    PS_IPV6_MULTI_HOMED_IND_NOT_SUPPORT = 0,
    PS_IPV6_MULTI_HOMED_IND_SUPPORT     = 1,

    PS_IPV6_MULTI_HOMED_IND_BUTT
};
typedef VOS_UINT8 PS_IPV6_MULTI_HOMED_IND_ENUM_UINT8;


enum PS_PDU_SESSION_ALWAYS_ON_IND_ENUM
{
    PS_PDU_SESSION_ALWAYS_ON_IND_NOT    = 0,
    PS_PDU_SESSION_ALWAYS_ON_IND        = 1,

    PS_PDU_SESSION_ALWAYS_ON_IND_BUTT
};
typedef VOS_UINT8 PS_PDU_SESSION_ALWAYS_ON_IND_ENUM_UINT8;


enum PS_SESSION_AMBR_UNIT_TYPE_ENUM
{
    PS_SESSION_AMBR_UNIT_TYPE_NOT_USED                                    = 0x00,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_1KBPS       = 0X01,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_4KBPS       = 0X02,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_16KBPS      = 0X03,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_64KBPS      = 0X04,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_256KBPS     = 0X05,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_1MBPS       = 0X06,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_4MBPS       = 0X07,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_16MBPS      = 0X08,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_64MBPS      = 0X09,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_256MBPS     = 0X0A,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_1GBPS       = 0X0B,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_4GBPS       = 0X0C,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_16GBPS      = 0X0D,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_64GBPS      = 0X0E,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_256GBPS     = 0X0F,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_1TBPS       = 0X10,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_4TBPS       = 0X11,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_16TBPS      = 0X12,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_64TBPS      = 0X13,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_256TBPS     = 0X14,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_1PBPS       = 0X15,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_4PBPS       = 0X16,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_16PBPS      = 0X17,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_64PBPS      = 0X18,
    PS_SESSION_AMBR_UNIT_TYPE_VALUE_INCREMENT_IN_MULTIPLES_OF_256PBPS     = 0X19,

    PS_SESSION_AMBR_UNIT_TYPE_ENUM_BUTT
};

typedef VOS_UINT8       PS_SESSION_AMBR_UNIT_TYPE_ENUM_UINT8;


enum PS_NR_QOS_RULE_OPERATION_CODE_ENUM
{
    PS_NR_QOS_RULE_OPERATION_CODE_RESERVED                  = 0x00, /* Reserved */
    PS_NR_QOS_RULE_OPERATION_CODE_CREATE_QOS_RULE           = 0x01, /* Create new Qos rule */
    PS_NR_QOS_RULE_OPERATION_CODE_DELETE_QOS_RULE           = 0x02, /* Delete exiting Qos rule */
    PS_NR_QOS_RULE_OPERATION_CODE_MODIFY_ADD_PF             = 0x03, /* Modify exiting Qos rule and add packet filters */
    PS_NR_QOS_RULE_OPERATION_CODE_MODIFY_REPLACE_PF         = 0x04, /* Modify exiting Qos rule and replace packet filters */
    PS_NR_QOS_RULE_OPERATION_CODE_MODIFY_DELETE_PF          = 0x05, /* Modify exiting Qos rule and delete packet filters */
    PS_NR_QOS_RULE_OPERATION_CODE_MODIFY_WITHOUT_PF         = 0x06, /* Modify exiting Qos rule without modifying packet filters */
    PS_NR_QOS_RULE_OPERATION_CODE_BUTT
};

typedef VOS_UINT8   PS_NR_QOS_RULE_OPERATION_CODE_ENUM_UINT8;


enum PS_NR_QOS_FLOW_OPERATION_CODE_ENUM
{
    PS_NR_QOS_FLOW_OPERATION_CODE_RESERVED                  = 0x00, /* Reserved */
    PS_NR_QOS_FLOW_OPERATION_CODE_CREATE_QOS_FLOW           = 0x01, /* Create new Qos Flow */
    PS_NR_QOS_FLOW_OPERATION_CODE_DELETE_QOS_FLOW           = 0x02, /* Delete exiting Qos Flow */
    PS_NR_QOS_FLOW_OPERATION_CODE_MODIFY_QOS_FLOW           = 0x03, /* Modify exiting Qos Flow */
    PS_NR_QOS_FLOW_OPERATION_CODE_BUTT
};

typedef VOS_UINT8   PS_NR_QOS_FLOW_OPERATION_CODE_ENUM_UINT8;

typedef PS_SESSION_AMBR_UNIT_TYPE_ENUM_UINT8                PS_NR_BIT_RATE_UNIT_TYPE_ENUM_UINT8;
/*******************************************************************************
 4. Message Header declaration
*******************************************************************************/

/*******************************************************************************
 5. Message declaration
*******************************************************************************/

/*******************************************************************************
 6. STRUCT and UNION declaration
*******************************************************************************/

typedef struct
{
    VOS_UINT8                           ucLength;                               /* ����Ϊ0��ʾAPN/DNN��Ч */
    VOS_UINT8                           aucReserved[3];
    VOS_UINT8                           aucValue[PS_MAX_APN_LEN];
} PS_APN_STRU;


typedef struct
{
    VOS_UINT32                          ulDLSessionAmbr;/* ת����ĵ�λ��:kbps */
    VOS_UINT32                          ulULSessionAmbr;/* ת����ĵ�λ��:kbps */
} PS_PDU_SESSION_AMBR_STRU;


typedef struct
{
    VOS_UINT32                          bitOpSd             : 1;
    VOS_UINT32                          bitOpMappedSst      : 1;
    VOS_UINT32                          bitOpMappedSd       : 1;
    VOS_UINT32                          bitOpSpare          : 29;

    VOS_UINT8                           ucSst;
    VOS_UINT8                           ucMappedSst;
    VOS_UINT8                           aucReserved[2];
    VOS_UINT32                          ulSd;
    VOS_UINT32                          ulMappedSd;
} PS_S_NSSAI_STRU;


typedef struct
{
    VOS_UINT8                           ucSNssaiNum;
    VOS_UINT8                           aucReserved[3];
    PS_S_NSSAI_STRU                     astSNssai[PS_MAX_ALLOWED_S_NSSAI_NUM];
}PS_ALLOW_NSSAI_STRU;


typedef struct
{
    VOS_UINT8                           ucSNssaiNum;
    VOS_UINT8                           aucReserved[3];
    PS_S_NSSAI_STRU                     astSNssai[PS_MAX_CFG_S_NSSAI_NUM];
}PS_CONFIGURED_NSSAI_STRU;


typedef struct
{
    VOS_UINT32                                              bitOpSd        : 1;
    VOS_UINT32                                              bitOpSpare     : 31;

    PS_REJ_S_NSSAI_CAUSE_ENUM_UINT8                         enCause;
    VOS_UINT8                                               ucSst;
    VOS_UINT8                                               aucReserved[2];
    VOS_UINT32                                              ulSd;
}PS_REJECTED_S_NSSAI_STRU;


typedef struct
{
    VOS_UINT8                                               ucSNssaiNum;
    VOS_UINT8                                               aucReserved[3];
    PS_REJECTED_S_NSSAI_STRU                                astSNssai[PS_MAX_REJECT_S_NSSAI_NUM];
}PS_REJECTED_NSSAI_INFO_STRU;

/*****************************************************************************
 �ṹ����   : PS_PF_STRU
 Э����   :
 ASN.1 ���� :
 �ṹ˵��   : ���TS 24.501 section 9.10.4.9 Traffic Flow Template��������
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpRmtIpv4AddrAndMask : 1;
    VOS_UINT32                          bitOpRmtIpv6AddrAndMask : 1;
    VOS_UINT32                          bitOpProtocolId         : 1;
    VOS_UINT32                          bitOpSingleLocalPort    : 1;
    VOS_UINT32                          bitOpLocalPortRange     : 1;
    VOS_UINT32                          bitOpSingleRemotePort   : 1;
    VOS_UINT32                          bitOpRemotePortRange    : 1;
    VOS_UINT32                          bitOpSecuParaIndex      : 1;
    VOS_UINT32                          bitOpTypeOfService      : 1;
    VOS_UINT32                          bitOpFlowLabelType      : 1;
    VOS_UINT32                          bitOpLocalIpv4AddrAndMask : 1;
    VOS_UINT32                          bitOpLocalIpv6AddrAndMask : 1;
    VOS_UINT32                          bitOpSpare              : 20;

    VOS_UINT8                           ucPacketFilterId;
    VOS_UINT8                           ucNwPacketFilterId;
    PS_PF_TRANS_DIRECTION_ENUM_UINT8    enDirection;
    VOS_UINT8                           ucPrecedence;             /*packet filter evaluation precedence*/

    VOS_UINT32                          ulSecuParaIndex;          /*SPI*/
    VOS_UINT16                          usSingleLcPort;
    VOS_UINT16                          usLcPortHighLimit;
    VOS_UINT16                          usLcPortLowLimit;
    VOS_UINT16                          usSingleRmtPort;
    VOS_UINT16                          usRmtPortHighLimit;
    VOS_UINT16                          usRmtPortLowLimit;
    VOS_UINT8                           ucProtocolId;             /*Э���*/
    VOS_UINT8                           ucTypeOfService;          /*TOS*/

    VOS_UINT8                           ucTypeOfServiceMask;      /*TOS Mask*/
    PS_PF_MATCH_ALL_ENUM_UINT8          enMatchAll;

    VOS_UINT8                           aucRmtIpv4Address[PS_IPV4_ADDR_LEN];
                                                                  /*ucSourceIpAddress[0]ΪIP��ַ���ֽ�λ
                                                                    ucSourceIpAddress[3]Ϊ���ֽ�λ*/
    VOS_UINT8                           aucRmtIpv4Mask[PS_IPV4_ADDR_LEN];
                                                                  /*ucSourceIpMask[0]ΪIP��ַ���ֽ�λ ,
                                                                    ucSourceIpMask[3]Ϊ���ֽ�λ */
    VOS_UINT8                           aucRmtIpv6Address[PS_IPV6_ADDR_LEN];
                                                                  /*ucRmtIpv6Address[0]ΪIPv6�ӿڱ�ʶ���ֽ�λ
                                                                    ucRmtIpv6Address[15]ΪIPv6�ӿڱ�ʶ���ֽ�λ*/
    VOS_UINT8                           aucRmtIpv6Mask[PS_IPV6_ADDR_LEN];
                                                                  /*ucRmtIpv6Mask[0]Ϊ���ֽ�λ
                                                                    ucRmtIpv6Mask[15]Ϊ���ֽ�λ*/

    VOS_UINT32                          ulFlowLabelType;          /*FlowLabelType*/
    VOS_UINT8                           aucLocalIpv4Addr[PS_IPV4_ADDR_LEN];
    VOS_UINT8                           aucLocalIpv4Mask[PS_IPV4_ADDR_LEN];
    VOS_UINT8                           aucLocalIpv6Addr[PS_IPV6_ADDR_LEN];
    VOS_UINT8                           ucLocalIpv6Prefix;
    VOS_UINT8                           aucReserved2[3];

} PS_PF_STRU;


typedef struct
{
    VOS_UINT32                          ulPfNum;                                /* pf���� */
    PS_PF_STRU                          astPf[PS_MAX_PF_NUM_IN_TFT];                   /* pf�� */
}PS_TFT_STRU;


typedef struct
{
    VOS_UINT32                          ulPfNum;                                /* pf���� */
    PS_PF_STRU                          astPf[PS_MAX_PF_NUM_IN_QOS_RULE];       /* pf�� */
}PS_QFT_STRU;


typedef struct
{
    /* 0 QCI is selected by network
       [1 - 4]value range for guranteed bit rate Traffic Flows
       [5 - 9]value range for non-guarenteed bit rate Traffic Flows */
    VOS_UINT8                           ucQCI;

    VOS_UINT8                           aucReserved[3];

    /* DL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulDLGBR;

    /* UL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulULGBR;

    /* DL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulDLMBR;

    /* UL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32                          ulULMBR;
}PS_EPS_QOS_STRU;


typedef struct
{
    VOS_UINT32                          ulULMBR;    /* �����������ֵ,��λKBPS */
    VOS_UINT32                          ulDLMBR;    /* �����������ֵ,��λKBPS */
    VOS_UINT32                          ulULGBR;    /* ���б�֤����ֵ,��λKBPS */
    VOS_UINT32                          ulDLGBR;    /* ���б�֤����ֵ,��λKBPS */
}PS_EPS_EXTENDED_QOS_STRU;


typedef struct
{
    VOS_UINT32                          ulDLApnAmbr;/* ��������ֵ,��λKBPS */
    VOS_UINT32                          ulULApnAmbr;/* ��������ֵ,��λKBPS */
} PS_EPS_APN_AMBR_STRU;


typedef struct
{
    VOS_UINT32                          ulExtDLApnAmbr;/* ��չ��������ֵ,��λKBPS */
    VOS_UINT32                          ulExtULApnAmbr;/* ��չ��������ֵ,��λKBPS */
} PS_EPS_EXTENDED_APN_AMBR_STRU;


typedef struct
{
    /* IPV4:1, IPV6:2, IPV4V6:3 */
    PS_IP_ADDR_TYPE_ENUM_UINT8          enIpType;
    VOS_UINT8                           aucReserved[3];
    VOS_UINT8                           aucIpv4Addr[PS_IPV4_ADDR_LEN];
    VOS_UINT8                           aucIpv6Addr[PS_IPV6_ADDR_LEN];
} PS_IP_ADDR_STRU;


typedef struct
{
    VOS_UINT8                           aucPcscfAddr[PS_IPV4_ADDR_LEN];
} PS_IPV4_PCSCF_STRU;


typedef struct
{
    VOS_UINT8                           ucIpv4PcscfAddrNum;                     /* IPV4��P-CSCF��ַ��������Ч��Χ[0,3] */
    VOS_UINT8                           aucRsv[3];                              /* ���� */

    PS_IPV4_PCSCF_STRU                  astIpv4PcscfAddrList[PS_MAX_PCSCF_ADDR_NUM];
} PS_IPV4_PCSCF_LIST_STRU;


typedef struct
{
    VOS_UINT8                           aucPcscfAddr[PS_IPV6_ADDR_LEN];
} PS_IPV6_PCSCF_STRU;


typedef struct
{
    VOS_UINT8                           ucIpv6PcscfAddrNum;                     /* IPV6��P-CSCF��ַ��������Ч��Χ[0,3] */
    VOS_UINT8                           aucRsv[3];                              /* ���� */

    PS_IPV6_PCSCF_STRU                  astIpv6PcscfAddrList[PS_MAX_PCSCF_ADDR_NUM];
} PS_IPV6_PCSCF_LIST_STRU;


typedef struct
{
    PS_NR_BIT_RATE_UNIT_TYPE_ENUM_UINT8 ucUnit;             /* ��λ */
    VOS_UINT8                           ucRsv;
    VOS_UINT16                          usValue;            /* ��ֵ */
}PS_NR_BIT_RATE_STRU;


typedef struct
{
    VOS_UINT32                          bitOpAveragWindow   : 1;
    VOS_UINT32                          bitOpSpare          : 31;

    VOS_UINT8                           uc5QI;
    VOS_UINT8                           ucRsv;
    VOS_UINT16                          usAveragWindow;     /* Averaging Window */
    VOS_UINT32                          ulULMaxRate;        /* MFBR uplink,��Чֵ:0 */
    VOS_UINT32                          ulDLMaxRate;        /* MFBR downlink,��Чֵ:0  */
    VOS_UINT32                          ulULGMaxRate;       /* GFBR uplink,��Чֵ:0  */
    VOS_UINT32                          ulDLGMaxRate;       /* GFBR downlink,��Чֵ:0  */
}PS_NR_QOS_STRU;


typedef struct
{
    VOS_UINT8                           ucQri;
    VOS_UINT8                           ucQfi;
    VOS_UINT8                           ucQosRulePrecedence;
    PS_QOS_RULE_TYPE_ENUM_UINT8         enQosRuleType;                  /* DQR:0 DEDICATED;1 DEFAULT */
    PS_QFT_STRU                         stQft;
} PS_NR_QOS_RULE_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucQosRuleNum;
    VOS_UINT8                           aucResv[3];
    PS_NR_QOS_RULE_INFO_STRU            astQosRule[PS_MAX_QOS_RULE_NUM];
} PS_NR_QOS_RULE_LIST_STRU;


typedef struct
{
    VOS_UINT32                          bitOpEpsbId         : 1;
    VOS_UINT32                          bitOpSpare          : 31;

    VOS_UINT8                           ucQfi;
    VOS_UINT8                           ucEpsbId;                       /* ���û��Epsbid,��ʾ�� QoS Flowû��ӳ���EPS BEARER*/
    VOS_UINT8                           ucPsCallId;                     /* 0xFFΪ��Чֵ */
    VOS_UINT8                           ucCid;
    VOS_UINT8                           ucQosRuleNum;
    VOS_UINT8                           aucResv[3];

    VOS_UINT8                           aucQri[PS_MAX_QOS_RULE_NUM_IN_QOS_FLOW];    /* ָ��PS_QOS_RULE_LIST_STRU�е�QoS Rule Identifier����ucQriֵ*/

    PS_NR_QOS_STRU                      stNrQos;

} PS_NR_QOS_FLOW_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucQosFlowNum;
    VOS_UINT8                           aucResv[3];
    PS_NR_QOS_FLOW_INFO_STRU            astQosFlow[PS_MAX_QOS_FLOW_NUM];
} PS_NR_QOS_FLOW_LIST_STRU;


typedef struct
{
    VOS_UINT8                           ucMapQosFlowNum;
    VOS_UINT8                           aucResv[3];
    PS_NR_QOS_FLOW_INFO_STRU            astMapQosFlow[PS_MAX_MAP_QOS_FLOW_NUM];
}PS_EPS_MAP_QOS_FLOW_LIST_STRU;


typedef struct
{
    VOS_UINT8                           ucMapQosRuleNum;
    VOS_UINT8                           aucResv[3];
    PS_NR_QOS_RULE_INFO_STRU            astMapQosRule[PS_MAX_MAP_QOS_RULE_NUM];
}PS_EPS_MAP_QOS_RULE_LIST_STRU;


typedef struct
{
    VOS_UINT8                           bitOpQos            : 1;
    VOS_UINT8                           bitOpApnAmbr        : 1;
    VOS_UINT8                           bitOpTft            : 1;
    VOS_UINT32                          bitSpare            : 29;

    VOS_UINT8                           ucEpsbId;
    PS_DEB_TYPE_ENUM_UINT8              enBearerType;                  /* DEB:0 DEDICATED;1 DEFAULT */
    VOS_UINT8                           aucResv[2];

    PS_EPS_QOS_STRU                     stEpsQos;
    PS_EPS_APN_AMBR_STRU                stApnAmbr;
    PS_TFT_STRU                         stTft;
} PS_MAP_EPS_BEARER_INFO_STRU;


typedef struct
{
    VOS_UINT8                           ucEpsBearerNum;
    VOS_UINT8                           aucResv[3];

    PS_MAP_EPS_BEARER_INFO_STRU         astEpsBearer[PS_MAX_EPS_BEARER_NUM_IN_PDU];
} PS_MAP_EPS_BEARER_LIST_STRU;


typedef struct
{
    VOS_UINT8                           aucPlmnId[PS_SIM_FORMAT_PLMN_LEN];
    VOS_UINT8                           ucRsv;
}PS_SIM_FORMAT_PLMN_ID_STRU;



typedef struct
{
    VOS_UINT8                           aucPlmnId[3];
    VOS_UINT8                           ucRsv;
}PS_NR_PLMN_ID_STRU;


typedef struct
{
    VOS_UINT8                           ucTac;
    VOS_UINT8                           ucTacCont;
    VOS_UINT8                           ucTacCont1;
    VOS_UINT8                           aucRsv[1];
}PS_NR_TAC_STRU;


typedef struct
{
    PS_NR_PLMN_ID_STRU                  stPlmnId;
    PS_NR_TAC_STRU                      stTac;
}PS_NR_TAI_STRU;


typedef struct
{
    VOS_UINT32                          ulTaNum;            /* TA�ĸ���    */
    PS_NR_TAI_STRU                      astTai[PS_MAX_NR_TA_NUM];
}PS_NR_TAI_LIST_STRU;


typedef struct
{
    PS_APN_STRU                         stDnn;
    PS_NR_TAI_LIST_STRU                 stTaiList;
}PS_LADN_STRU;


typedef struct
{
    VOS_UINT8                           ucLadnNum;
    VOS_UINT8                           aucReserved[3];
    PS_LADN_STRU                        astLadnList[PS_MAX_LADN_LIST_NUM];
}PS_LADN_INFO_STRU;


typedef struct
{
    VOS_UINT32                          bitOpPrimDnsAddr    : 1;
    VOS_UINT32                          bitOpSecDnsAddr     : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           aucPrimDnsAddr[PS_IPV4_ADDR_LEN];
    VOS_UINT8                           aucSecDnsAddr[PS_IPV4_ADDR_LEN];
}PS_IPV4_DNS_STRU;


typedef struct
{
    VOS_UINT32                          bitOpPrimDnsAddr    : 1;
    VOS_UINT32                          bitOpSecDnsAddr     : 1;
    VOS_UINT32                          bitOpSpare          : 30;

    VOS_UINT8                           aucPrimDnsAddr[PS_IPV6_ADDR_LEN];
    VOS_UINT8                           aucSecDnsAddr[PS_IPV6_ADDR_LEN];

}PS_IPV6_DNS_STRU;


typedef struct
{
    VOS_UINT8                           aucRmtIpv6Address[PS_IPV6_ADDR_LEN];
                                                                  /*ucRmtIpv6Address[0]ΪIPv6�ӿڱ�ʶ���ֽ�λ
                                                                    ucRmtIpv6Address[7]ΪIPv6�ӿڱ�ʶ���ֽ�λ*/
   /* VOS_UINT8                           aucRmtIpv6Mask[PS_IPV6_ADDR_LEN]; */
                                                                  /*ucRmtIpv6Mask[0]Ϊ���ֽ�λ
                                                                    ucRmtIpv6Mask[7]Ϊ���ֽ�λ*/
    VOS_UINT8                           ucPrefixLen;
    VOS_UINT8                           aucReserved[3];
}PS_URSP_RMT_IPV6_ADDR_MASK_STRU;


typedef struct
{
    VOS_UINT8                           aucRmtIpv4Address[PS_IPV4_ADDR_LEN];
                                                                  /*ucSourceIpAddress[0]ΪIP��ַ���ֽ�λ
                                                                  ucSourceIpAddress[3]Ϊ���ֽ�λ*/
    VOS_UINT8                           aucRmtIpv4Mask[PS_IPV4_ADDR_LEN];
                                                                  /*ucSourceIpMask[0]ΪIP��ַ���ֽ�λ ,
                                                                  ucSourceIpMask[3]Ϊ���ֽ�λ*/
}PS_URSP_RMT_IPV4_ADDR_MASK_STRU;


typedef struct
{
    VOS_UINT8                           aucOsId[PS_URSP_OS_ID_MAX_LEN];
    VOS_UINT8                           aucAppId[PS_URSP_APP_ID_MAX_LEN];
}PS_URSP_OS_APP_ID_STRU;


typedef struct
{
    VOS_UINT16                          usRmtPortLowLimit;
    VOS_UINT16                          usRmtPortHighLimit;
}PS_URSP_RMT_PORT_RANGE_STRU;


typedef struct
{
   VOS_UINT8                            ucTypeOfService;          /*TOS*/
   VOS_UINT8                            ucTypeOfServiceMask;      /*TOS Mask*/
   VOS_UINT8                            aucResvered[2];
}PS_URSP_TOS_STRU;

/*****************************************************************************
 �ṹ����  : PS_URSP_TRAFFIC_RULE_STRU
 �ṹ˵��  : 24.526Э�� 5.2�½�Traffic descriptor component
*****************************************************************************/
typedef struct
{

    PS_URSP_MATCH_ALL_ENUM_UINT8        enMatchAll;                                                     /* Ϊ1ʱ���������е�����Ϊ��*/
    VOS_UINT8                           ucSpiNum;
    VOS_UINT8                           ucDnnNum;
    VOS_UINT8                           ucTosNum;
    VOS_UINT8                           ucAppNum;
    VOS_UINT8                           ucIpv4AddrNum;
    VOS_UINT8                           ucIpv6AddrNum;
    VOS_UINT8                           ucProtoclIdNum;
    VOS_UINT8                           ucFlowLabelNum;
    VOS_UINT8                           ucSinglePortNum;
    VOS_UINT8                           ucPortRangeNum;
    VOS_UINT8                           ucResvered;

    VOS_UINT8                           aucProtocolId[PS_URSP_TRAFFIC_MAX_PROTOCL_ID_NUM];              /*Э���*/
    VOS_UINT32                          aulSecuParaIndex[PS_URSP_TRAFFIC_MAX_SPI_NUM];                  /*SPI*/
    VOS_UINT32                          aulFlowLabelType[PS_URSP_TRAFFIC_MAX_FLOW_LABEL_NUM];           /*FlowLabelType*/
    VOS_UINT16                          ausSingleRmtPort[PS_URSP_TRAFFIC_MAX_SINGLE_PORT_NUM];
    PS_APN_STRU                         astDnn[PS_URSP_TRAFFIC_MAX_APN_NUM];
    PS_URSP_TOS_STRU                    astTos[PS_URSP_TRAFFIC_MAX_TOS_NUM];
    PS_URSP_OS_APP_ID_STRU              astOsAppId[PS_URSP_TRAFFIC_MAX_APP_NUM];

    PS_URSP_RMT_PORT_RANGE_STRU         astRmtPortRange[PS_URSP_TRAFFIC_MAX_PORT_RANGE_NUM];
    PS_URSP_RMT_IPV4_ADDR_MASK_STRU     astRmtIpv4Addr[PS_URSP_TRAFFIC_MAX_IPV4_ADDR_NUM];
    PS_URSP_RMT_IPV6_ADDR_MASK_STRU     astRmtIpv6Addr[PS_URSP_TRAFFIC_MAX_IPV6_ADDR_NUM];
}PS_URSP_TRAFFIC_RULE_STRU;


/*****************************************************************************
 �ṹ����  : PS_URSP_ROUTE_RULE_STRU
 �ṹ˵��  : 24.526Э�� 5.2�½�Route selection descriptor contents
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                              bitOpSscMode                   : 1;
    VOS_UINT32                                              bitOpPduSessionType            : 1;
    VOS_UINT32                                              bitOpPreferredAccessType       : 1;
    VOS_UINT32                                              bitOpSpare                     : 29;

    VOS_UINT8                                               ucRoutePrecedence;
    PS_SSC_MODE_ENUM_UINT8                                  enSscMode;
    PS_PDU_SESSION_TYPE_ENUM_UINT8                          enPduSessionType;
    PS_PREF_ACCESS_TYPE_ENUM_UINT8                          enPrefAccessType;

    PS_ALLOW_NSSAI_STRU                                     stNssai;
    PS_NON_SEAMLESS_NON_3GPP_OFFLOAD_IND_ENUM_UINT8         enOffloadFlag;                              /* non-seamless non-3GPP offload indication type */
    VOS_UINT8                                               aucRsv[2];
    VOS_UINT8                                               ucDnnNum;                                   /* numΪ0ʱ��ʾû��DNN */
    PS_APN_STRU                                             astDnn[PS_URSP_ROUTE_MAX_APN_NUM];          /* PS_APN_IN_ROUTE_RULE_MAX_NUM= 4 */
} PS_URSP_ROUTE_RULE_STRU;

/*****************************************************************************
 �ṹ����  : PS_URSP_ROUTE_RULE_LIST_STRU
 �ṹ˵��  : 24.526Э�� 5.2�½�Route selection descriptor contents
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRouteRuleNum;                             /* numΪ0ʱ��ʾû��RouteRule */
    VOS_UINT8                           aucRsv[3];
    PS_URSP_ROUTE_RULE_STRU             astRouteRule[PS_URSP_ROUTE_RULE_MAX_NUM];   /* PS_URSP_ROUTE_RULE_MAX_NUM= 4 */
} PS_URSP_ROUTE_RULE_LIST_STRU;

/*****************************************************************************
 �ṹ����  : TAF_PS_ROUTE_RULE_LIST_STRU
 �ṹ˵��  : 24.526Э�� 5.2�½�URSP rule
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucUrspPrecedence;
    VOS_UINT8                           aucRsv[3];
    PS_URSP_TRAFFIC_RULE_STRU           stTrafficRule;
    PS_URSP_ROUTE_RULE_LIST_STRU        stRouteList;
} PS_URSP_RULE_STRU;


/*****************************************************************************
 �ṹ����  : PS_URSP_RULE_LIST_STRU
 �ṹ˵��  : 24.526Э�� 5.2�½�URSP rules
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucUrspNum;                                      /* numΪ0ʱ��ʾû��RouteRule */
    VOS_UINT8                           aucRsv[3];
    PS_URSP_RULE_STRU                   astUrspRule[PS_URSP_RULE_IN_PLMN_MAX_NUM];      /* PS_URSP_RULE_MAX_NUM= 16 */
} PS_URSP_RULE_LIST_STRU;

/*****************************************************************************
 �ṹ����  : PS_5GSM_CAPABILITY_STRU
 �ṹ˵��  : 24.501Э�� 9.10.4.2
*****************************************************************************/
typedef struct
{
    PS_REFLECT_QOS_IND_ENUM_UINT8       enRqosInd;
    PS_IPV6_MULTI_HOMED_IND_ENUM_UINT8  enMH6Ind;
    VOS_UINT8                           aucRsv[2];
} PS_5GSM_CAPABILITY_STRU;

/*****************************************************************************
 �ṹ��    : PS_ALLOC_SSC_MODE_STRU
 �ṹ˵��  : ALLOWED SSC MODE���ݽṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSscMode1Allowed;    /* VOS_TRUE :allowed��VOS_FALSE :not allowed */
    VOS_UINT8                           ucSscMode2Allowed;    /* VOS_TRUE :allowed��VOS_FALSE :not allowed */
    VOS_UINT8                           ucSscMode3Allowed;    /* VOS_TRUE :allowed��VOS_FALSE :not allowed */
    VOS_UINT8                           aucReserved[1];
}PS_ALLOWED_SSC_MODE_STRU;

/*******************************************************************************
 8. Global  declaration
*******************************************************************************/

/*******************************************************************************
 9. Function declarations
*******************************************************************************/

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


#endif
