#include "mymqtt.h"


int mqtt_init(mqtt_broker_handle_t *broker, char *clientid)
{
    broker->alive = 300;// 5min
    broker->seq = 1;// seq no
    memset(broker->clientid, 0, sizeof(broker->clientid));
    memset(broker->username, 0, sizeof(broker->username));
    memset(broker->password, 0, sizeof(broker->password));

    strcpy(broker->clientid, clientid );
    broker->will_qos = 0;
    broker->will_retain = 0;
    
}

int mqtt_init_auth(mqtt_broker_handle_t *broker, char *username, char *password)
{
    if(username) {
        strncpy(broker->username, username, sizeof(broker->username)-1);
    }
    if(password) {
        strncpy(broker->password, password, sizeof(broker->password)-1);
    }
    return 0;
}

int mqtt_connect(mqtt_broker_handle_t *broker)
{
    u16 clientid_len = strlen(broker->clientid);
    u16 payload_len = clientid_len + 2;
    u16 username_len = strlen(broker->username);
    u16 password_len = strlen(broker->password);
    u8 flags = 0;
    if(username_len) {
        payload_len += username_len + 2;
        flags |= MQTT_USRENAME_FLAG;
    }
    if(password_len) {
        payload_len += password_len + 2;
        flags |= MQTT_PASSWORD_FLAG;
    }
    if(broker->clean_session) {
        flags |= MQTT_CLEAN_SESSION;
    }
    u8 var_headers = {
        0x00,0x06, 0x4d, 0x51, 0x49, 0x73, 0x64, 0x70,// MQIdsp
        0x03,
        flags,
        broker->alive>>8, broker->alive&0xff,
    };
    u8 fix_header_len = 2;
    u8 remain_len = sizeof(var_headers) + payload_len;
    if(remain_len > 127) {
        fix_header_len ++;
    }
    u8 fix_headers[fix_header_len];
    fix_headers[0] = MQTT_MSG_CONN;
    if(remain_len <=127) {
        fix_headers[1] = remain_len;
    } else {
        fix_headers[1] = remain_len%128;
        fix_headers[1] = remain_len|0x80;
        fix_headers[2] = remain/128;
    }
    u8 packet[sizeof(fix_headers)+sizeof(var_headers)+payload_len];
    memset(packet, 0, sizeof(packet));
    memcpy(packet, fix_headers, sizeof(fix_headers));
    u16 offset = 0;
    offset += sizeof(fix_headers);
    memcpy(packet+offset, var_headers, sizeof(var_headers));
    offset += sizeof(var_headers);
    memcpy(packet+offset, broker->clientid,clientid_len);
    offset += clientid_len;

    if(username_len) {
        packet[offset++] = username_len>>8;
        packet[offset++] = username_len&0xff;
        memcpy(packet+offset, broker->username, username_len);
        offset += username_len;
    }

    if(password_len) {
        packet[offset++] = password_len>>8;
        packet[offset++] = password_len&0xff;
        memcpy(packet+offset, broker->password, password_len);
        offset += password_len;
    }

    //send 
    broker->send(broker->socket_info, packet, sizeof(packet));
    return 0;
    
}

int mqtt_parse_msg_type(char *buf)
{
    return *buf&0xf0;
}

int mqtt_publish_with_qos
(
    mqtt_broker_handle_t *broker,
    char *topic,
    char *msg,
    u8 retain,
    u8 qos, 
    u16 *message_id
)
{
    u16 topiclen = strlen(topic);
    u16 msglen = strlen(msglen);
    u8 qos_flag = MQTT_QOS0_FLAG;
    u8 qos_size = 0;

    if(qos == 1) {
        qos_size = 2;// 2 bytes for qos
        qos_flag = MQTT_QOS1_FLAG;
    } else if(qos == 2) {
        qos_size = 2;
        qos_flag = MQTT_QOS2_FLAG;
    }
    u8 var_header[topiclen+2+qos_size];
    memset(var_header, 0, sizeof(var_header));
    var_header[0] = topiclen >>8;
    var_header[1]  =topiclen&0xff;
    memcpy(var_header+2, topic, topiclen);
    if(qos_size) {
        var_header[topiclen+2] = broker->seq>>8;
        var_header[topiclen+3] = broker->seq&0xff;
        if(message_id) {
            *message_id = broker->seq;
        }
        broker->seq ++;
    }
    u8 fix_header_size = 2;
    u16 remainlen = sizeof(var_header)+msglen;
    if(remainlen > 127) {
        fix_header_size ++;
    }
    u8 fix_header[fix_header_size];
    fix_header[0] = MQTT_MSG_PUB | qos_flag;
    if(retain) {
        fix_header[0] |= MQTT_RETAIN_FLAG;
    }
    if(remainlen <= 127) {
        fix_header[1] = remainlen;
    } else {
        fix_header[1] = remainlen%128;
        fix_header[1] |= 0x80;
        fix_header[2] = remainlen/128;
    }
    u8 packet[fix_header_size + sizeof(var_header) + msglen];
    memset(packet, 0, sizeof(packet));
    memcpy(packet, fix_header, sieof(fix_header));
    int offset = 0;
    offset += sizeof(fix_header);
    memcpy(packet+offset, var_header, sizeof(var_header));
    offset += sizeof(var_header);
    memcpy(packet+offset, msg, msglen);

    int ret ;
    ret = broker->send(broker->socket_info, packet, sizeof(packet), 0);
    return ret;
}
int mqtt_publish(mqtt_broker_handle_t *broker, char *topic, char *msg, u8 retain)
{
    return mqtt_publish_with_qos(broker, topic, msg, retain, 0, NULL);
}


u8 mqtt_parse_msg_qos(char *buf)
{
    return ((*buf)&0x06) >> 1;
}

/**
    only pub and sub have this id.
*/
u16 mqtt_parse_msg_id(char *buf)
{
    u8 type = mqtt_parse_msg_type(buf);
    u8 qos = mqtt_parse_msg_qos(buf);
    u16 id = 0;
    if((type >= MQTT_MSG_PUB )
        && (type<=MQTT_MSG_UNSUB_ACK)) {
        if(type == MQTT_MSG_PUB) {
            if(qos!=0) {
                u8 rlb = 
            }
        } else {

        }
    }
}

