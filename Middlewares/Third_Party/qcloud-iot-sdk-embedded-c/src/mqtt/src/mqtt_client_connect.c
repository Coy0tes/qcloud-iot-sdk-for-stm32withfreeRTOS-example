/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander/Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <limits.h>

#include "mqtt_client.h"
#include "utils_hmac.h"

typedef union {
    uint8_t all;    /**< all connect flags */
#if defined(REVERSED)
    struct
    {
        unsigned int username : 1;			/**< 3.1 user name */
        unsigned int password : 1; 			/**< 3.1 password */
        unsigned int willRetain : 1;		/**< will retain setting */
        unsigned int willQoS : 2;				/**< will QoS value */
        unsigned int will : 1;			    /**< will flag */
        unsigned int cleansession : 1;	  /**< clean session flag */
        unsigned int : 1;	  	          /**< unused */
    } bits;
#else
    struct {
        unsigned int : 1;                            /**< unused */
        unsigned int cleansession : 1;      /**< cleansession flag */
        unsigned int will : 1;                /**< will flag */
        unsigned int willQoS : 2;                /**< will QoS value */
        unsigned int willRetain : 1;        /**< will retain setting */
        unsigned int password : 1;            /**< 3.1 password */
        unsigned int username : 1;            /**< 3.1 user name */
    } bits;
#endif
} MQTTConnectFlags;    /**< connect flags byte */

typedef union {
    uint8_t all;    /**< all connack flags */
#if defined(REVERSED)
    struct
    {
        unsigned int sessionpresent : 1;    /**< session present flag */
        unsigned int : 7;	  	          /**< unused */
    } bits;
#else
    struct {
        unsigned int : 7;                    /**< unused */
        unsigned int sessionpresent : 1;    /**< session present flag */
    } bits;
#endif
} MQTTConnackFlags;    /**< connack flags byte */

/**
 * Connect return code
 */
typedef enum {
    CONNACK_CONNECTION_ACCEPTED = 0,    // connection accepted
    CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = 1,    // connection refused: unaccpeted protocol verison
    CONNACK_IDENTIFIER_REJECTED_ERROR = 2,    // connection refused: identifier rejected
    CONNACK_SERVER_UNAVAILABLE_ERROR = 3,    // connection refused: server unavailable
    CONNACK_BAD_USERDATA_ERROR = 4,    // connection refused: bad user name or password
    CONNACK_NOT_AUTHORIZED_ERROR = 5     // connection refused: not authorized
} MQTTConnackReturnCodes;

/**
  * Determines the length of the MQTT connect packet that would be produced using the supplied connect options.
  * @param options the options to be used to build the connect packet
  * @param the length of buffer needed to contain the serialized version of the packet
  * @return int indicating function execution status
  */
static uint32_t _get_packet_connect_rem_len(MQTTConnectParams *options) {
    size_t len = 0;
    /* variable depending on MQTT or MQIsdp */
    if (3 == options->mqtt_version) {
        len = 12;
    } else if (4 == options->mqtt_version) {
        len = 10;
    }

    len += strlen(options->client_id) + 2;

    if (options->username) {
        len += strlen(options->username) + 2;
    }

    if (options->password) {
        len += strlen(options->password) + 2;
    }

    return (uint32_t) len;
}

static void _copy_connect_params(MQTTConnectParams *destination, MQTTConnectParams *source) {

	POINTER_SANITY_CHECK_RTN(destination);
	POINTER_SANITY_CHECK_RTN(source);

    destination->mqtt_version = source->mqtt_version;
    destination->client_id = source->client_id;
    destination->username = source->username;
    destination->keep_alive_interval = source->keep_alive_interval;
    destination->clean_session = source->clean_session;
    destination->auto_connect_enable = source->auto_connect_enable;
#ifdef AUTH_WITH_NOTLS
    destination->device_secret = source->device_secret;
#endif
}

/**
  * Serializes the connect options into the buffer.
  * @param buf the buffer into which the packet will be serialized
  * @param len the length in bytes of the supplied buffer
  * @param options the options to be used to build the connect packet
  * @param serialized length
  * @return int indicating function execution status
  */
static int _serialize_connect_packet(unsigned char *buf, size_t buf_len, MQTTConnectParams *options, uint32_t *serialized_len) {
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(options, QCLOUD_ERR_INVAL);
    STRING_PTR_SANITY_CHECK(options->client_id, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(serialized_len, QCLOUD_ERR_INVAL);

    unsigned char *ptr = buf;
    MQTTHeader header = {0};
    MQTTConnectFlags flags = {0};
    uint32_t rem_len = 0;
    int rc;

    long cur_timesec = HAL_Timer_current_sec() + MAX_ACCESS_EXPIRE_TIMEOUT / 1000;
    if (cur_timesec <= 0 || MAX_ACCESS_EXPIRE_TIMEOUT <= 0) {
    	cur_timesec = LONG_MAX;
    }
    long cur_timesec_bak = cur_timesec;
    int cur_timesec_len = 0;
    while(cur_timesec_bak != 0) {
    	cur_timesec_bak /= 10;
		++cur_timesec_len;
	}

    int username_len = strlen(options->client_id) + strlen(QCLOUD_IOT_DEVICE_SDK_APPID) + MAX_CONN_ID_LEN + cur_timesec_len + 4;
    options->username = (char*)HAL_Malloc(username_len);
    get_next_conn_id(options);
	HAL_Snprintf(options->username, username_len, "%s;%s;%s;%ld", options->client_id, QCLOUD_IOT_DEVICE_SDK_APPID, options->conn_id, cur_timesec);

#if defined(AUTH_WITH_NOTLS) && defined(AUTH_MODE_KEY)
     if (options->device_secret != NULL && options->username != NULL) {
    	 char                sign[41]   = {0};
    	 //utils_hmac_sha1(options->username, strlen(options->username), sign, options->device_secret, strlen(options->device_secret));
    	 utils_hmac_sha1(options->username, strlen(options->username), sign, options->device_secret, 16);
    	 options->password = (char*) HAL_Malloc (51);
    	 if (options->password == NULL) IOT_FUNC_EXIT_RC(QCLOUD_ERR_INVAL);
		 HAL_Snprintf(options->password, 51, "%s;hmacsha1", sign);
     }
#endif

    rem_len = _get_packet_connect_rem_len(options);
    if (get_mqtt_packet_len(rem_len) > buf_len) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    rc = mqtt_init_packet_header(&header, CONNECT, QOS0, 0, 0);
    if (QCLOUD_ERR_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    // ���Ĺ̶�ͷ����һ���ֽ�
    mqtt_write_char(&ptr, header.byte);

    // ���Ĺ̶�ͷ��ʣ�೤���ֶ�
    ptr += mqtt_write_packet_rem_len(ptr, rem_len);

    // ���Ŀɱ�ͷ��Э���� + Э��汾��
    if (4 == options->mqtt_version) {
        mqtt_write_utf8_string(&ptr, "MQTT");
        mqtt_write_char(&ptr, (unsigned char) 4);
    } else {
        mqtt_write_utf8_string(&ptr, "MQIsdp");
        mqtt_write_char(&ptr, (unsigned char) 3);
    }

    // ���Ŀɱ�ͷ�����ӱ�ʶλ
    flags.all = 0;
    flags.bits.cleansession = (options->clean_session) ? 1 : 0;

    if (options->username != NULL) {
        flags.bits.username = 1;
    } else {
        flags.bits.username = 0;
    }

#if defined(AUTH_WITH_NOTLS) && defined(AUTH_MODE_KEY)
	flags.bits.password = 1;
#else
    flags.bits.password = 0;
#endif
    
    mqtt_write_char(&ptr, flags.all);

    // ���Ŀɱ�ͷ����������/��������, һ������Ϊ��λ��ʱ����, ��ʾΪһ��16λ����
    mqtt_write_uint_16(&ptr, options->keep_alive_interval);

    // ��Ч���ز���: �ͻ��˱�ʶ��
    mqtt_write_utf8_string(&ptr, options->client_id);

    // �û���
    if (flags.bits.username && options->username != NULL) {
        mqtt_write_utf8_string(&ptr, options->username);
        HAL_Free(options->username);
    }

    if (flags.bits.password && options->password != NULL) {
    	mqtt_write_utf8_string(&ptr, options->password);
    	HAL_Free(options->password);
    }

    *serialized_len = (uint32_t) (ptr - buf);

    IOT_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
  * Deserializes the supplied (wire) buffer into connack data - return code
  * @param sessionPresent the session present flag returned (only for MQTT 3.1.1)
  * @param connack_rc returned integer value of the connack return code
  * @param buf the raw buffer data, of the correct length determined by the remaining length field
  * @param buflen the length in bytes of the data in the supplied buffer
  * @return int indicating function execution status
  */
static int _deserialize_connack_packet(uint8_t *sessionPresent, int *connack_rc, unsigned char *buf, size_t buflen) {
    IOT_FUNC_ENTRY;

    POINTER_SANITY_CHECK(sessionPresent, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(connack_rc, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(buf, QCLOUD_ERR_INVAL);

    MQTTHeader header = {0};
    unsigned char *curdata = buf;
    unsigned char *enddata = NULL;
    int rc;
    uint32_t decodedLen = 0, readBytesLen = 0;
    MQTTConnackFlags flags = {0};
    unsigned char connack_rc_char;

    // CONNACK ͷ����С�ǹ̶���2�ֽڳ���, �ɱ�ͷ��Ҳ�������ֽڵĳ���, ����Ч����
    if (4 > buflen) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_BUF_TOO_SHORT);
    }

    // ��ȡ�̶�ͷ����һ���ֽ�
    header.byte = mqtt_read_char(&curdata);
    if (CONNACK != header.bits.type) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    // ��ȡ�̶�ͷ��ʣ�೤���ֶ�
    rc = mqtt_read_packet_rem_len_form_buf(curdata, &decodedLen, &readBytesLen);
    if (QCLOUD_ERR_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }
    curdata += (readBytesLen);
    enddata = curdata + decodedLen;
    if (enddata - curdata != 2) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_FAILURE);
    }

    // ��ȡ�ɱ�ͷ��-����ȷ�ϱ�־ �ο�MQTTЭ��˵���ĵ�3.2.2.1С��
    flags.all = mqtt_read_char(&curdata);
    *sessionPresent = flags.bits.sessionpresent;

    // ��ȡ�ɱ�ͷ��-���ӷ����� �ο�MQTTЭ��˵���ĵ�3.2.2.3С��
    connack_rc_char = mqtt_read_char(&curdata);
    switch (connack_rc_char) {
        case CONNACK_CONNECTION_ACCEPTED:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_CONNECTION_ACCEPTED;
            break;
        case CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION;
            break;
        case CONNACK_IDENTIFIER_REJECTED_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_IDENTIFIER_REJECTED;
            break;
        case CONNACK_SERVER_UNAVAILABLE_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_SERVER_UNAVAILABLE;
            break;
        case CONNACK_BAD_USERDATA_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_BAD_USERDATA;
            break;
        case CONNACK_NOT_AUTHORIZED_ERROR:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_NOT_AUTHORIZED;
            break;
        default:
            *connack_rc = QCLOUD_ERR_MQTT_CONNACK_UNKNOWN;
            break;
    }

    IOT_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

/**
 * @brief �����������MQTT����
 *
 * @param pClient
 * @param options
 * @return
 */
static int _mqtt_connect(Qcloud_IoT_Client *pClient, MQTTConnectParams *options) {

    IOT_FUNC_ENTRY;

    Timer connect_timer;
    int connack_rc = QCLOUD_ERR_FAILURE, rc = QCLOUD_ERR_FAILURE;
    uint8_t sessionPresent = 0;
    uint32_t len = 0;

    InitTimer(&connect_timer);
    countdown_ms(&connect_timer, pClient->command_timeout_ms);

    if (NULL != options) {
        _copy_connect_params(&(pClient->options), options);
    }

	Log_d("host:%s port:%d",pClient->network_stack.host, pClient->network_stack.port);	

    // ����TLS����
    rc = pClient->network_stack.connect(&(pClient->network_stack));
	//rc = utils_net_connect(&(pClient->network_stack));
    if (QCLOUD_ERR_SUCCESS != rc) 
	{
		Log_e("utils_net_connect fail");			
        IOT_FUNC_EXIT_RC(rc);
    }
	
    HAL_MutexLock(pClient->lock_write_buf);
	Log_d("utils_net_connect success");	
    // ���л�CONNECT����
    rc = _serialize_connect_packet(pClient->write_buf, pClient->write_buf_size, &(pClient->options), &len);
    if (QCLOUD_ERR_SUCCESS != rc || 0 == len) {
		Log_e("_serialize_connect_packet fail");	
    	HAL_MutexUnlock(pClient->lock_write_buf);
        IOT_FUNC_EXIT_RC(rc);
    }

	Log_d("_serialize_connect_packet success");	
    // ����CONNECT����
    rc = send_mqtt_packet(pClient, len, &connect_timer);
    if (QCLOUD_ERR_SUCCESS != rc) {
    	HAL_MutexUnlock(pClient->lock_write_buf);
        IOT_FUNC_EXIT_RC(rc);
    }
    HAL_MutexUnlock(pClient->lock_write_buf);

    // �����ȴ�CONNACK�ı���,
    rc = wait_for_read(pClient, CONNACK, &connect_timer, 0);
    if (QCLOUD_ERR_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    // �����л�CONNACK��, ��鷵����
    rc = _deserialize_connack_packet(&sessionPresent, &connack_rc, pClient->read_buf, pClient->read_buf_size);
    if (QCLOUD_ERR_SUCCESS != rc) {
        IOT_FUNC_EXIT_RC(rc);
    }

    if (QCLOUD_ERR_MQTT_CONNACK_CONNECTION_ACCEPTED != connack_rc) {
        IOT_FUNC_EXIT_RC(connack_rc);
    }

    set_client_conn_state(pClient, CONNECTED);
    HAL_MutexLock(pClient->lock_generic);
    pClient->was_manually_disconnected = 0;
    pClient->is_ping_outstanding = 0;
    countdown(&pClient->ping_timer, pClient->options.keep_alive_interval);
    HAL_MutexUnlock(pClient->lock_generic);

    IOT_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

int qcloud_iot_mqtt_connect(Qcloud_IoT_Client *pClient, MQTTConnectParams *pParams) {

    IOT_FUNC_ENTRY;
    int rc;
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(pParams, QCLOUD_ERR_INVAL);

    // ���MQTT�����Ѿ�����, ��Ҫ�ظ�����CONNECT����
    if (get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_ALREADY_CONNECTED);
    }

    rc = _mqtt_connect(pClient, pParams);

    // ���MQTT���ӽ���ʧ��, ��Ͽ��ײ��TLS����
    if (rc != QCLOUD_ERR_SUCCESS) {
        pClient->network_stack.disconnect(&(pClient->network_stack));
    }

    IOT_FUNC_EXIT_RC(rc);
}

int qcloud_iot_mqtt_attempt_reconnect(Qcloud_IoT_Client *pClient) {

    IOT_FUNC_ENTRY;

    int rc;
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    Log_i("attempt to reconnect %s = %p", pClient, pClient);

    if (get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_ALREADY_CONNECTED);
    }

    rc = qcloud_iot_mqtt_connect(pClient, &pClient->options);

    if (!get_client_conn_state(pClient)) {
        IOT_FUNC_EXIT_RC(rc);
    }

    rc = qcloud_iot_mqtt_resubscribe(pClient);
    if (rc != QCLOUD_ERR_SUCCESS) {
        IOT_FUNC_EXIT_RC(rc);
    }

    IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_RECONNECTED);
}

int qcloud_iot_mqtt_disconnect(Qcloud_IoT_Client *pClient) {

    IOT_FUNC_ENTRY;

    int rc;
    POINTER_SANITY_CHECK(pClient, QCLOUD_ERR_INVAL);

    Timer timer;
    uint32_t serialized_len = 0;

    if (get_client_conn_state(pClient) == 0) {
        IOT_FUNC_EXIT_RC(QCLOUD_ERR_MQTT_NO_CONN);
    }

    // 1. ��disconnect��
    rc = serialize_packet_with_zero_payload(pClient->write_buf, pClient->write_buf_size, DISCONNECT, &serialized_len);
    if (rc != QCLOUD_ERR_SUCCESS) {
        IOT_FUNC_EXIT_RC(rc);
    }

    InitTimer(&timer);
    countdown_ms(&timer, pClient->command_timeout_ms);

    // 2. ����disconnect��
    if (serialized_len > 0) {
        rc = send_mqtt_packet(pClient, serialized_len, &timer);
        if (rc != QCLOUD_ERR_SUCCESS) {
            IOT_FUNC_EXIT_RC(rc);
        }
    }

    // 3. �Ͽ��ײ�TCP����, ���޸���ر�ʶλ
    pClient->network_stack.disconnect(&(pClient->network_stack));
    set_client_conn_state(pClient, NOTCONNECTED);
    pClient->was_manually_disconnected = 1;

    Log_i("mqtt disconnect!");

    IOT_FUNC_EXIT_RC(QCLOUD_ERR_SUCCESS);
}

#ifdef __cplusplus
}
#endif


