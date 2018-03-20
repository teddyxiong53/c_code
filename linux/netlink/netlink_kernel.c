#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/skbuff.h>
struct sock *nl_sock;

static void nl_recv_msg(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	nlh = (struct nlmsghdr *)skb->data;
	char *msg = "hello from kernel";
	int msgsize = strlen(msg);
	int pid;
	pid = nlh->nlmsg_pid;
	printk("msg from :%d, msg content:%s \n", pid, (char *)nlmsg_data(nlh));
	struct sk_buff *skb_out;
	skb_out = nlmsg_new(msgsize, 0);
	if(!skb_out) {
		printk("nlmsg_new failed\n");
		return;
	}
	nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE,msgsize, 0);
	NETLINK_CB(skb_out).dst_group = 0;
	strncpy(nlmsg_data(nlh), msg, msgsize);
	int ret;
	ret = nlmsg_unicast(nl_sock, skb_out, 0);
	if(ret < 0) {
		printk("nlmsg_unicast failed\n");
	}
	
}
static int nl_init(void)
{
    struct netlink_kernel_cfg cfg = {
		.input = nl_recv_msg,
	};
	nl_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, &cfg);
	if(!nl_sock) {
		printk("netlink_kernel_create failed\n");
		return -1;
	}
	printk("nl_init ok\n");
	return 0;
}

static void nl_exit(void)
{
	printk("nl_exit\n");
	netlink_kernel_release(nl_sock);
}
module_init(nl_init);
module_exit(nl_exit);

MODULE_LICENSE("Dual BSD/GPL");