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
//#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"


/* ��Ʒ����, ���ƶ�ͬ���豸״̬ʱ��Ҫ  */
#define QCLOUD_IOT_MY_PRODUCT_ID            "03UKNYBUZG"
/* �豸����, ���ƶ�ͬ���豸״̬ʱ��Ҫ */
#define QCLOUD_IOT_MY_DEVICE_NAME           "general_sdev1"

#ifdef AUTH_MODE_CERT
    /* �ͻ���֤���ļ���  �ǶԳƼ���ʹ��*/
    #define QCLOUD_IOT_CERT_FILENAME          "YOUR_DEVICE_NAME_cert.crt"
    /* �ͻ���˽Կ�ļ��� �ǶԳƼ���ʹ��*/
    #define QCLOUD_IOT_KEY_FILENAME           "YOUR_DEVICE_NAME_private.key"

    static char sg_cert_file[PATH_MAX + 1];      //�ͻ���֤��ȫ·��
    static char sg_key_file[PATH_MAX + 1];       //�ͻ�����Կȫ·��

#else
    #define QCLOUD_IOT_DEVICE_SECRET                  "VI04Eh4N8VgM29U/dnu9cQ=="
#endif

#define MAX_SIZE_OF_TOPIC_CONTENT 100

static int sg_count = 0;
static int sg_sub_packet_id = -1;

bool log_handler(const char* message) {
	//ʵ����־�ص���д����
	//ʵ�����ݺ��뷵��true
	return false;
}

void event_handler(void *pclient, void *handle_context, MQTTEventMsg *msg) {
	MQTTMessage* mqtt_messge = (MQTTMessage*)msg->msg;
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

		case MQTT_EVENT_PUBLISH_RECVEIVED:
			Log_i("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
					  mqtt_messge->topic_len,
					  mqtt_messge->ptopic,
					  mqtt_messge->payload_len,
					  mqtt_messge->payload);
			break;
		case MQTT_EVENT_SUBCRIBE_SUCCESS:
			Log_i("subscribe success, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
			break;

		case MQTT_EVENT_SUBCRIBE_TIMEOUT:
			Log_i("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
			break;

		case MQTT_EVENT_SUBCRIBE_NACK:
			Log_i("subscribe nack, packet-id=%u", (unsigned int)packet_id);
			sg_sub_packet_id = packet_id;
			break;

		case MQTT_EVENT_UNSUBCRIBE_SUCCESS:
			Log_i("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
			Log_i("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
			break;

		case MQTT_EVENT_UNSUBCRIBE_NACK:
			Log_i("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
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
static void on_message_callback(void *pClient, MQTTMessage *message, void *userData) {
	if (message == NULL) {
		return;
	}

	Log_i("Receive Message With topicName:%.*s, payload:%.*s",
		  (int) message->topic_len, message->ptopic, (int) message->payload_len, (char *) message->payload);
}

/**
 * ����MQTT connet��ʼ������
 *
 * @param initParams MQTT connet��ʼ������
 *
 * @return 0: ������ʼ���ɹ�  ��0: ʧ��
 */
static int _setup_connect_init_params(MQTTInitParams* initParams)
{
	initParams->device_name = QCLOUD_IOT_MY_DEVICE_NAME;
	initParams->product_id = QCLOUD_IOT_MY_PRODUCT_ID;

#ifdef AUTH_MODE_CERT
	/* ʹ�÷ǶԳƼ���*/
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

	initParams->command_timeout = QCLOUD_IOT_MQTT_COMMAND_TIMEOUT;
	initParams->keep_alive_interval_ms = QCLOUD_IOT_MQTT_KEEP_ALIVE_INTERNAL;

	initParams->auto_connect_enable = 1;
	initParams->event_handle.h_fp = event_handler;
	initParams->event_handle.context = NULL;

    return QCLOUD_ERR_SUCCESS;
}

/**
 * ����topic��Ϣ
 *
 */
static int _publish_msg(void *client)
{
    char topicName[128] = {0};
    sprintf(topicName,"%s/%s/%s", QCLOUD_IOT_MY_PRODUCT_ID, QCLOUD_IOT_MY_DEVICE_NAME, "data");

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos = QOS1;

    char topic_content[MAX_SIZE_OF_TOPIC_CONTENT + 1] = {0};

	int size = HAL_Snprintf(topic_content, sizeof(topic_content), "{\"action\": \"publish_test\", \"count\": \"%d\"}", sg_count++);
	if (size < 0 || size > sizeof(topic_content) - 1)
	{
		Log_e("payload content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_content));
		return -3;
	}

	pub_params.payload = topic_content;
	pub_params.payload_len = strlen(topic_content);

    return IOT_MQTT_Publish(client, topicName, &pub_params);
}

/**
 * ���Ĺ�עtopic��ע����Ӧ�ص�����
 *
 */
static int _register_subscribe_topics(void *client)
{
    static char topic_name[128] = {0};
    int size = HAL_Snprintf(topic_name, sizeof(topic_name), "%s/%s/%s", QCLOUD_IOT_MY_PRODUCT_ID, QCLOUD_IOT_MY_DEVICE_NAME, "data");
    if (size < 0 || size > sizeof(topic_name) - 1)
    {
        Log_e("topic content length not enough! content size:%d  buf size:%d", size, (int)sizeof(topic_name));
        return QCLOUD_ERR_FAILURE;
    }
	Log_i("topic_name:%s", topic_name);
    SubscribeParams sub_params = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = on_message_callback;
    return IOT_MQTT_Subscribe(client, topic_name, &sub_params);
}

extern MQTTConnectParams g_connect_params;

int mqtt_reconnect(Qcloud_IoT_Client *pClient)
{
	int rc;
	
	   
	while(1)
	{
		if(QCLOUD_ERR_SUCCESS == net_init_by_at())
		{
			Log_i("net reinit at mqtt_reconnect ok");
			break;
		}
		else
		{
			Log_i("net reinit at mqtt_reconnect fail");
			HAL_SleepMs(1000);
		}
	}

	pClient->next_packet_id = _get_random_start_packet_id();
	rc = qcloud_iot_mqtt_connect(pClient, &g_connect_params);
	if (rc != QCLOUD_ERR_SUCCESS) 
	{
		Log_e("mqtt connect with id: %s failed: %d", pClient->options.conn_id, rc);	
	}
	else 
	{
		Log_i("mqtt connect with id: %s success", pClient->options.conn_id);
	}

	return rc;

}

void mqtt_sample(void) 
{
	 int rc;

    //init log level
    IOT_Log_Set_Level(LOG_DEBUG);
    IOT_Log_Set_MessageHandler(log_handler);



	
	Log_i("mqtt_sample start");
	
	mem_info();

    //init connection
    MQTTInitParams init_params = DEFAULT_MQTTINIT_PARAMS;
    rc = _setup_connect_init_params(&init_params);
	if (rc != QCLOUD_ERR_SUCCESS) 
	{
		goto end;
	}	

    void *client = IOT_MQTT_Construct(&init_params);
    if (client != NULL) 
	{
        Log_i("Cloud Device Construct Success");
    } 
	else 
    {
        Log_e("Cloud Device Construct Failed");
        goto end;
    }

	mem_info();
	
#ifdef SYSTEM_COMM
    long time = 0;

	rc = IOT_SYSTEM_GET_TIME(client, &time);
	if (QCLOUD_ERR_SUCCESS == rc){
		Log_i("the time is %ld", time);
	}
	else{
		Log_e("get system time failed!");
	}
#endif

start:

	//register subscribe topics here
    rc = _register_subscribe_topics(client);
    if (rc < 0) 
	{
        Log_e("Client Subscribe Topic Failed: %d", rc);
        goto end;
    }
	else
	{
		Log_d("Subscribe Topic success");
	}

	mem_info();

	//rc = IOT_MQTT_Yield(client, 2000);

	//mem_info();
	


	Log_d("Start mqtt Loop");
    do 
	{
		mem_info();
    	rc = IOT_MQTT_Yield(client, 5000);
		if (rc == QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT) 
		{
			HAL_SleepMs(1000);
			continue;
		}
		else if (rc != QCLOUD_ERR_SUCCESS && rc != QCLOUD_ERR_MQTT_RECONNECTED)
		{
			Log_e("exit with error: %d", rc);
			break;
		}

		// �ȴ����Ľ��
		if (sg_sub_packet_id > 0) 
		{
			rc = _publish_msg(client);
			if (rc < 0) 
			{
				Log_e("client publish topic failed :%d.", rc);
				break;
			}
		}

		HAL_SleepMs(1000);

    } while (1);
	
end:	
	 //ע������쳣�˳����
	rc = IOT_MQTT_Destroy(&client);  //for card demo situation never come here, you should adjust this logic for your product	
	HAL_NVIC_SystemReset();	 
	
//	(void)qcloud_iot_mqtt_disconnect((Qcloud_IoT_Client *)client); 

//	while(1)
//	{
//		if(QCLOUD_ERR_SUCCESS == mqtt_reconnect(client))
//		{
//			Log_e("mqtt_reconnect success");
//			break;
//		}
//	}
	
	goto start;


	
    return;
}
