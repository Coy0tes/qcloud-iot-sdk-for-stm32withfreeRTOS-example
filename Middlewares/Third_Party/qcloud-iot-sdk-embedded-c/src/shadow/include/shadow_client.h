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

/**
 * @brief �豸Ӱ���ĵ�������ص�һЩ�ӿ�
 *
 * �����ṩһЩ�ӿ����ڹ����豸Ӱ���ĵ������豸Ӱ���ĵ����н���; ͨ��DeviceName,
 * �������豸Ӱ�ӽ��н���, ������ǰ�豸���豸Ӱ�Ӻ������豸���豸Ӱ��; һ
 * ���豸һ�������ֲ�ͬ�������豸Ӱ�ӽ���:
 *     1. Get
 *     2. Update
 *     3. Delete
 *
 * �������ֲ���, �ײ㻹�ǻ���MQTTЭ��, ����ԭ��Ҳ�ǻ��ڷ���/����ģ��, ��ִ��
 * ����������, ���յ���Ӧ����Ӧ: 1. accepted; 2. rejected������, ����ִ��
 * Get���豸Ӱ�ӽ��н���, �豸�˽����ͺͽ��յ�һ����Ϣ:
 *     1. ����MQTT����: $shadow/get/{productName}/{deviceName};
 *     2. ����MQTT����: $shadow/get/accepted/{productName}/{deviceName} �� $shadow/get/rejected/{productName}/{deviceName}
 *     3. �����������ɹ��Ļ�, �豸�˻��յ�accepted����, �Լ���Ӧ�豸��json�ĵ���
 */
#ifndef IOT_SHADOW_CLIENT_H_
#define IOT_SHADOW_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "qcloud_iot_sdk_impl_internal.h"
#include "device.h"
#include "mqtt_client.h"
#include "shadow_client_json.h"

/* ���������ʱ����, ����appending״̬������������ */
#define MAX_APPENDING_REQUEST_AT_ANY_GIVEN_TIME                     (10)

/* һ��clientToken����󳤶� */
#define MAX_SIZE_OF_CLIENT_TOKEN                                    (MAX_SIZE_OF_CLIENT_ID + 10)

/* һ��������clientToken�ֶε�JSON�ĵ�����󳤶� */
#define MAX_SIZE_OF_JSON_WITH_CLIENT_TOKEN                          (MAX_SIZE_OF_CLIENT_TOKEN + 20)

/* ���������ʱ����, ����ͬʱ�����豸�������� */
#define MAX_DEVICE_HANDLED_AT_ANY_GIVEN_TIME                        (10)

/* ���豸����֮��, �ƶ˱����������󳤶� */
#define MAX_SIZE_OF_CLOUD_TOPIC_WITHOUT_DEVICE_NAME                 (60)

/* �����ƶ˷��ص�JSON�ĵ���buffer��С */
#define CLOUD_IOT_JSON_RX_BUF_LEN                                   (QCLOUD_IOT_MQTT_RX_BUF_LEN + 1)

/**
 * @brief �ĵ���������Ĳ����ṹ�嶨��
 */
typedef struct _RequestParam {

    Method               	method;              	// �ĵ�����ʽ: GET, UPDATE, DELETE

    uint32_t             	timeout_sec;         	// ����ʱʱ��, ��λ:s

    OnRequestCallback    	request_callback;    	// ����ص�����

    void                 	*user_context;          // �û�����, ��ͨ���ص�����OnRequestCallback����

} RequestParams;

#define DEFAULT_REQUEST_PARAMS {GET, 4, NULL, NULL};

/**
 * @brief �ýṹ�����ڱ����ѵǼǵ��豸���Լ��豸���Դ���Ļص�����
 */
typedef struct {

    void *property;							// �豸����

    OnPropRegCallback callback;      // �ص�������

} PropertyHandler;

typedef struct _ShadowInnerData {
    uint32_t token_num;
    uint32_t version;
    int32_t sync_status;
    List *request_list;
    List *property_handle_list;
    char *result_topic;
} ShadowInnerData;

typedef struct _Shadow {
    void *mqtt;
    void *mutex;
    MQTTEventHandler event_handle;
    ShadowInnerData inner_data;
} Qcloud_IoT_Shadow;

int qcloud_iot_shadow_init(Qcloud_IoT_Shadow *pShadow);

void qcloud_iot_shadow_reset(void *pClient);

/**
 * @brief ��������������Ѿ���ʱ������
 * 
 * @param pShadow   shadow client
 */
void handle_expired_request(Qcloud_IoT_Shadow *pShadow);

/**
 * @brief ���е��ƶ��豸�ĵ���������, ͨ���÷���������ת�ַ�
 *
 * @param pShadow       shadow client
 * @param pParams  		�������
 * @param pJsonDoc 		�����ĵ�
 * @param sizeOfBuffer 	�ĵ���������С
 * @return         		����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int do_shadow_request(Qcloud_IoT_Shadow *pShadow, RequestParams *pParams, char *pJsonDoc, size_t sizeOfBuffer);

/**
 * @brief �����豸Ӱ�Ӳ������topic
 *
 * @param pShadow       shadow client
 * @param pParams  		�������
 * @param pJsonDoc 		�����ĵ�
 * @param sizeOfBuffer 	�ĵ���������С
 * @return         		����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int subscribe_operation_result_to_cloud(Qcloud_IoT_Shadow *pShadow);

#ifdef __cplusplus
}
#endif

#endif /* IOT_SHADOW_CLIENT_H_ */
