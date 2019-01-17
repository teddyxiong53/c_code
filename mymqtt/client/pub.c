#include "libemqtt.h"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/tcp.h>

int socket_id;
#define RCVBUFSIZE 1024
uint8_t packet_buffer[RCVBUFSIZE];

int send_packet(void *socket_info, const char *buf, unsigned int count)
{
    int fd = *((int *)socket_info);
    return send(fd, buf, count, 0);
}
int init_socket(mqtt_broker_handle_t *broker, const char *hostname, short port)
{
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_id < 0) {
        myloge("socket create fail");
        return -1;
    }
    int ret;
    int flag = 1;
    ret = setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag) );
    if(ret) {
        myloge("setsockopt fail");
    }
    struct sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = inet_addr(hostname);
    socket_addr.sin_port = htons(port);
    ret = connect(socket_id, (struct sockaddr *)&socket_addr, sizeof(socket_addr));
    if(ret) {
        myloge("connect fail");
        return -1;
    }
    int keepalive = 3;
    mqtt_set_alive(broker, keepalive);
    broker->socket_info = (void *)&socket_id;
    broker->send = send_packet;
    return 0;
}

int read_packet(int timeout)
{
    int ret;
    if(timeout >0 ) {
        fd_set readfds;
        struct timeval tmv;
        FD_ZERO(&readfds);
        FD_SET(socket_id, &readfds);
        tmv.tv_sec = timeout;
        tmv.tv_usec = 0;

        ret = select(socket_id+1, &readfds, NULL, NULL, &tmv);
        if(ret == 0) {
            mylogd("select timeout");
            //return -1;
        } else if(ret > 0) {
            mylogd("select get input");

        } else {
            myloge("select fail");
            return -1;
        }
    }
    int total_bytes = 0;
    memset(packet_buffer, 0, sizeof(packet_buffer));
    while(total_bytes < 2) {
        ret = recv(socket_id, packet_buffer, RCVBUFSIZE, 0);
        if(ret<=0) {
            myloge("recv fail");
            return -1;
        }
        total_bytes += ret;
    }
    int packet_length = packet_buffer[1] + 2;//why?
    mylogd("packet len:%d", packet_length);
    while(total_bytes < packet_length) {
        ret = recv(socket_id, packet_buffer+total_bytes, RCVBUFSIZE, 0);
        if(ret<=0) {
            return -1;
        }
        total_bytes += ret;
    }
    return packet_length;
}

int close_socket(mqtt_broker_handle_t *broker)
{
    int fd = *((int *)broker->socket_info);
    return close(fd);
}
int main(int argc, char const *argv[])
{
    mqtt_broker_handle_t broker;
    int ret;
    mqtt_init(&broker, "test_cid");
    mqtt_init_auth(&broker, "test_name", "test_pwd");
    init_socket(&broker, "127.0.0.1", 1883);
    ret = mqtt_connect(&broker);
    if(ret < 0) {
        myloge("mqtt_connect fail");
        return -1;
    }
    //read the conn ack
    int packet_length;
    packet_length = read_packet(1);
    if(packet_length < 0) {
        myloge("read packet fail");
        return -1;
    }
    if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_CONNACK) {
        myloge("message type is not conn ack");
        return -1;
    }
    if(packet_buffer[3]!= 0x00) {
        myloge("conn ack fail");
        return -1;
    }
    uint16_t msg_id, msg_id_rcv;
    //pub qos 0
    mqtt_publish(&broker, "hello", "example qos 0", 0);

    //pub qos 1
    mylogd("pub qos 1");
    mqtt_publish_with_qos(&broker, "hello", "example qos 1", 0, 1, &msg_id);
    packet_length = read_packet(1);
    if(packet_length < 0) {
        myloge("read packet fail");
        return -1;
    }
    if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_PUBACK) {
        myloge("message type is not pub ack");
        return -1;
    }
    msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
    if(msg_id != msg_id_rcv) {
        myloge("message id is not right, msg id:%x, recv msg id:%x", msg_id, msg_id_rcv);
        return -1;
    }
    //pub qos 2
    mylogd("pub qos 2");
    mqtt_publish_with_qos(&broker, "hello", "example qos 2", 0, 2, &msg_id);
    packet_length = read_packet(1);
    if(packet_length < 0) {
        myloge("read packet fail");
        return -1;
    }
    if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_PUBREC) {
        myloge("pub rec expected");
        return -1;
    }
    msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
    if(msg_id != msg_id_rcv) {
        myloge("message id is not right, msg id:%x, recv msg id:%x", msg_id, msg_id_rcv);
        return -1;
    }
    mqtt_pubrel(&broker, msg_id);
    //get pub comp
    packet_length = read_packet(1);
    if(packet_length < 0) {
        myloge("read packet fail");
        return -1;
    }
    if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_PUBCOMP) {
        myloge("msg pub comp expected");
        return -1;
    }
    msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
    if(msg_id != msg_id_rcv) {
        myloge("msg id not right");
        return -1;
    }

    mqtt_disconnect(&broker);
    close_socket(&broker);
    return 0;
}
