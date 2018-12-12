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

#ifndef IOT_MQTT_CLIENT_H_
#define IOT_MQTT_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "qcloud_iot_export.h"
#include "qcloud_iot_import.h"
#include "qcloud_iot_sdk_impl_internal.h"
#include "qcloud_iot_platform.h"

#include "mqtt_client_net.h"

#include "utils_timer.h"
#include "utils_list.h"

/* ����id���ֵ */
#define MAX_PACKET_ID               								(65535)

/* ���ӱ�ʶ�ַ�����󳤶�  */
#define MAX_CONN_ID_LEN												(6)

/* �ɹ���������������� */
#define MAX_MESSAGE_HANDLERS        								(5)

/* ���б����������·������� */
#define MAX_REPUB_NUM                                               (20)

/* ������С�ȴ�ʱ�� */
#define MIN_RECONNECT_WAIT_INTERVAL 								(1000)

/* MQTT������С��ʱʱ�� */
#define MIN_COMMAND_TIMEOUT         								(500)

/* MQTT�������ʱʱ�� */
#define MAX_COMMAND_TIMEOUT         								(50000)

/* �ƶ˱����������󳤶� */
#define MAX_SIZE_OF_CLOUD_TOPIC                                     ((MAX_SIZE_OF_DEVICE_NAME) + (MAX_SIZE_OF_PRODUCT_ID) + 64)


/**
 * @brief MQTT Message Type
 */
typedef enum msgTypes {
	RESERVED    = 0,     // Reserved
	CONNECT     = 1,     // Client request to connect to Server
	CONNACK     = 2,     // Connect Acknowledgment
	PUBLISH     = 3,     // Publish message
	PUBACK      = 4,     // Publish Acknowledgment
	PUBREC      = 5,     // Publish Received
	PUBREL      = 6,     // Publish Release
	PUBCOMP     = 7,     // Publish Complete
	SUBSCRIBE   = 8,     // Client Subscribe request
	SUBACK      = 9,     // Subscribe Acknowledgment
	UNSUBSCRIBE = 10,    // Client Unsubscribe request
	UNSUBACK    = 11,    // Unsubscribe Acknowledgment
	PINGREQ     = 12,    // PING Request
	PINGRESP    = 13,    // PING Response
	DISCONNECT  = 14     // Client is Disconnecting
} MessageTypes;

typedef enum {
	NOTCONNECTED = 0,
	CONNECTED	 = 1
} ConnStatus;


/**
 * Bitfields for the MQTT header byte.
 */
typedef union {
	unsigned char byte;	                /**< the whole byte */
#if defined(REVERSED)
	struct {
		unsigned int type : 4;			/**< message type nibble */
		unsigned int dup : 1;				/**< DUP flag bit */
		unsigned int qos : 2;				/**< QoS value, 0, 1 or 2 */
		unsigned int retain : 1;		/**< retained flag bit */
	} bits;
#else
	struct {
		unsigned int retain : 1;		/**< retained flag bit */
		unsigned int qos : 2;				/**< QoS value, 0, 1 or 2 */
		unsigned int dup : 1;				/**< DUP flag bit */
		unsigned int type : 4;			/**< message type nibble */
	} bits;
#endif
} MQTTHeader;

/**
 * @brief MQTT ������Ϣ�����ṹ�嶨��
 *
 * ���ͻ����쳣�Ͽ�����, ���ͻ���������ʱ������������Ϣ, ����˻ᷢ��������Ϣ��
 */
typedef struct {
    char        struct_id[4];      // The eyecatcher for this structure. must be MQTW
    uint8_t     struct_version;    // �ṹ�� 0
    char        *topic_name;        // ������Ϣ������
    char        *message;          // ������Ϣ���ز�������
    uint8_t     retained;          // ������Ϣretain��־λ
    QoS         qos;               // ������Ϣqos��־λ
} WillOptions;

/**
 * MQTT������Ϣ�ṹ��Ĭ��ֵ����
 */
#define DEFAULT_WILL_OPTIONS { {'M', 'Q', 'T', 'W'}, 0, NULL, NULL, 0, QOS0 }

/**
 * @brief MQTT ���Ӳ����ṹ�嶨��
 *
 */
typedef struct {
    char            			*client_id;             // �ͻ��˱�ʶ��, �뱣��Ψһ
    char            			*username;              // �û���
    char						*password;				// ����

    char					 	conn_id[MAX_CONN_ID_LEN];

    char            			struct_id[4];           // The eyecatcher for this structure.  must be MQTC.
    uint8_t         			struct_version;         // �ṹ��汾��, ����Ϊ0
    uint8_t         			mqtt_version;            // MQTT�汾Э��� 4 = 3.1.1

    uint16_t        			keep_alive_interval;    // ��������, ��λ: s
    uint8_t         			clean_session;          // ����Ự��־λ, ���庬����ο�MQTTЭ��˵���ĵ�3.1.2.4С��

    uint8_t                   	auto_connect_enable;    // �Ƿ����Զ�����

#ifdef AUTH_WITH_NOTLS
    char						*device_secret;					// Ԥ����Կ
#endif

} MQTTConnectParams;

/**
 * MQTT���Ӳ����ṹ��Ĭ��ֵ����
 */
#ifdef AUTH_WITH_NOTLS
#define DEFAULT_MQTTCONNECT_PARAMS { NULL, NULL, NULL, {0}, {'M', 'Q', 'T', 'C'}, 0, 4, 240, 1, 1, NULL}
#else
#define DEFAULT_MQTTCONNECT_PARAMS { NULL, NULL, NULL, {0}, {'M', 'Q', 'T', 'C'}, 0, 4, 240, 1, 1}
#endif

/**
 * @brief ���������Ӧ����Ϣ����ṹ�嶨��
 */
typedef struct SubTopicHandle {
    const char              *topic_filter;               // ����������, �ɰ���ͨ���
    OnMessageHandler        message_handler;             // ����������Ϣ�ص�����ָ��
    void                    *message_handler_data;       // �û�����, ͨ���ص���������
    QoS                     qos;                         // ���������ȼ�
} SubTopicHandle;

/**
 * @brief IoT Client�ṹ�嶨��
 */
typedef struct Client {
    uint8_t                  is_connected;                                  // �����Ƿ�����
    uint8_t                  was_manually_disconnected;                     // �Ƿ��ֶ��Ͽ�����
    uint8_t                  is_ping_outstanding;                           // �������Ƿ�δ���, ��δ�յ���������Ӧ

    uint16_t                 next_packet_id;                                // MQTT���ı�ʶ��
    uint32_t                 command_timeout_ms;                            // MQTT��Ϣ��ʱʱ��, ��λ:ms

    uint32_t                 current_reconnect_wait_interval;               // MQTT��������, ��λ:ms
    uint32_t                 counter_network_disconnected;                  // ����Ͽ����Ӵ���

    size_t                   write_buf_size;                                // ��Ϣ����buffer����
    size_t                   read_buf_size;                                 // ��Ϣ����buffer����
    unsigned char            write_buf[QCLOUD_IOT_MQTT_TX_BUF_LEN];         // MQTT��Ϣ����buffer
    unsigned char            read_buf[QCLOUD_IOT_MQTT_RX_BUF_LEN];          // MQTT��Ϣ����buffer

    void                     *lock_generic;                                 // clientԭ����
    void                     *lock_write_buf;                          		// ���������

    void                     *lock_list_pub;                                // �ȴ�������Ϣack�б����
    void                     *lock_list_sub;                                // �ȴ�������Ϣack�б����

    List                     *list_pub_wait_ack;                            // �ȴ�������Ϣack�б�
    List                     *list_sub_wait_ack;                            // �ȴ�������Ϣack�б�

    MQTTEventHandler         event_handle;                                  // �¼����

    MQTTConnectParams        options;                                       // ������ز���

    Network                  network_stack;                                 // MQTT�ײ�ʹ�õ��������

    Timer                    ping_timer;                                    // MQTT���������Ͷ�ʱ��
    Timer                    reconnect_delay_timer;                         // MQTT������ʱ��, �ж��Ƿ��ѵ�����ʱ��

    SubTopicHandle           sub_handles[MAX_MESSAGE_HANDLERS];             // ���������Ӧ����Ϣ����ṹ����

} Qcloud_IoT_Client;

/**
 * @brief MQTTЭ��汾
 */
typedef enum {
    MQTT_3_1_1 = 4
} MQTT_VERSION;


typedef enum MQTT_NODE_STATE {
    MQTT_NODE_STATE_NORMANL = 0,
    MQTT_NODE_STATE_INVALID,
} MQTTNodeState;

/* ��¼�Ѿ�������topic����Ϣ */
typedef struct REPUBLISH_INFO {
    Timer                   pub_start_time;     /* ������ʱ�� */
    MQTTNodeState           node_state;         /* �ڵ�״̬ */
    uint16_t                msg_id;             /* ������Ϣ��packet id */
    uint32_t                len;                /* ��Ϣ���� */
    unsigned char          *buf;                /* ��Ϣ���� */
} QcloudIotPubInfo;

/* ��¼�Ѿ����ĵ�topic����Ϣ */
typedef struct SUBSCRIBE_INFO {
    enum msgTypes           type;           /* ����, (sub or unsub) */
    uint16_t                msg_id;         /* ����/ȡ�����ĵ� packet id */
    Timer                   sub_start_time; /* ���ĵĿ�ʼʱ�� */
    MQTTNodeState           node_state;     /* �ڵ�״̬ */
    SubTopicHandle          handler;        /* handle of topic subscribed(unsubcribed) */
    uint16_t                len;            /* ��Ϣ���� */
    unsigned char          *buf;            /* ��Ϣ���� */
} QcloudIotSubInfo;

/**
 * @brief �Խṹ��Client���г�ʼ��
 *
 * @param pClient MQTT�ͻ��˽ṹ��
 * @param pParams MQTT�ͻ��˳�ʼ������
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int qcloud_iot_mqtt_init(Qcloud_IoT_Client *pClient, MQTTInitParams *pParams);

/**
 * @brief ��������TLS��MQTT����
 *
 * ע��: Client ID����ΪNULL����ַ���
 *
 * @param pClient MQTT�ͻ��˽ṹ��
 * @param pParams MQTT������ز���, �ɲο�MQTTЭ��˵��
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int qcloud_iot_mqtt_connect(Qcloud_IoT_Client *pClient, MQTTConnectParams *pParams);

/**
 * @brief ����������½���MQTT����
 *
 * 1. ����������½���MQTT����
 * 2. ���ӳɹ���, ���¶���֮ǰ�Ķ��Ĺ�������
 *
 * @param pClient MQTT Client�ṹ��
 *
 * @return ����QCLOUD_ERR_MQTT_RECONNECTED, ��ʾ�����ɹ�
 */
int qcloud_iot_mqtt_attempt_reconnect(Qcloud_IoT_Client *pClient);

/**
 * @brief �Ͽ�MQTT����
 *
 * @param pClient MQTT Client�ṹ��
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int qcloud_iot_mqtt_disconnect(Qcloud_IoT_Client *pClient);

/**
 * @brief ����MQTT��Ϣ
 *
 * @param pClient MQTT�ͻ��˽ṹ��
 * @param topicName ������
 * @param pParams ��������
 * @return < 0  :   ��ʾʧ��
 *         >= 0 :   ����Ψһ��packet id 
 */
int qcloud_iot_mqtt_publish(Qcloud_IoT_Client *pClient, char *topicName, PublishParams *pParams);

/**
 * @brief ����MQTT����
 *
 * @param pClient MQTT�ͻ��˽ṹ��
 * @param topicFilter ���������, �ɲο�MQTTЭ��˵�� 4.7
 * @param pParams ���Ĳ���
 * @return < 0  :   ��ʾʧ��
 *         >= 0 :   ����Ψһ��packet id 
 */
int qcloud_iot_mqtt_subscribe(Qcloud_IoT_Client *pClient, char *topicFilter, SubscribeParams *pParams);

/**
 * @brief ���¶��ĶϿ�����֮ǰ�Ѷ��ĵ�����
 *
 * @param pClient MQTT�ͻ��˽ṹ��
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int qcloud_iot_mqtt_resubscribe(Qcloud_IoT_Client *pClient);

/**
 * @brief ȡ�������Ѷ��ĵ�MQTT����
 *
 * @param pClient MQTT�ͻ��˽ṹ��
 * @param topicFilter  ���������, �ɲο�MQTTЭ��˵�� 4.7
 * @return < 0  :   ��ʾʧ��
 *         >= 0 :   ����Ψһ��packet id   
 */
int qcloud_iot_mqtt_unsubscribe(Qcloud_IoT_Client *pClient, char *topicFilter);

/**
 * @brief �ڵ�ǰ�߳�Ϊ�ײ�MQTT�ͻ����ó�һ��CPUִ��ʱ��
 *
 * �����ʱ����, MQTT�ͻ��˻����ô�����Ϣ����, �Լ�����PING����, �������״̬
 *
 * @param pClient    MQTT Client�ṹ��
 * @param timeout_ms Yield������ʱʱ��
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�, ����QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT, ��ʾ��������
 */
int qcloud_iot_mqtt_yield(Qcloud_IoT_Client *pClient, uint32_t timeout_ms);

/**
 * @brief �ͻ����Զ������Ƿ���
 *
 * @param pClient MQTT�ͻ��˽ṹ��
 * @return ����true, ��ʾ�ͻ����ѿ����Զ���������
 */
bool qcloud_iot_mqtt_is_autoreconnect_enabled(Qcloud_IoT_Client *pClient);

/**
 * @brief ���ÿͻ����Ƿ����Զ�����
 *
 * @param pClient MQTT�ͻ��˽ṹ��
 * @param value �Ƿ����ù���
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ���óɹ�
 */
int qcloud_iot_mqtt_set_autoreconnect(Qcloud_IoT_Client *pClient, bool value);

/**
 * @brief ��ȡ����Ͽ��Ĵ���
 *
 * @param pClient MQTT Client�ṹ��
 * @return ���ؿͻ���MQTT����Ͽ��Ĵ���
 */
int qcloud_iot_mqtt_get_network_disconnected_count(Qcloud_IoT_Client *pClient);

/**
 * @brief �������ӶϿ�����
 *
 * @param pClient MQTT Client�ṹ��
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ���óɹ�
 */
int qcloud_iot_mqtt_reset_network_disconnected_count(Qcloud_IoT_Client *pClient);

/**
 * @brief ��ȡ���ı�ʶ��
 *
 * @param pClient
 * @return
 */
uint16_t get_next_packet_id(Qcloud_IoT_Client *pClient);

/**
 * @brief ��ȡ����Ψһ��ʶconnid
 *
 * @param options
 * @return
 */
void get_next_conn_id(MQTTConnectParams *options);

/**
 *
 * @param header
 * @param message_type
 * @param qos
 * @param dup
 * @param retained
 * @return
 */
int mqtt_init_packet_header(MQTTHeader *header, MessageTypes message_type, QoS qos, uint8_t dup, uint8_t retained);

/**
 * @brief ���շ������Ϣ
 *
 * @param pClient
 * @param timer
 * @param packet_type
 * @param qos
 * @return
 */
int cycle_for_read(Qcloud_IoT_Client *pClient, Timer *timer, uint8_t *packet_type, QoS qos);

/**
 * @brief ���ͱ�������
 *
 * @param pClient       Client�ṹ��
 * @param length        ���ĳ���
 * @param timer         ��ʱ��
 * @return
 */
int send_mqtt_packet(Qcloud_IoT_Client *pClient, size_t length, Timer *timer);

/**
 * @brief �ȴ�ָ�����͵�MQTT���Ʊ���
 *
 * only used in single-threaded mode where one command at a time is in process
 *
 * @param pClient       MQTT Client�ṹ��
 * @param packet_type   ���Ʊ�������
 * @param timer         ��ʱ��
 * @return
 */
int wait_for_read(Qcloud_IoT_Client *pClient, uint8_t packet_type, Timer *timer, QoS qos);

/**
 * @brief ����MQTT��ǰ����״̬
 *
 * @param pClient 		Client�ṹ��
 * @param connected 	0: ���ӶϿ�״̬  1: ������״̬
 * @return
 */
void set_client_conn_state(Qcloud_IoT_Client *pClient, uint8_t connected);

/**
 * @brief ��ȡMQTT��ǰ����״̬
 *
 * @param pClient 		Client�ṹ��
 * @return				0: ���ӶϿ�״̬  1: ������״̬
 */
uint8_t get_client_conn_state(Qcloud_IoT_Client *pClient);

/**
 * @brief ��� Publish ACK �ȴ��б����гɹ����ջ��߳�ʱ���򽫶�Ӧ�ڵ���б����Ƴ�
 *
 * @param pClient MQTT�ͻ���
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int qcloud_iot_mqtt_pub_info_proc(Qcloud_IoT_Client *pClient);

/**
 * @brief ��� Subscribe ACK �ȴ��б����гɹ����ջ��߳�ʱ���򽫶�Ӧ�ڵ���б����Ƴ�
 *
 * @param pClient MQTT�ͻ���
 * @return ����QCLOUD_ERR_SUCCESS, ��ʾ�ɹ�
 */
int qcloud_iot_mqtt_sub_info_proc(Qcloud_IoT_Client *pClient);

int push_sub_info_to(Qcloud_IoT_Client *c, int len, unsigned short msgId, MessageTypes type,
								   SubTopicHandle *handler, ListNode **node);

int serialize_pub_ack_packet(unsigned char *buf, size_t buf_len, MessageTypes packet_type, uint8_t dup,
							 uint16_t packet_id,
							 uint32_t *serialized_len);

int serialize_packet_with_zero_payload(unsigned char *buf, size_t buf_len, MessageTypes packetType, uint32_t *serialized_len);

int deserialize_publish_packet(unsigned char *dup, QoS *qos, uint8_t *retained, uint16_t *packet_id, char **topicName,
							   uint16_t *topicNameLen, unsigned char **payload, size_t *payload_len, unsigned char *buf, size_t buf_len);

int deserialize_suback_packet(uint16_t *packet_id, uint32_t max_count, uint32_t *count,
                                     QoS *grantedQoSs, unsigned char *buf, size_t buf_len);

int deserialize_unsuback_packet(uint16_t *packet_id, unsigned char *buf, size_t buf_len);

int deserialize_ack_packet(uint8_t *packet_type, uint8_t *dup, uint16_t *packet_id, unsigned char *buf, size_t buf_len);

#ifdef MQTT_RMDUP_MSG_ENABLED

void reset_repeat_packet_id_buffer(void);

#endif
/**
 * @brief ����ʣ�೤�ȼ�������MQTT���ĵĳ���
 *
 * @param rem_len    ʣ�೤��
 * @return           ����MQTT���ĵĳ���
 */
size_t get_mqtt_packet_len(size_t rem_len);

size_t mqtt_write_packet_rem_len(unsigned char *buf, uint32_t length);

int mqtt_read_packet_rem_len_form_buf(unsigned char *buf, uint32_t *value, uint32_t *readBytesLen);

uint16_t mqtt_read_uint16_t(unsigned char **pptr);

unsigned char mqtt_read_char(unsigned char **pptr);

void mqtt_write_char(unsigned char **pptr, unsigned char c);

void mqtt_write_uint_16(unsigned char **pptr, uint16_t anInt);

void mqtt_write_utf8_string(unsigned char **pptr, const char *string);

#ifdef __cplusplus
}
#endif

#endif //IOT_MQTT_CLIENT_H_
