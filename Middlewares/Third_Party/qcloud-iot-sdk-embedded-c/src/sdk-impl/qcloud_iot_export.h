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

#ifndef QCLOUD_IOT_EXPORT_H_
#define QCLOUD_IOT_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_compile_flag.h"


/* IoT C-SDK APPID */
#define QCLOUD_IOT_DEVICE_SDK_APPID                                	"21010406"

/* MQTT������Ϣ��������, ��λ:ms */
#define QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL                         (240 * 1000)

/* MQTT ��������(��������, ����, ������)�ĳ�ʱʱ��, ��λ:ms ����5000ms */
#define QCLOUD_IOT_MQTT_COMMAND_TIMEOUT                             (20 * 1000)

/* TLS�������ֳ�ʱʱ��, ��λ:ms */
#define QCLOUD_IOT_TLS_HANDSHAKE_TIMEOUT                            (5 * 1000)

/* �豸ID����󳤶�, ���뱣��Ψһ */
#define MAX_SIZE_OF_CLIENT_ID                                       (80)

/* ��Ʒ���Ƶ���󳤶� */
#define MAX_SIZE_OF_PRODUCT_ID                                    	(10)

/* �豸ID����󳤶� */
#define MAX_SIZE_OF_DEVICE_NAME                                     (64)

/* MQTT��Ϣ����buffer��С, ֧�����256*1024 */
#define QCLOUD_IOT_MQTT_TX_BUF_LEN                                  (1024)

/* MQTT��Ϣ����buffer��С, ֧�����256*1024 */
#define QCLOUD_IOT_MQTT_RX_BUF_LEN                                  (1024)

/* COAP ������Ϣbuffer��С�����֧��64*1024�ֽ� */
#define COAP_SENDMSG_MAX_BUFLEN                   					(512)

/* COAP ������Ϣbuffer��С�����֧��64*1024�ֽ� */
#define COAP_RECVMSG_MAX_BUFLEN                   					(512)

/* �������ȴ�ʱ�� */
#define MAX_RECONNECT_WAIT_INTERVAL 								(60 * 1000)

/* ��������Чʱ��ƫ�ƣ�ʱ����룬0����ʾ�������ڣ�>0����ʾ�ڵ�ǰʱ��+timeout���̨��ʱ*/
#define MAX_ACCESS_EXPIRE_TIMEOUT									(0)

/* MQTT�������� */
#define QCLOUD_IOT_MQTT_DIRECT_DOMAIN        						"iotcloud.tencentdevices.com"

/* CoAP�������� */
#define QCLOUD_IOT_COAP_DEIRECT_DOMAIN								"iotcloud.tencentdevices.com"

typedef struct {
	char	product_id[MAX_SIZE_OF_PRODUCT_ID + 1];
	char 	device_name[MAX_SIZE_OF_DEVICE_NAME + 1];
	char	client_id[MAX_SIZE_OF_CLIENT_ID + 1];
} DeviceInfo;


#include "qcloud_iot_export_log.h"
#include "qcloud_iot_export_error.h"

#ifdef MQTT_COMM_ENABLED
#include "qcloud_iot_export_mqtt.h"
#include "mqtt_client.h"
#endif

#ifdef COAP_COMM_ENABLED
#include "qcloud_iot_export_coap.h"
#endif

#ifdef MQTT_DEVICE_SHADOW
#include "qcloud_iot_export_shadow.h"
#endif

#ifdef SYSTEM_COMM
#include "qcloud_iot_export_system.h"
#endif

#ifdef SYSTEM_COMM
#include "qcloud_iot_export_system.h"
#endif

#ifdef NBIOT_COMM_ENABLED
#include "qcloud_iot_export_nbiot.h"
#endif

#ifdef OTA_ENABLE
#include "qcloud_iot_export_ota.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_H_ */
