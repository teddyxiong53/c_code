#include "libemqtt.h"

#define MQTT_DUP_FLAG 1<<3
#define MQTT_QOS0_FLAG 0<<1
#define MQTT_QOS1_FLAG 1<<1
#define MQTT_QOS2_FLAG 2<<1

#define MQTT_USERNAME_FLAG 1<<7
#define MQTT_PASSWORD_FLAG 1<<6
#define MQTT_WILL_RETAIN 1<<5
#define MQTT_WILL_FLAG 1<<2
#define MQTT_CLEAN_SESSION 1<<1
#define MQTT_RETAIN_FLAG 1<<0

void mqtt_init(mqtt_broker_handle_t *broker, const char *clientid)
{
    broker->alive = 300;
    broker->seq = 1;
    memset(broker->clientid, 0, sizeof(broker->clientid));
    memset(broker->username, 0, sizeof(broker->username));
    memset(broker->password, 0, sizeof(broker->password));
    if(clientid) {
        strncpy(broker->clientid, clientid, sizeof(broker->clientid));
    } else {
        strcpy(broker->clientid, "emqtt");
    }
    broker->clean_session = 1;
}


void mqtt_init_auth(mqtt_broker_handle_t *broker, const char *username, const char *password)
{
    if(username && username[0] != '\0') {
        strncpy(broker->username, username, sizeof(broker->username)-1);
    }
    if(password && password[0] != '\0') {
        strncpy(broker->password, password, sizeof(broker->username)-1);
    }
}

void mqtt_set_alive(mqtt_broker_handle_t *broker, int alive)
{
    broker->alive = alive;
}

int mqtt_connect(mqtt_broker_handle_t *broker)
{
    uint8_t flags = 0x00;
    uint16_t clientidlen = strlen(broker->clientid);
    uint16_t usernamelen = strlen(broker->username);
    uint16_t passwordlen = strlen(broker->password);
    uint16_t payloadlen = clientidlen + 2;
    if(usernamelen) {
        payloadlen += usernamelen+2;
        flags |= MQTT_USERNAME_FLAG;
    }
    if(passwordlen) {
        payloadlen += passwordlen+2;
        flags |= MQTT_PASSWORD_FLAG;
    }
    if(broker->clean_session) {
        flags |= MQTT_CLEAN_SESSION;
    }
    uint8_t var_header[] = {
        0x00,0x06,0x4d,0x51,0x49,0x73,0x64,0x70,//MQIdsp
        //0x00,0x04,0x4d,0x51,0x54,0x54,//MQTT
        0x03,//protocol version
        flags,//
        broker->alive>>8, broker->alive&0xff
    };
    uint8_t fixedHeaderSize = 2;
    uint8_t remainLen = sizeof(var_header) + payloadlen;
    if(remainLen > 127) {
        fixedHeaderSize++;
    }
    uint8_t fixed_header[fixedHeaderSize];
    fixed_header[0] = MQTT_MSG_CONNECT;
    if(remainLen <=127) {
        fixed_header[1] = remainLen;
    } else {
        fixed_header[1] = remainLen%128;
        fixed_header[1] |= 0x80;
        fixed_header[2] = remainLen/128;
    }
    uint16_t offset = 0;
    uint8_t packet[sizeof(fixed_header) + sizeof(var_header) + payloadlen];
    memset(packet, 0, sizeof(packet));
    memcpy(packet, fixed_header, sizeof(fixed_header));

    offset += sizeof(fixed_header);
    memcpy(packet+offset, var_header, sizeof(var_header));
    offset += sizeof(var_header);

    packet[offset++] = clientidlen>>8;
    packet[offset++] = clientidlen&0xff;
    memcpy(packet+offset, broker->clientid, clientidlen);
    offset += clientidlen;

    if(usernamelen) {
        packet[offset++] = usernamelen>>8;
        packet[offset++] = usernamelen&0xff;
        memcpy(packet+offset, broker->username, usernamelen);
        offset += usernamelen;
    }
    if(passwordlen) {
        packet[offset++] = passwordlen>>8;
        packet[offset++] = passwordlen&0xff;
        memcpy(packet+offset, broker->password, passwordlen);
        offset += passwordlen;
    }
    int ret;
    ret = broker->send(broker->socket_info, packet, sizeof(packet));
    if(ret < sizeof(packet)) {
        return -1;
    }
    return 0;
}

int mqtt_publish(mqtt_broker_handle_t *broker, const char *topic, const char *msg, uint8_t retain)
{
    return mqtt_publish_with_qos(broker, topic, msg, retain, 0, NULL);
}

int mqtt_publish_with_qos(mqtt_broker_handle_t *broker, const char *topic, const char *msg, uint8_t retain, uint8_t qos, uint16_t *message_id)
{
    uint16_t topiclen = strlen(topic);
    uint16_t msglen = strlen(msg);
    uint8_t qos_flag = MQTT_QOS0_FLAG;
    uint8_t qos_size = 0;
    if(qos == 1) {
        qos_size = 2;
        qos_flag = MQTT_QOS1_FLAG;
    } else if(qos == 2) {
        qos_size = 2;
        qos_flag = MQTT_QOS2_FLAG;
    }
    uint8_t var_header[topiclen + 2 + qos_size];//2 for len(2 bytes)
    memset(var_header, 0 ,sizeof(var_header));
    var_header[0] = topiclen>>8;
    var_header[1] = topiclen&0xff;
    memcpy(var_header+2, topic, topiclen);
    if(qos_size) {
        var_header[topiclen+2] = broker->seq>>8;
        var_header[topiclen+3] = broker->seq&0xff;
        if(message_id) {
            *message_id = broker->seq;
        }
        broker->seq ++;
    }
    //fixed header
    uint8_t fixedHeaderSize = 2;
    uint16_t remainLen = strlen(msg) + sizeof(var_header);
    if(remainLen > 127) {
        fixedHeaderSize ++;
    }
    uint8_t fixed_header[fixedHeaderSize];
    fixed_header[0] = MQTT_MSG_PUBLISH | qos_flag;
    if(retain) {
        fixed_header[0] |= MQTT_RETAIN_FLAG;
    }
    if(remainLen <= 127) {
        fixed_header[1] = remainLen;
    } else {
        fixed_header[1] = remainLen%128;
        fixed_header[1] |= 0x80;
        fixed_header[2] = remainLen/128;

    }
    uint8_t packet[sizeof(var_header) + sizeof(fixed_header) + msglen];
    memset(packet, 0, sizeof(packet));
    memcpy(packet, fixed_header, sizeof(fixed_header));
    memcpy(packet+sizeof(fixed_header), var_header, sizeof(var_header));
    memcpy(packet+sizeof(fixed_header)+sizeof(var_header), msg, msglen);
    int ret;
    ret = broker->send(broker->socket_info, packet, sizeof(packet));
    if(ret < packet) {
        return -1;
    }
    return 0;
}

int mqtt_disconnect(mqtt_broker_handle_t *broker)
{
    uint8_t packet[] = {
        MQTT_MSG_DISCONNECT,
        0x00
    };
    int ret;
    ret = broker->send(broker->socket_info, packet, sizeof(packet));
    if(ret != sizeof(packet)) {
        return -1;
    }
    return 0;
}

