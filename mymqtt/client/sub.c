#include "libemqtt.h"
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

mqtt_broker_handle_t broker;
uint16_t keepalive = 3;

int socket_id = 0;
#define RCVBUFSIZE 1024
uint8_t packet_buffer[RCVBUFSIZE];

int send_packet(void *socket_info, const void *buf, unsigned int count)
{
    int fd = *((int *)socket_info);
    return send(fd, buf, count, 0);

}
int init_socket(mqtt_broker_handle_t *broker, const char *hostname, short port, uint16_t alive)
{
    int flag = 1;
    int ret;
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_id < 0) {
        myloge("socket create fail");
        return -1;
    }
    ret = setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    if(ret < 0) {
        myloge("set sock opt fail");
        return -1;
    }
    struct sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr = inet_addr(hostname);
    socket_addr.sin_port = htons(port);
    ret = connect(socket_id, (struct sockaddr *)&socket_addr, sizeof(socket_addr));
    if(ret<0) {
        myloge("connect fail");
        return -1;
    }
    mqtt_set_alive(alive);
    broker->socket_info = (void *)socket_id;
    broker->send = send_packet;
    return 0;
}

int read_packet(int timeout)
{
    int ret = 0;
    if(timeout > 0) {
        fd_set readfds;
        struct timeval tmv;
        FD_ZERO(&readfds);
        FD_SET(socket_id, &readfds);
        tmv.tv_sec = timeout;
        tmv.tv_usec = 0;
        ret = select(socket_fd+1, &readfds, 0, 0, &tmv);
        if(ret == 0) {
            myloge("select timeout");
            return -1;
        } else if(ret > 0) {
            if(FD_ISSET(socket_id, readfds)) {
                mylogd("select get input");
            }

        } else {
            myloge("select fail");
        }
    }
    int total_bytes;
    int packet_length;
    memset(packet_buffer,0,sizeof(packet_buffer));
    ret = recv(socket_id, packet_buffer, sizeof(packet_buffer), 0);
    if(ret <= 0) {
        myloge("recv fail");
        return -1;
    }
    total_bytes += ret;
    if(total_bytes < 2) {
        myloge("recv error, too short data");
        return -1;
    }
    uint16_t rem_len = mqtt_parse_rem_len(packet_buffer);
    uint8_t rem_len_bytes = mqtt_num_rem_len_bytes(packet_buffer);
    packet_length = rem_len_bytes + rem_len + 1;
    while(total_bytes < packet_length) {
        ret = recv(socket_id, packet_buffer+total_bytes, RCVBUFSIZE, 0);
        total_bytes += ret;
    }
    return packet_length;
}
int main(int argc, char const *argv[])
{
    int packet_length = 0;
    uint16_t msg_id, msg_id_rcv;
    mqtt_init(&broker, "test_cid");
    int ret;
    ret = init_socket(&broker, "0.0.0.0", 1883, keepalive);
    if(ret < 0) {
        myloge("init socket fail");
        return -1;
    }
    mqtt_connect(&broker);
    packet_length = read_packet(1);
    if(packet_length < 0) {
        myloge("read packet fail");
        return -1;
    }
    if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_CONNACK) {
        myloge("expect msg conn ack");
        return -1;
    }
    if(packet_buffer[3] != 0x00) {
        myloge("conn ack error");
        return -1;
    }
    mqtt_subscribe(&broker, "hello", &msg_id);

    packet_length = read_packet(1);
    if(packet_buffer < 0) {
        myloge("read packet fail");
        return -1;
    }
    if(MQTTParseMessageRetain(packet_buffer) != MQTT_MSG_SUBACK) {
        myloge("expect msg sub ack");
        return -1;
    }
    msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
    if(msg_id != msg_id_rcv) {
        myloge("msg id is not right");
        return -1;
    }
    while(1) {
        packet_length = read_packet(0);
        if(packet_length == -1) {
            myloge("some error happens ");
            return -1;
        } else if(packet_length > 0) {
            mylogd("get msg");
            if(MQTTParseMessageType(packet_buffer) == MQTT_MSG_PUBLISH) {
                uint8_t topic[255];
                uint8_t msg[1000];
                uint16_t len;
                len = mqtt_parse_pub_topic(packet_buffer, topic);
                topic[len] = '\0';
                len = mqtt_parse_publish_msg(packet_buffer, msg);
            }
        }
    }
    return 0;
}
