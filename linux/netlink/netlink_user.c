#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define NETLINK_USER NETLINK_USERSOCK
#define MAX_PAYLOAD 1024

int main(int argc, char **argv)
{
    int sockfd;
    sockfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USER);
    if(sockfd < 0) {
        printf("socket failed \n");
        perror("");
        return -1;
    }
    struct sockaddr_nl src_addr,dst_addr;
    memset(&src_addr, 0 ,sizeof(src_addr));
    memset(&dst_addr, 0 ,sizeof(dst_addr));
    
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    
    int ret;
    ret = bind(sockfd, (struct sockaddr *)&src_addr, sizeof(src_addr) );
    if(ret < 0) {
        printf("bind failed\n");
        return -1;
    }
    
    dst_addr.nl_family =AF_NETLINK;
    dst_addr.nl_pid = 0;//to kernel
    dst_addr.nl_groups = 0;//unicast
    
    struct nlmsghdr *nlh;
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    
    strcpy(NLMSG_DATA(nlh), "hello");
    struct iovec iov;
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    struct msghdr msg;
    msg.msg_name = (void *)&dst_addr;
    msg.msg_namelen = sizeof(dst_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    printf("send msg to kernel \n");
    sendmsg(sockfd, &msg, 0);
    printf("waiting for msg from kernel\n");
    recvmsg(sockfd, &msg, 0);
    printf("recv from kernel:%s \n", (char *)NLMSG_DATA(nlh));
    close(sockfd);
}
