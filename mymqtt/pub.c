#include <sys/socket.h>

#include "mymqtt.h"
#include "mylog.h"

mqtt_broker_handle_t pub_broker;
int sockfd = 0;

#define MQTT_PORT 1883 

#define PUB_BUFSIZE 1024
char pub_recv_buf[PUB_BUFSIZE];
int pub_send_packet(void *socket_info, void *buf, u32 len)
{
    int sock = *((int *)socket_info);
    return send(sock, buf, len,0);
    
}
int pub_init_socket(char *host, u16 port)
{
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        myloge("create socket failed");
        return -1;
    }
    mylogd("sockfd is:%d", sockfd);
    int ret ;
    int flag = 1;
    ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    if(ret <0 ){
        myloge("set socket options failed");
        return -1;
    }
    struct sockaddr_in serveraddr = {0};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(hostname);
    serveraddr.sin_port = htons(port);
    
    ret = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct serveraddr));
    if(ret < 0) {
        myloge("connect failed");
        return -1;
    }
    pub_broker->socket_info = (void *)&sockfd;
    pub_broker->send = pub_send_packet;
    
}

int pub_read_packet(int timeout)
{
    if(timeout > 0) {
        fd_set readfds;
        struct timeval tv = {0};
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        int ret;
        ret = select(sockfd+1, &readfds, NULL, NULL, &tv);
        if(ret > 0) {
            mylogd("recv packet");
        } else if (ret == 0) {
            mylogd("recv timeout");
        } else {
            myloge("select return error");
            return -1;
        }
    }
    memset(pub_recv_buf, 0, sizeof(pub_recv_buf));
    int total_bytes = 0;
    int received_bytes = 0;
    int packet_len = 0;
    while(total_bytes < 2) {
        received_bytes = recv(sockfd, pub_recv_buf+total_bytes, PUB_BUFSIZE, 0);
        if(received_bytes <= 0) {
            return -1;
        }
        total_bytes += received_bytes;
    }
    packet_len = pub_recv_buf[1] + 2;
    while(total_bytes < packet_len) {
        received_bytes = recv(sockfd, pub_recv_buf+total_bytes, PUB_BUFSIZE, 0);
        if(received_bytes <= 0) {
            return -1;
        }
        total_bytes += received_bytes;
    }
    return packet_len;
}


int main(int argc, char **argv)
{
    u16 msgid, msgid_recv;
    
    mqtt_init(&pub_broker, "mymqttpub");
    mqtt_init_auth(&pub_broker, "teddy", "123456");
    pub_init_socket("127.0.0.1", MQTT_PORT);

    // 1.conn
    mqtt_connect(&pub_broker);
    // 2. wait for ack
    int packet_len = pub_read_packet(1);
    if(packet_len < 0) {
        myloge("read conn ack failed");
        return -1;
    }
    // 3. parse packet
    if(mqtt_parse_msg_type(pub_recv_buf) != MQTT_MSG_CONN_ACK) {
        myloge("no receive conn ack");
        return -1;
    }
    // 4. pub a message, use qos 0
    mylogd("pub(using qos 0)");
    mqtt_publish(&broker, "topic1/lev2/lev3", "msg1", 0);
    // 5. wait for pub ack
    packet_len = pub_read_packet(1);
    if(packet_len < 0) {
        myloge("read pub ack failed");
        return -1;
    }
    if(mqtt_parse_msg_type(pub_recv_buf) != MQTT_MSG_PUB_ACK) {
        myloge("receive is not pub ack ");
        return -1;
    }
    msgid_recv = mqtt_parse_msg_id(pub_recv_buf);
}
