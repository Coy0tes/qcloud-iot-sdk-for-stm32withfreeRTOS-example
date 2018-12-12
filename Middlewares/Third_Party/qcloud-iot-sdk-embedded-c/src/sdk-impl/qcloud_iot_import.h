/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QCLOUD_IOT_IMPORT_H_
#define QCLOUD_IOT_IMPORT_H_
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>
#include "qcloud_iot_platform.h"



#define _IN_            /* ��������һ���������. */
#define _OU_            /* ��������һ���������. */

#define IOT_TRUE    (1)     /* indicate boolean value true */
#define IOT_FALSE   (0)     /* indicate boolean value false */



/**
 * @brief ����������
 *
 * @return ����ʧ�ܷ���NULL���ɹ�����Mutexָ��
 */
void *HAL_MutexCreate(void);

/**
 * @brief ���ٻ�����
 *
 * @param Mutexָ��
 */
void HAL_MutexDestroy(_IN_ void *mutex);

/**
 * @brief ����
 *
 * @param Mutexָ��
 */
void HAL_MutexLock(_IN_ void *mutex);

/**
 * @brief �ͷ���
 *
 * @param Mutexָ��
 */
void HAL_MutexUnlock(_IN_ void *mutex);

/**
 * @brief ����һ����ڴ棬����һ��ָ��鿪ʼ��ָ��.
 *
 * @param size   ���ֽ�ָ�����С.
 * @return       һ��ָ��block��ͷ��ָ��.
 */
void *HAL_Malloc(_IN_ uint32_t size);

/**
 * @brief �ͷ��ڴ��
 *
 * @param ptr   ָ����ǰ�����ƽ̨malloc���ڴ���ָ��.
 */
void HAL_Free(_IN_ void *ptr);

/**
 * @brief ����ʽ��������д���׼�������.
 *
 * @param fmt   Ҫд����ı����ַ���, ������ѡ�����Ƕ��ĸ�ʽָ����, ��ָ�������Ĳ������ת��Ϊ���.
 * @param ...   ���������б�.
 */
void HAL_Printf(_IN_ const char *fmt, ...);

/**
 * @brief ����ʽ��������д���ַ���.
 *
 * @param str   Ŀ���ַ���.
 * @param len   ����д���ַ�����󳤶�
 * @param fmt   Ҫ��д���ı��ĸ�ʽ��������ѡ�����Ƕ��ĸ�ʽָ����, ��ָ�������Ĳ������ת��Ϊ���.
 * @param ...   ���������б����ڸ�ʽ�������뵽���ɵ��ַ����У��滻���Ǹ��Ե�ָ����.
 * @return      �ɹ�д���ַ������ֽ���.
 */
//int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...);

/**
 * @brief ����ʽ��������д���ַ���.
 *
 * @param [out] str: Ŀ���ַ���.
 * @param [in] len: ����д���ַ�����󳤶�.
 * @param [in] fmt: Ҫ��д���ı��ĸ�ʽ.
 * @param [in] ap:  �����б�.
 * @return �ɹ�д���ַ������ֽ���.
 */
int HAL_Vsnprintf(_OU_ char *str, _IN_ const int len, _IN_ const char *fmt, _IN_ va_list ap);

/**
 * @brief ������ϵͳ�������������еĺ�����.
 *
 * @return ���غ�����.
 */
uint32_t HAL_GetTimeMs(void);

/**
 * @brief ����.
 *
 * @param ms ���ߵ�ʱ��, ��λ����.
 */
void HAL_SleepMs(_IN_ uint32_t ms);



/**
 * @brief �ж϶�ʱ��ʱ���Ƿ��Ѿ�����
 *
 * @param timer     ��ʱ���ṹ��
 * @return          ����1, ��ʾ��ʱ���ѹ���
 */
char HAL_Timer_expired(Timer *timer);

/**
 * @brief ���ݶ�ʱ����ʼ��ʱ, ��λ:ms
 *
 * @param timer         ��ʱ���ṹ��
 * @param timeout_ms    ��ʱʱ��, ��λ:ms
 */
void HAL_Timer_countdown_ms(Timer *timer, unsigned int timeout_ms);

/**
 * @brief ���ݶ�ʱ����ʼ��ʱ, ��λ:ms
 *
 * @param timer   ��ʱ���ṹ��
 * @param timeout ��ʱʱ��, ��λ:s
 */
void HAL_Timer_countdown(Timer *timer, unsigned int timeout);

/**
 * @brief ��������ʱ����ʣ�¶���ʱ��
 *
 * @param timer     ��ʱ���ṹ��
 * @return          ����ʣ��ʱ��
 */
int HAL_Timer_remain(Timer *timer);

/**
 * @brief ��ʼ����ʱ���ṹ��
 *
 * @param timer ��ʱ���ṹ��
 */
void HAL_Timer_init(Timer *timer);

/**
 * @brief ��ȡ��ǰʱ���ʽ���ַ��� %Y-%m-%d %z %H:%M:%S
 *
 * @return ��ǰʱ���ʽ���ַ���
 */
char* HAL_Timer_current(void);

/**
 * @brief ��ȡ��ǰʱ������
 *
 * @return ��ǰʱ����뼶����
 */
long HAL_Timer_current_sec(void);

#ifndef AUTH_WITH_NOTLS
/**
 * @brief TLS������ز�������
 *
 * �ڳ�ʼ��ʱ, ����Ҫ��ca֤�顢�ͻ���֤�顢�ͻ���˽Կ�ļ�������������������
 */
typedef struct {
    const char		 *ca_crt;
    uint16_t 		 ca_crt_len;

#ifdef AUTH_MODE_CERT
	/**
	 * �ǶԳƼ���
	 */
    const char       *cert_file;            // �ͻ���֤��
    const char       *key_file;             // �ͻ���˽Կ
#else
    /**
     * �ԳƼ���
     */
    const char       *psk;                  // �ԳƼ�����Կ
    const char       *psk_id;               // psk��ԿID
#endif

    size_t           psk_length;            // psk����

    unsigned int     timeout_ms;            // SSL���ֳ�ʱʱ��

} SSLConnectParams;

/********** TLS network **********/
typedef SSLConnectParams TLSConnectParams;

/**
 * @brief ΪMQTT�ͻ��˽���TLS����
 *
 * ��Ҫ��������:
 *     1. ��ʼ������, ����mbedtls���ʼ��, ���֤���ļ����ص�
 *     2. ����TCP socket����
 *     3. ����SSL����, ��������, ������֤�����
 *
 * @param   pConnectParams TLS���ӳ�ʼ������
 * @host    ��������
 * @port    ���Ӷ˿�
 * @return  ����0 ��ʾTLS���ӳɹ�
 */
uintptr_t HAL_TLS_Connect(TLSConnectParams *pConnectParams, const char *host, int port);

/**
 * @brief �Ͽ�TLS����, ���ͷ���ض�����Դ
 *
 * @param pParams TLS���Ӳ���
 */
void HAL_TLS_Disconnect(uintptr_t handle);

/**
 * @brief ͨ��TLS����д����
 *
 * @param handle        TLS����������ݽṹ
 * @param data          д������
 * @param totalLen      д�����ݳ���
 * @param timeout_ms    ��ʱʱ��, ��λ:ms
 * @param written_len   ��д�����ݳ���
 * @return              ��д���ݳɹ�, �򷵻�д�����ݵĳ���
 */
int HAL_TLS_Write(uintptr_t handle, unsigned char *data, size_t totalLen, uint32_t timeout_ms,
                                 size_t *written_len);

/**
 * @brief ͨ��TLS���Ӷ�����
 *
 * @param handle        TLS����������ݽṹ
 * @param data          ��ȡ����
 * @param totalLen      ��ȡ���ݵĳ���
 * @param timeout_ms    ��ʱʱ��, ��λ:ms
 * @param read_len      �Ѷ�ȡ���ݵĳ���
 * @return              �������ݳɹ�, �򷵻ض�ȡ���ݵĳ���
 */
int HAL_TLS_Read(uintptr_t handle, unsigned char *data, size_t totalLen, uint32_t timeout_ms,
                                size_t *read_len);


/********** DTLS network **********/
#ifdef COAP_COMM_ENABLED
typedef SSLConnectParams DTLSConnectParams;

/**
 * @brief ΪCoAP�ͻ��˽���DTLS����
 *
 * ��Ҫ��������:
 *     1. ��ʼ������, ����mbedtls���ʼ��, ���֤���ļ����ص�
 *     2. ����UDP socket����
 *     3. ����SSL����, ��������, ������֤�����
 *
 * @param pConnectParams DTLS���ӳ�ʼ������
 * @host    ��������
 * @port    ���Ӷ˿�
 * @return  ����0 ��ʾDTLS���ӳɹ�
 */
uintptr_t HAL_DTLS_Connect(DTLSConnectParams *pConnectParams, const char *host, int port);

/**
 * @brief �Ͽ�DTLS����
 *
 * @param handle DTLS����������ݽṹ
 * @return  ����0 ��ʾDTLS����
 */
void HAL_DTLS_Disconnect(uintptr_t handle);

/**
 * @brief ͨ��DTLS����д����
 *
 * @param pParams           DTLS����������ݽṹ
 * @param data              д������
 * @param datalen           д�����ݳ���
 * @param written_len       ��д�����ݳ���
 * @return                  ��д���ݳɹ�, �򷵻�д�����ݵĳ���
 */
int HAL_DTLS_Write(uintptr_t handle, const unsigned char *data, size_t datalen, size_t *written_len);

/**
 * @brief ͨ��DTLS���Ӷ�����
 *
 * @param handle            DTLS����������ݽṹ
 * @param data              ��ȡ����
 * @param timeout_ms        ��ʱʱ��, ��λ:ms
 * @param datalen   	    ��ȡ���ݵĳ���
 * @param read_len          �Ѷ�ȡ���ݵĳ���
 * @return                  �������ݳɹ�, �򷵻ض�ȡ���ݵĳ���
 */
int HAL_DTLS_Read(uintptr_t handle, unsigned char *data, size_t datalen, uint32_t timeout_ms,
                  size_t *read_len);

#endif //CoAP Enabled

#else
/********** TCP network **********/
/**
 * @brief ΪMQTT�ͻ��˽���TCP����
 *
 * @host    ��������
 * @port    ���Ӷ˿�
 * @return  ����0 ��ʾTCP����ʧ�ܣ����� > 0 ��ʾTCP����������FDֵ
 */
intptr_t HAL_TCP_Connect(const char *host, uint16_t port);

/**
 * @brief �Ͽ�TCP����
 *
 * @param fd TCP Socket������
 * @return  ����0 ��ʾTCP�����ɹ�
 */
int HAL_TCP_Disconnect(uintptr_t fd);

/**
 * @brief ͨ��TCP Socketд����
 *
 * @param fd           		TCP Socket������
 * @param buf              	д������
 * @param len           	д�����ݳ���
 * @param timeout_ms		��ʱʱ��
 * @param written_len       ��д�����ݳ���
 * @return                  ��д���ݳɹ�, �򷵻�д�����ݵĳ���
 */
int HAL_TCP_Write(uintptr_t fd, const unsigned char *buf, uint32_t len, uint32_t timeout_ms,
                size_t *written_len);

/**
 * @brief ͨ��TCP Socket������
 *
 * @param fd           		TCP Socket������
 * @param buf              	��������
 * @param len           	�������ݳ���
 * @param timeout_ms		��ʱʱ��
 * @param written_len       �Ѷ������ݳ���
 * @return                  �������ݳɹ�, �򷵻ض������ݵĳ���
 */
int HAL_TCP_Read(uintptr_t fd, unsigned char *buf, uint32_t len, uint32_t timeout_ms,
                size_t *read_len);

/********** UDP network **********/
#ifdef COAP_COMM_ENABLED
/**
 * @brief ����UDP����
 *
 * @host    ��������
 * @port    ���Ӷ˿�
 * @return  ����0 ��ʾUDP����ʧ�ܣ����� > 0 ��ʾUDP����������FDֵ
 */
uintptr_t HAL_UDP_Connect(const char *host, unsigned short port);

/**
 * @brief �Ͽ�UDP����
 *
 * @param fd UDP Socket������
 * @return
 */
void HAL_UDP_Disconnect(uintptr_t fd);

/**
 * @brief ͨ��UDP Socketд����
 *
 * @param fd           		UDP Socket������
 * @param buf              	д������
 * @param len           	д�����ݳ���
 * @return                  ��д���ݳɹ�, �򷵻�д�����ݵĳ���
 */
int HAL_UDP_Write(uintptr_t fd, const unsigned char *p_data, unsigned int datalen);

/**
 * @brief ͨ��TCP Socket������
 *
 * @param fd           		UDP Socket������
 * @param buf              	��������
 * @param len           	�������ݳ���
 * @return                  �������ݳɹ�, �򷵻ض������ݵĳ���
 */
int HAL_UDP_Read(uintptr_t fd, unsigned char *p_data, unsigned int datalen);

/**
 * @brief ͨ��TCP Socket������
 *
 * @param fd           		UDP Socket������
 * @param buf              	��������
 * @param len           	�������ݳ���
 * @param timeout_ms		��ʱʱ��
 * @return                  �������ݳɹ�, �򷵻ض������ݵĳ���
 */
int HAL_UDP_ReadTimeout(uintptr_t fd, unsigned char *p_data, unsigned int datalen, unsigned int timeout_ms);
#endif
#endif //NOTLS Enabled

#if defined(__cplusplus)
}
#endif
#endif  /* QCLOUD_IOT_IMPORT_H_ */

