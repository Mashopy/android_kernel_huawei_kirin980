#ifndef __MDRV_EASYRF_H__
#define __MDRV_EASYRF_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* easyrf dumpͷ�ṹ */
typedef struct
{
    unsigned char                       aucDescription[4];                      /* dump�ļ���Ϣ��������дRFIC��Ӧ��ASCII�� */
    unsigned int                        uwDumpSize;                             /* dump�ļ���С������ͷ����λbyte */
}EASYRF_DUMP_HEAD_STRU;

#ifdef __cplusplus
}
#endif

#endif
