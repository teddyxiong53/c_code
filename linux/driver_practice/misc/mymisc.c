
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>



MODULE_AUTHOR("teddyxiong53 1073167306@qq.com");
MODULE_DESCRIPTION("mymisc driver");
MODULE_LICENSE("GPL");






static int mymisc_open(struct inode *inode, struct file *file)
{
	printk("mymisc open\n");
	return 0;
}

static int mymisc_release(struct inode *inode, struct file *file)
{
	printk("mymisc release\n");
	return 0;
}
static ssize_t mymisc_read(struct file *filp, char *buf, size_t count, loff_t fpos)
{
	printk("mymisc read \n");
	return 0;
}

static ssize_t mymisc_write(struct file *filp, char *buf, size_t count, loff_t fpos)
{
	printk("mymisc write \n");
	return 0;
}

static int mymisc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("mymisc ioctl: cmd:%d, arg:%ld \n", cmd, arg);
	return 0;
}


static const struct file_operations mymisc_fops = {
	.owner		= THIS_MODULE,
	.open		= mymisc_open,
	.release	= mymisc_release,
	.compat_ioctl      = mymisc_ioctl,
	.read 		= mymisc_read,
	.write		= mymisc_write,
};

static struct miscdevice mymisc = {
	.fops		= &mymisc_fops,
	.name		= "mymisc",
	.minor		= MISC_DYNAMIC_MINOR,
};

static int __init mymisc_init(void)
{
	return misc_register(&mymisc);
}

static void __exit mymisc_exit(void)
{
	misc_deregister(&mymisc);
}

module_init(mymisc_init);
module_exit(mymisc_exit);
