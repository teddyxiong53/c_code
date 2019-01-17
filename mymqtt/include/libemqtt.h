#ifndef _LIBEMQTT_H_
#define _LIBEMQTT_H_

#include <stdint.h>
#include "mylog.h"

#define MQTT_CONF_USERNAME_LENGTH 13
#define MQTT_CONF_PASSSWORD_LENGTH 13

typedef struct {
    void * socket_info;
    int (*send)(void *socket_info, const void *buf, unsigned int count);
    char clientid[50];
    char username[MQTT_CONF_USERNAME_LENGTH];
    char password[MQTT_CONF_PASSSWORD_LENGTH];
    uint8_t will_retain;
    uint8_t will_qos;
    uint8_t clean_session;
    uint16_t seq;
    uint16_t alive;
} mqtt_broker_handle_t;


#define MQTT_MSG_CONNECT 1<<4
#define MQTT_MSG_CONNACK 2<<4
#define MQTT_MSG_PUBLISH 3<<4
#define MQTT_MSG_PUBACK 4<<4
#define MQTT_MSG_PUBREC 5<<4
#define MQTT_MSG_PUBREL 6<<4
#define MQTT_MSG_PUBCOMP 7<<4

#define MQTT_MSG_SUBCRIBE 8<<4
#define MQTT_MSG_SUBACK   9<<4
#define MQTT_MSG_UNSUBCRIBE 10<<4
#define MQTT_MSG_UNSUBACK  11<4
#define MQTT_MSG_PINGREQ  12<<4
#define MQTT_MSG_PINGRESP  13<<4
#define MQTT_MSG_DISCONNECT 14<<4


#define MQTTParseMessageType(buffer)  (*buffer&0xf0)
#define MQTTParseMesageDuplicate(buffer)  (*buffer&0x80)
#define MQTTParseMessageQos(buffer) ((*buffer &0x60)>>1)
#define MQTTParseMessageRetain(buffer) (*buffer&0x01)

void mqtt_init(mqtt_broker_handle_t *broker, const char *clientid);
void mqtt_init_auth(mqtt_broker_handle_t *broker, const char *username, const char *password);

int mqtt_publish(mqtt_broker_handle_t *broker, const char *topic, const char *msg, uint8_t retain);
int mqtt_publish_with_qos(mqtt_broker_handle_t *broker, const char *topic, const char *msg, uint8_t retain, uint8_t qos, uint16_t *message_id);
int mqtt_connect(mqtt_broker_handle_t *broker);
void mqtt_set_alive(mqtt_broker_handle_t *broker, int alive);
int mqtt_disconnect(mqtt_broker_handle_t *broker);
#endif