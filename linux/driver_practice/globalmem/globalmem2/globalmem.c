#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>


#define GLOBALMEM_SIZE 0x1000

struct globalmem_dev
{
	struct cdev cdev;//just like inherit from cdev
	char mem[GLOBALMEM_SIZE];
};


struct globalmem_dev *globalmem_devp;
static int globalmem_major = 230;


static ssize_t globalmem_read(struct file * filp, char __user *buf, size_t count)
{
	return 0;
}

static ssize_t globalmem_write (struct file *filp , const char __user *buf , size_t count, loff_t *ppos)
{
	return 0;
}

static loff_t globalmem_llseek (struct file *filp , loff_t pos , int orig)
{
	return 0;
}

static int globalmem_ioctl (struct inode * inodep, struct file *filp, unsigned int cmd, unsigned long arg)
{
	return 0;
}

struct file_operations globalmem_fops =
{
	.owner = THIS_MODULE,
	.llseek = globalmem_llseek,
	.read = globalmem_read,
	.write = globalmem_write,
	.compat_ioctl = globalmem_ioctl,
};





int globalmem_init(void)
{
	dev_t devno = MKDEV(globalmem_major, 0);
	int ret = -1;
	if(globalmem_major)
	{
		ret = register_chrdev_region(devno, 1, "globalmem");
	}
	else
	{
		ret = alloc_chrdev_region(&devno, 0, 1, "globalmem");
		globalmem_major = MAJOR(devno);
	}
	if(ret )
	{
		printk(KERN_ERR "register_chrdev_region failed \n");
		return ret;
	}
	globalmem_devp = kmalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
	if(globalmem_devp == NULL)
	{
		printk(KERN_ERR "kmalloc failed \n");
		goto fail_malloc;
	}
	memset(globalmem_devp, 0, sizeof(struct globalmem_dev));
	
	cdev_init(&dev.cdev, &globalmem_fops);
	ret = cdev_add(&dev.cdev, devno, 1);
	if(ret)
	{
		printk(KERN_ERR "cdev_add failed \n");
		return ret;
	}
	printk(KERN_EMERG "xhl -- gm init \n");
	return 0;
	
fail_malloc:
	unregister_chrdev_region(devno, 1);
	return ret;
}

void globalmem_exit(void)
{
	printk(KERN_EMERG     "xhl -- gm exit \n");
	cdev_del(&dev.cdev);
	unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);
	
}

module_init(globalmem_init);
module_exit(globalmem_exit);


