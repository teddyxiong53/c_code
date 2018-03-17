#ifndef __MYMQTT_H__
#define __MYMQTT_H__

#include <stdint.h>

#define u8 uint8_t 
#define u16 uint16_t 
#define u32 uint32_t 
#define u64 uint64_t 

#define s8 sint8_t 
#define s16 sint16_t 
#define s32 sint32_t 
#define s64 sint64_t 


#define USERNAME_LEN 13
#define PASSWORD_LEN 13

//flags
#define MQTT_USRENAME_FLAG 1<<7
#define MQTT_PASSWORD_FLAG 1<<7
#define MQTT_WILL_RETAIN 1<<5
#define MQTT_WILL_FLAG 1<<2
#define MQTT_CLEAN_SESSION 1<<1

//msg type
#define MQTT_MSG_CONN 1<<4
#define MQTT_MSG_CONN_ACK 2<<4

#define MQTT_MSG_PUB 3<<4
#define MQTT_MSG_PUB_ACK 4<<4
#define MQTT_MSG_PUB_REC 5<<4
#define MQTT_MSG_PUB_REL 6<<4
#define MQTT_MSG_PUB_COMP 7<<4

#define MQTT_MSG_SUB 8<<4
#define MQTT_MSG_SUB_ACK 9<<4

#define MQTT_MSG_UNSUB 10<<4
#define MQTT_MSG_UNSUB_ACK 11<<4


#define MQTT_MSG_PING_REQ 12<<4
#define MQTT_MSG_PING_RESP 13<<4

#define MQTT_MSG_DISCONN 14<<4


// fix header flag
#define MQTT_QOS0_FLAG 0<<1
#define MQTT_QOS1_FLAG 1<<1

#define MQTT_QOS2_FLAG 2<<1

#define MQTT_RETAIN_FLAG 1




typedef struct {
    char clientid[50];
    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    u8 will_retain;
    u8 will_qos;
    u8 clean_session;
    u16 seq;
    u16 alive;
    void *socket_info;
    int (*send)(void *socket_info, void *buf, u32 len);
} mqtt_broker_handle_t;



#endif
