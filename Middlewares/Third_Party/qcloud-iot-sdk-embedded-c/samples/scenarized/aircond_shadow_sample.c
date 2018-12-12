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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "lite-utils.h"

/* ��Ʒ����, ���ƶ�ͬ���豸״̬ʱ��Ҫ  */
#define QCLOUD_IOT_MY_PRODUCT_ID            "YOUR_PRODUCT_ID"
/* �豸����, ���ƶ�ͬ���豸״̬ʱ��Ҫ */
#define QCLOUD_IOT_MY_DEVICE_NAME           "YOUR_DEVICE_NAME"

#ifdef AUTH_MODE_CERT
    /* �ͻ���֤���ļ���  �ǶԳƼ���ʹ��*/
    #define QCLOUD_IOT_CERT_FILENAME          "YOUR_DEVICE_NAME_cert.crt"
    /* �ͻ���˽Կ�ļ��� �ǶԳƼ���ʹ��*/
    #define QCLOUD_IOT_KEY_FILENAME           "YOUR_DEVICE_NAME_private.key"

    static char sg_cert_file[PATH_MAX + 1];      //�ͻ���֤��ȫ·��
    static char sg_key_file[PATH_MAX + 1];       //�ͻ�����Կȫ·��

#else
    #define QCLOUD_IOT_DEVICE_SECRET                  "YOUR_IOT_PSK"
#endif

#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 200

#define ROOM_TEMPERATURE 32.0f
static float sg_desire_temperature = 20.0f;
static float sg_report_temperature = ROOM_TEMPERATURE;

static float sg_energy_consumption = 0.0f;
static bool sg_airconditioner_openned = false;

static MQTTEventType sg_subscribe_event_result = MQTT_EVENT_UNDEF;

#define MAX_RECV_LEN (512 + 1)

/**
 * �Ƚϸ������Ƿ����
 *
 * @param left ��ֵ
 * @param right ��ֵ
 */
bool _is_value_equal(float left, float right)
{
    if((left<right+0.01) && (left>right-0.01))
        return true;
    else
        return false;
}

/**
 * ģ�������¶�
 *
 * @param roomTemperature
 */
static void _simulate_room_temperature(float *roomTemperature) {
    float delta_change = 0;

    if (!sg_airconditioner_openned) {
        if(!_is_value_equal(*roomTemperature, ROOM_TEMPERATURE)) {
            delta_change = (*roomTemperature)>ROOM_TEMPERATURE ? -0.5: 0.5;
        }
    } else {
        sg_energy_consumption += 1;
        if (!_is_value_equal(*roomTemperature, sg_desire_temperature)) {
            delta_change = (*roomTemperature)>sg_desire_temperature ? -1.0: 1.0;
        }
    }

    *roomTemperature += delta_change;
}

static void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg) 
{	
	uintptr_t packet_id = (uintptr_t)msg->msg;

	switch(msg->event_type) {
		case MQTT_EVENT_UNDEF:
			Log_i("undefined event occur.");
			break;

		case MQTT_EVENT_DISCONNECT:
			Log_i("MQTT disconnect.");
			break;

		case MQTT_EVENT_RECONNECT:
			Log_i("MQTT reconnect.");
			break;

		case MQTT_EVENT_SUBCRIBE_SUCCESS:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
            sg_subscribe_event_result = msg->event_type;
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_SUCCESS:
			Log_i("publish success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_TIMEOUT:
			Log_i("publish timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_PUBLISH_NACK:
			Log_i("publish nack, packet-id=%u", (unsigned int)packet_id);
			break;
		default:
			Log_i("Should NOT arrive here.");
			break;
	}
}

/**
 * MQTT��Ϣ���մ�����
 *
 * @param topicName         topic����
 * @param topicNameLen      topic����
 * @param message           �Ѷ�����Ϣ�Ľṹ
 * @param userData         ��Ϣ����
 */
static void on_message_callback(void *pClient, MQTTMessage *message, void *userData) 
{
    if (message == NULL)
        return;

    const char *topicName = message->ptopic; 
    size_t topicNameLen = message->topic_len;

    if (topicName == NULL || topicNameLen == 0) {
        return;
    }

    Log_i("Receive Message With topicName:%.*s, payload:%.*s",
          (int) topicNameLen, topicName, (int) message->payload_len, (char *) message->payload);

    static char cloud_rcv_buf[MAX_RECV_LEN];
    memcpy(cloud_rcv_buf, message->payload, message->payload_len);
    cloud_rcv_buf[message->payload_len] = '\0';    // jsmn_parse relies on a string

    char* value = LITE_json_value_of("action", cloud_rcv_buf);
    if (value != NULL) {
        if(strcmp(value, "come_home") == 0)
        {
            sg_airconditioner_openned = true;
        }
        else if(strcmp(value, "leave_home") == 0)
        {
            sg_airconditioner_openned = false;
        }
    }

    HAL_Free(value);
}

/**
 * ����MQTT connet��ʼ������
 *
 * @param initParams MQTT connet��ʼ������
 *
 * @return 0: ������ʼ���ɹ�  ��0: ʧ��
 */
static int _setup_connect_init_params(ShadowInitParams* initParams)
{
	initParams->device_name = QCLOUD_IOT_MY_DEVICE_NAME;
	initParams->product_id = QCLOUD_IOT_MY_PRODUCT_ID;

#ifdef AUTH_MODE_CERT
    // ��ȡCA֤�顢�ͻ���֤���Լ�˽Կ�ļ���·��
    char certs_dir[PATH_MAX + 1] = "certs";
    char current_path[PATH_MAX + 1];
    char *cwd = getcwd(current_path, sizeof(current_path));
    if (cwd == NULL)
    {
        Log_e("getcwd return NULL");
        return QCLOUD_ERR_FAILURE;
    }
    sprintf(sg_cert_file, "%s/%s/%s", current_path, certs_dir, QCLOUD_IOT_CERT_FILENAME);
    sprintf(sg_key_file, "%s/%s/%s", current_path, certs_dir, QCLOUD_IOT_KEY_FILENAME);

    initParams->cert_file = sg_cert_file;
    initParams->key_file = sg_key_file;
#else
    initParams->device_secret = QCLOUD_IOT_DEVICE_SECRET;
#endif

    initParams->auto_connect_enable = 1;
    initParams->event_handle.h_fp = event_handler;

    return QCLOUD_ERR_SUCCESS;
}

/**
 * ���Ĺ�עtopic��ע����Ӧ�ص�����
 *
 */
static int _register_subscribe_topics(void *client)
{
    static char topic_name[128] = {0};
    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", QCLOUD_IOT_MY_PRODUCT_ID, QCLOUD_IOT_MY_DEVICE_NAME, "control");
    if (size < 0 || size > sizeof(topic_name) - 1)
    {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }
    SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = on_message_callback;
    return IOT_Shadow_Subscribe(client, topic_name, &sub_params);
}

int main(int argc, char **argv) {
    int rc;

    //init log level
    IOT_Log_Set_Level(DEBUG);

    //init connection
    ShadowInitParams init_params = DEFAULT_SHAWDOW_INIT_PARAMS;
    rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_ERR_SUCCESS) {
		return rc;
	}

    void *client = IOT_Shadow_Construct(&init_params);
    if (client != NULL) {
        Log_i("Cloud Device Construct Success");
    } else {
        Log_e("Cloud Device Construct Failed");
        return QCLOUD_ERR_FAILURE;
    }

    //register subscribe topics here
    rc = _register_subscribe_topics(client);
    if (rc < 0) {
        Log_e("Client Subscribe Topic Failed: %d", rc);
        return rc;
    }

    while (IOT_Shadow_IsConnected(client) || rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT || rc == QCLOUD_ERR_MQTT_RECONNECTED) {

        rc = IOT_Shadow_Yield(client, 200);

        if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) {
            sleep(1);
            continue;
        }
		else if (rc != QCLOUD_ERR_SUCCESS) {
			Log_e("Exit loop caused of errCode: %d", rc);
		}

        if (sg_subscribe_event_result != MQTT_EVENT_SUBCRIBE_SUCCESS &&
            sg_subscribe_event_result != MQTT_EVENT_SUBCRIBE_TIMEOUT &&
            sg_subscribe_event_result != MQTT_EVENT_SUBCRIBE_NACK)
        {
            Log_i("Wait for subscribe result��sg_subscribe_event_result = %d", sg_subscribe_event_result);
            sleep(1);
            continue;
        }

        _simulate_room_temperature(&sg_report_temperature);
        Log_i("airConditioner state: %s", sg_airconditioner_openned ? "open" : "close");
        Log_i("currentTemperature: %f, energyConsumption: %f", sg_report_temperature, sg_energy_consumption);

        sleep(1);
    }

    rc = IOT_Shadow_Destroy(client);

    //ע������쳣�˳����

    return rc;
}
