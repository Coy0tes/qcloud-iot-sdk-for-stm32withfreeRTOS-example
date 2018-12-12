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

#ifndef QCLOUD_IOT_EXPORT_ERROR_H_
#define QCLOUD_IOT_EXPORT_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * IOT SDK ������
 */
typedef enum {
    QCLOUD_ERR_MQTT_ALREADY_CONNECTED                        = 4,       // ��ʾ��MQTT�������Ѿ���������
    QCLOUD_ERR_MQTT_CONNACK_CONNECTION_ACCEPTED              = 3,       // ��ʾ���������ܿͻ���MQTT����
    QCLOUD_ERR_MQTT_MANUALLY_DISCONNECTED                    = 2,       // ��ʾ��MQTT�������Ѿ��ֶ��Ͽ�
    QCLOUD_ERR_MQTT_RECONNECTED                              = 1,       // ��ʾ��MQTT�����������ɹ�

    QCLOUD_ERR_HTTP_CLOSED                                   = -3,      // Զ�������ر�����
    QCLOUD_ERR_HTTP                                          = -4,      // HTTPδ֪����
    QCLOUD_ERR_HTTP_PRTCL                                    = -5,      // Э�����
    QCLOUD_ERR_HTTP_UNRESOLVED_DNS                           = -6,      // ��������ʧ��
    QCLOUD_ERR_HTTP_PARSE                                    = -7,      // URL����ʧ��
    QCLOUD_ERR_HTTP_CONN                                     = -8,      // HTTP����ʧ��
    QCLOUD_ERR_HTTP_AUTH                                     = -9,      // HTTP��Ȩ����
    QCLOUD_ERR_HTTP_NOT_FOUND                                = -10,     // HTTP 404
    QCLOUD_ERR_HTTP_TIMEOUT                                  = -11,     // HTTP ��ʱ

    QCLOUD_ERR_SUCCESS                                       = 0,       // ��ʾ�ɹ�����
    QCLOUD_ERR_FAILURE                                       = -1001,   // ��ʾʧ�ܷ���
    QCLOUD_ERR_INVAL                                         = -1002,   // ��ʾ������Ч����
    QCLOUD_ERR_NULL                                          = -1003,   // ��ʾ��ָ��

    QCLOUD_ERR_MQTT_PUSH_TO_LIST_FAILED                      = -102,    // ��ʾ���ȴ� ACK �б������Ԫ��ʧ��
    QCLOUD_ERR_MQTT_NO_CONN                                  = -103,    // ��ʾδ��MQTT�������������ӻ��Ѿ��Ͽ�����
    QCLOUD_ERR_MQTT_UNKNOWN                                  = -104,    // ��ʾMQTT��ص�δ֪����
    QCLOUD_ERR_MQTT_ATTEMPTING_RECONNECT                     = -105,    // ��ʾ������MQTT�������½�������
    QCLOUD_ERR_MQTT_RECONNECT_TIMEOUT                        = -106,    // ��ʾ�����Ѿ���ʱ
    QCLOUD_ERR_MQTT_MAX_SUBSCRIPTIONS                        = -107,    // ��ʾ�����ɶ��ĵ�������
    QCLOUD_ERR_MQTT_SUB                                      = -108,    // ��ʾ��������ʧ��, ���������ܾ�
    QCLOUD_ERR_MQTT_NOTHING_TO_READ                          = -109,    // ��ʾ��MQTT��ر��Ŀ��Զ�ȡ
    QCLOUD_ERR_MQTT_PACKET_READ                              = -110,    // ��ʾ��ȡ��MQTT����������
    QCLOUD_ERR_MQTT_REQUEST_TIMEOUT                          = -111,    // ��ʾMQTT��ز�������ʱ
    QCLOUD_ERR_MQTT_CONNACK_UNKNOWN                          = -112,    // ��ʾ�ͻ���MQTT����δ֪����
    QCLOUD_ERR_MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION    = -113,    // ��ʾ�ͻ���MQTT�汾����
    QCLOUD_ERR_MQTT_CONNACK_IDENTIFIER_REJECTED              = -114,    // ��ʾ�ͻ��˱�ʶ������
    QCLOUD_ERR_MQTT_CONNACK_SERVER_UNAVAILABLE               = -115,    // ��ʾ������������
    QCLOUD_ERR_MQTT_CONNACK_BAD_USERDATA                     = -116,    // ��ʾ�ͻ������Ӳ����е�username��password����
    QCLOUD_ERR_MQTT_CONNACK_NOT_AUTHORIZED                   = -117,    // ��ʾ�ͻ���������֤ʧ��
    QCLOUD_ERR_RX_MESSAGE_INVAL                              = -118,    // ��ʾ�յ�����Ϣ��Ч
    QCLOUD_ERR_BUF_TOO_SHORT                                 = -119,    // ��ʾ��Ϣ���ջ������ĳ���С����Ϣ�ĳ���

    QCLOUD_ERR_JSON_PARSE                                    = -132,    // ��ʾJSON��������
    QCLOUD_ERR_JSON_BUFFER_TRUNCATED                         = -133,    // ��ʾJSON�ĵ��ᱻ�ض�
    QCLOUD_ERR_JSON_BUFFER_TOO_SMALL                         = -134,    // ��ʾ�洢JSON�ĵ��Ļ�����̫С
    QCLOUD_ERR_JSON                                          = -135,    // ��ʾJSON�ĵ����ɴ���
    QCLOUD_ERR_MAX_JSON_TOKEN                                = -136,    // ��ʾ����JSON�ĵ��е����Token��
    QCLOUD_ERR_MAX_APPENDING_REQUEST                         = -137,    // ��ʾ����ͬʱ�����ĵ�����
    QCLOUD_ERR_MAX_TOPIC_LENGTH                              = -138,    // ��ʾ�����涨����topic����

    QCLOUD_ERR_COAP_CONNRESET                                = -150,    // COAP��������
    QCLOUD_ERR_COAP_NULL                                     = -151,    // ��ָ��
    QCLOUD_ERR_COAP_INVALID_LENGTH                           = -152,    // COAP�������ȴ���
    QCLOUD_ERR_COAP_DATA_SIZE                                = -153,    // COAP���ݴ�С��������
    QCLOUD_ERR_COAP_NOT_FOUND                                = -154,    // COAP����ʧ��
    QCLOUD_ERR_COAP_NET_INIT_FAILED                          = -155,    // COAP��ʼ��ʧ��
    QCLOUD_ERR_COAP_INTERNAL                                 = -156,    // COAP�ڲ�����
    QCLOUD_ERR_COAP_WRITE_FAILED                             = -157,    // COAPд����ʧ��
    QCLOUD_ERR_COAP_READ_FAILED                              = -158,    // COAP������ʧ��
    QCLOUD_ERR_COAP_BADMSG                                   = -159,    // COAPbad��Ϣ
    QCLOUD_ERR_COAP_TIMEOUT                                  = -160,    // COAP��ʱ����
    QCLOUD_ERR_DTLS_PEER_CLOSE_NOTIFY                        = -161,    // ��ʾDTLSͨ�����ر�
    
    QCLOUD_ERR_SHADOW_PROPERTY_EXIST                         = -201,    // ��ʾע��������Ѿ�����
    QCLOUD_ERR_SHADOW_NOT_PROPERTY_EXIST                     = -202,    // ��ʾע������Բ�����
    QCLOUD_ERR_SHADOW_UPDATE_TIMEOUT                         = -203,    // ��ʾ�����豸Ӱ���ĵ���ʱ
    QCLOUD_ERR_SHADOW_UPDATE_REJECTED                        = -204,    // ��ʾ�����豸Ӱ���ĵ����ܾ�
    QCLOUD_ERR_SHADOW_GET_TIMEOUT                            = -205,    // ��ʾ��ȡ�豸Ӱ���ĵ���ʱ
    QCLOUD_ERR_SHADOW_GET_REJECTED                           = -206,    // ��ʾ��ȡ�豸Ӱ���ĵ����ܾ�

    QCLOUD_ERR_TCP_SOCKET_FAILED                             = -601,    // ��ʾTCP���ӽ����׽���ʧ��
    QCLOUD_ERR_TCP_UNKNOWN_HOST                              = -602,    // ��ʾ�޷�ͨ����������ȡIP��ַ
    QCLOUD_ERR_TCP_CONNECT                                   = -603,    // ��ʾ����TCP����ʧ��
    QCLOUD_ERR_TCP_READ_TIMEOUT                              = -604,    // ��ʾ����TCP����ʱ
    QCLOUD_ERR_TCP_WRITE_TIMEOUT                             = -605,    // ��ʾ����TCPд��ʱ

    QCLOUD_ERR_SSL_INIT                                      = -701,    // ��ʾSSL��ʼ��ʧ��
    QCLOUD_ERR_SSL_CERT                                      = -702,    // ��ʾSSL֤���������
    QCLOUD_ERR_SSL_CONNECT                                   = -703,    // ��ʾSSL����ʧ��
    QCLOUD_ERR_SSL_CONNECT_TIMEOUT                           = -704,    // ��ʾSSL���ӳ�ʱ
    QCLOUD_ERR_SSL_WRITE_TIMEOUT                             = -705,    // ��ʾSSLд��ʱ
    QCLOUD_ERR_SSL_WRITE                                     = -706,    // ��ʾSSLд����
    QCLOUD_ERR_SSL_READ_TIMEOUT                              = -707,    // ��ʾSSL����ʱ
    QCLOUD_ERR_SSL_READ                                      = -708,    // ��ʾSSL������
    QCLOUD_ERR_SSL_NOTHING_TO_READ                           = -709,    // ��ʾ�ײ�û�����ݿ��Զ�ȡ
} IoT_Error_Code;

#ifdef __cplusplus
}
#endif

#endif /* QCLOUD_IOT_EXPORT_ERROR_H_ */
