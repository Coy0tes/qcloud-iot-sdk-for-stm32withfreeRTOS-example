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

#ifndef QCLOUD_IOT_EXPORT_SHADOW_H_
#define QCLOUD_IOT_EXPORT_SHADOW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_export_mqtt.h"

typedef struct {
    /**
     * �豸������Ϣ
     */
    char                        *product_id;            // ��Ʒ����
    char                        *device_name;           // �豸����

#ifdef AUTH_MODE_CERT
    /**
     * �ǶԳƼ���ʹ��
     */
    char                        *cert_file;              // �ͻ���֤���ļ�·��
    char                        *key_file;               // �ͻ���˽Կ�ļ�·��
#else
    /**
     * �ԳƼ���
     */
    char                        *device_secret;                    // �ԳƼ�����Կ
#endif

    uint32_t                    command_timeout;         // �������������д��ʱʱ�� ms
    uint32_t                    keep_alive_interval_ms;  // ��������, ��λ: ms

    uint8_t                     clean_session;           // ����Ự��־λ

    uint8_t                     auto_connect_enable;     // �Ƿ����Զ����� 1:�����Զ����� 0���������Զ�����  ����Ϊ1

    MQTTEventHandler            event_handle;            // �¼��ص�

} ShadowInitParams;

#ifdef AUTH_MODE_CERT
    #define DEFAULT_SHAWDOW_INIT_PARAMS { NULL, NULL, NULL, NULL, 20000, 240 * 1000, 1, 1, {0}}
#else
    #define DEFAULT_SHAWDOW_INIT_PARAMS { NULL, NULL, NULL, 20000, 240 * 1000, 1, 1, {0}}
#endif

/**
 * @brief ������Ӧ���ص�����
 */
typedef enum {
    ACK_NONE = -3,      // ����ʱ
    ACK_TIMEOUT = -2,   // ����ʱ
    ACK_REJECTED = -1,  // ����ܾ�
    ACK_ACCEPTED = 0    // �������
} RequestAck;

/**
 * @brief �����ƶ��豸�ĵ�����ʹ�õ����ַ�ʽ
 */
typedef enum {
    GET,     // ��ȡ�ƶ��豸�ĵ�
    UPDATE,  // ���»򴴽��ƶ��豸�ĵ�
} Method;

/**
 * @brief JSON�ĵ���֧�ֵ���������
 */
typedef enum {
    JINT32,     // 32λ�з�������
    JINT16,     // 16λ�з�������
    JINT8,      // 8λ�з�������
    JUINT32,    // 32λ�޷�������
    JUINT16,    // 16λ�޷�������
    JUINT8,     // 8λ�޷�������
    JFLOAT,     // �����ȸ�����
    JDOUBLE,    // ˫���ȸ�����
    JBOOL,      // ������
    JSTRING,    // �ַ���
    JOBJECT     // JSON����
} JsonDataType;

/**
 * @brief �����豸��ĳ������, ʵ�ʾ���һ��JSON�ĵ��ڵ�
 */
typedef struct _JSONNode {
    char   		 *key;    // ��JSON�ڵ��Key
    void         *data;   // ��JSON�ڵ��Value
    JsonDataType type;    // ��JSON�ڵ����������
} DeviceProperty;

/**
 * @brief ÿ���ĵ�������Ӧ�Ļص�����
 *
 * @param method         �ĵ�������ʽ
 * @param requestAck     ������Ӧ����
 * @param pJsonDocument  �ƶ���Ӧ���ص��ĵ�
 * @param userContext      �û�����
 *
 */
typedef void (*OnRequestCallback)(void *pClient, Method method, RequestAck requestAck, const char *pJsonDocument, void *userContext);

/**
 * @brief �豸���Դ���ص�����
 *
 * @param pJsonValueBuffer �豸����ֵ
 * @param valueLength      �豸����ֵ����
 * @param DeviceProperty   �豸���Խṹ��
 */
typedef void (*OnPropRegCallback)(void *pClient, const char *pJsonValueBuffer, uint32_t valueLength, DeviceProperty *pProperty);

/**
 * @brief ����ShadowClient
 *
 * @param pInitParams MQTTЭ�����ӽ���������ά�ֽ׶�����Ҫ�Ĳ���
 *
 * @return ����NULL: ����ʧ��
 */
void* IOT_Shadow_Construct(ShadowInitParams *pParams);

/**
 * @brief ����MQTT��Ϣ
 *
 * @param handle        Shadow�ͻ��˽ṹ��
 * @param topicName     ������
 * @param pParams       ��������
 * @return < 0  :   ��ʾʧ��
 *         >= 0 :   ����Ψһ��packet id 
 */
int IOT_Shadow_Publish(void *handle, char *topicName, PublishParams *pParams);

/**
 * @brief ����MQTT��Ϣ
 *
 * @param handle        Shadow�ͻ��˽ṹ��
 * @param topicName     ������
 * @param pParams       ��������
 * @return <  0  :   ��ʾʧ��
 *         >= 0 :   ����Ψһ��packet id
 */
int IOT_Shadow_Subscribe(void *handle, char *topicFilter, SubscribeParams *pParams);

/**
 * @brief ȡ������MQTT��Ϣ
 *
 * @param handle        Shadow�ͻ��˽ṹ��
 * @param topicName     ������
 * @return <  0  :   ��ʾʧ��
 *         >= 0 :   ����Ψһ��packet id
 */
int IOT_Shadow_Unsubscribe(void *handle, char *topicFilter);

/**
 * @brief �ͻ���Ŀǰ�Ƿ�������
 *
 * @param pClient Shadow Client�ṹ��
 * @return ����true, ��ʾ�ͻ���������
 */
bool IOT_Shadow_IsConnected(void *handle);

/**
 * @brief ����ShadowClient �ر�MQTT����
 *
 * @param pClient ShadowClient����
 *
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int IOT_Shadow_Destroy(void *handle);

/**
 * @brief ��Ϣ����, ����������, ��ʱ������
 *
 * @param timeout_ms ��ʱʱ��, ��λ:ms
 * @return           ����QCLOUD_ERR_SUCCESS, ��ʾ���óɹ�
 */
int IOT_Shadow_Yield(void *handle, uint32_t timeout_ms);

/**
 * @brief �첽��ʽ�����豸Ӱ���ĵ�
 *
 * @param pClient           Client�ṹ��
 * @param pJsonDoc          ���µ��ƶ˵��豸�ĵ�
 * @param sizeOfBuffer      �ĵ�����
 * @param callback          ������Ӧ����ص�����
 * @param userContext       �û�����, ������Ӧ����ʱͨ���ص���������
 * @param timeout_ms        ����ʱʱ��, ��λ:ms
 * @return                  ����QCLOUD_ERR_SUCCESS, ��ʾ����ɹ�
 */
int IOT_Shadow_Update(void *handle, char *pJsonDoc, size_t sizeOfBuffer, OnRequestCallback callback, void *userContext, uint32_t timeout_ms);

/**
 * @brief ͬ����ʽ�����豸Ӱ���ĵ�
 *
 * @param pClient           Client�ṹ��
 * @param pJsonDoc          ���µ��ƶ˵��豸�ĵ�
 * @param sizeOfBuffer      �ĵ�����
 * @param timeout_ms        ����ʱʱ��, ��λ:ms
 * @return                  QCLOUD_ERR_SUCCESS ����ɹ�
 *                          QCLOUD_ERR_SHADOW_UPDATE_TIMEOUT ����ʱ
 *                          QCLOUD_ERR_SHADOW_UPDATE_REJECTED ���󱻾ܾ�
 */
int IOT_Shadow_Update_Sync(void *handle, char *pJsonDoc, size_t sizeOfBuffer, uint32_t timeout_ms);

/**
 * @brief ��ȡ�豸Ӱ���ĵ�
 *
 * @param pClient           Client�ṹ��
 * @param callback          ������Ӧ����ص�����
 * @param userContext       �û�����, ������Ӧ����ʱͨ���ص���������
 * @param timeout_ms        ����ʱʱ��, ��λ:s
 * @return                  ����QCLOUD_ERR_SUCCESS, ��ʾ����ɹ�
 */
int IOT_Shadow_Get(void *handle, OnRequestCallback callback, void *userContext, uint32_t timeout_ms);

/**
 * @brief ��ȡ�豸Ӱ���ĵ�
 *
 * @param pClient           Client�ṹ��
 * @param timeout_ms        ����ʱʱ��, ��λ:s
 * @return                  QCLOUD_ERR_SUCCESS ����ɹ�
 *                          QCLOUD_ERR_SHADOW_GET_TIMEOUT ����ʱ
 *                          QCLOUD_ERR_SHADOW_GET_REJECTED ���󱻾ܾ�
 */
int IOT_Shadow_Get_Sync(void *handle, uint32_t timeout_ms);

/**
 * @brief ע�ᵱǰ�豸���豸����
 *
 * @param pClient    Client�ṹ��
 * @param pProperty  �豸����
 * @param callback   �豸���Ը��»ص�������
 * @return           ����QCLOUD_ERR_SUCCESS, ��ʾ����ɹ�
 */
int IOT_Shadow_Register_Property(void *handle, DeviceProperty *pProperty, OnPropRegCallback callback);

/**
 * @brief ɾ���Ѿ�ע������豸����
 *
 * @param pClient    Client�ṹ��
 * @param pProperty  �豸����
 * @return           ����QCLOUD_ERR_SUCCESS, ��ʾ����ɹ�
 */
int IOT_Shadow_UnRegister_Property(void *handle, DeviceProperty *pProperty);

/**
 * @brief ��JSON�ĵ������reported�ֶΣ������Ǹ���
 *
 *
 * @param jsonBuffer    Ϊ�洢JSON�ĵ�׼�����ַ���������
 * @param sizeOfBuffer  ��������С
 * @param count         �ɱ�����ĸ���, �����ϱ����豸���Եĸ���
 * @return              ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int IOT_Shadow_JSON_ConstructReport(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...);

/**
 * @brief ��JSON�ĵ������reported�ֶΣ����Ǹ���
 *
 *
 * @param jsonBuffer    Ϊ�洢JSON�ĵ�׼�����ַ���������
 * @param sizeOfBuffer  ��������С
 * @param overwrite		��д�ֶ�
 * @param count         �ɱ�����ĸ���, �����ϱ����豸���Եĸ���
 * @return              ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int IOT_Shadow_JSON_Construct_OverwriteReport(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...);

/**
 * @brief ��JSON�ĵ������reported�ֶΣ�ͬʱ���desired�ֶ�
 *
 *
 * @param jsonBuffer    Ϊ�洢JSON�ĵ�׼�����ַ���������
 * @param sizeOfBuffer  ��������С
 * @param count         �ɱ�����ĸ���, �����ϱ����豸���Եĸ���
 * @return              ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int IOT_Shadow_JSON_ConstructReportAndDesireAllNull(void *handle, char *jsonBuffer, size_t sizeOfBuffer, uint8_t count, ...);

/**
 * @brief ��JSON�ĵ������ "desired": null �ֶ�
 *
 * @param jsonBuffer   Ϊ�洢JSON�ĵ�׼�����ַ���������
 * @param sizeOfBuffer  ��������С
 */
int IOT_Shadow_JSON_ConstructDesireAllNull(void *handle, char *jsonBuffer, size_t sizeOfBuffer);

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_SHADOW_H_ */
