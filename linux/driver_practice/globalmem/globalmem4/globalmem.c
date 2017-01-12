#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/semaphore.h>



#define GLOBALMEM_SIZE 0x1000

struct globalmem_dev
{
	struct cdev cdev;//just like inherit from cdev
	char mem[GLOBALMEM_SIZE];
	struct semaphore sem;
};


struct globalmem_dev *globalmem_devp;
static int globalmem_major = 230;

#define MEM_CLEAR 0x01

static ssize_t globalmem_read(struct file * filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;
	if(p >= GLOBALMEM_SIZE)
	{
		return 0;
	}
	if(count > GLOBALMEM_SIZE-p)
	{
		count = GLOBALMEM_SIZE-p;
	}
	if(down_interruptible(&dev->sem))
	{
		return -ERESTARTSYS;
	}
	if(copy_to_user(buf, (void *)(dev->mem +p), count )) 
	{
		ret = -EFAULT;
	}
	else
	{
		*ppos += count;
		ret = count;
		printk(KERN_INFO "read %d bytes from globalmem \n", size);
	}
	up(&dev->sem);
	return ret;
}


static ssize_t globalmem_write (struct file *filp , const char __user *buf , size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;
	if(p >= GLOBALMEM_SIZE)
	{
		return 0;
	}
	if(count > GLOBALMEM_SIZE-p)
	{
		count = GLOBALMEM_SIZE-p;
	}
	if(down_interruptible(&dev->sem))
	{
		return -ERESTARTSYS;
	}
	if(copy_from_user((dev->mem +p), buf, count )) 
	{
		ret = -EFAULT;
	}
	else
	{
		*ppos += count;
		ret = count;
		printk(KERN_INFO "write %d bytes to globalmem \n", size);
	}
	up(&dev->sem);
	return ret;
}

static loff_t globalmem_llseek (struct file *filp , loff_t offset , int orig)
{
	loff_t ret = 0;
	switch(orig) {
		case 0:
			if(offset < 0)
			{
				ret = -EINVAL;
				break;
			}
			if(offset > GLOBALMEM_SIZE) 
			{
				ret = -EINVAL;
				break;
			}
			filp->f_pos = offset;
			ret = filp->f_pos;
			
			break;
		case 1:
			if((filp->f_pos + offset > GLOBALMEM_SIZE) 
				|| (filp->f_pos+offset < 0) )
			{
				ret = -EINVAL;
				break;
			}
			filp->f_pos += offset;
			ret = filp->f_pos;
			break;
		default:
			ret = -EINVAL;
			break;
	}
	return ret;

}

static int globalmem_ioctl (struct inode * inodep, struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;
	switch(cmd) {
		case MEM_CLEAR:
			if(down_interruptible(&dev->sem))
			{
				return -ERESTARTSYS;
			}
			memset(dev->mem, 0, GLOBALMEM_SIZE);
			up(&dev->sem);
			printk(KERN_INFO "globalmem is set to zero \n");
			break;
		default:
			return -EINVAL;
	}
	
	return 0;
}

static int globalmem_open (struct inode * inodep , struct file * filp)
{
	filp->private_data = globalmem_devp;
	return 0;
}


static int globalmem_release (struct inode * inodep , struct file * filp)
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
	.open = globalmem_open,
	.release = globalmem_release,
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
	globalmem_devp->cdev.owner = THIS_MODULE;
	cdev_init(&globalmem_devp->cdev, &globalmem_fops);
	ret = cdev_add(&globalmem_devp->cdev, devno, 1);
	if(ret)
	{
		printk(KERN_ERR "cdev_add failed \n");
		return ret;
	}
	sema_init(&globalmem_devp->sem, 1);
	printk(KERN_EMERG "xhl -- gm init \n");
	return 0;
	
fail_malloc:
	unregister_chrdev_region(devno, 1);
	return ret;
}

void globalmem_exit(void)
{
	printk(KERN_EMERG     "xhl -- gm exit \n");
	cdev_del(&globalmem_devp->cdev);
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);

}

module_init(globalmem_init);
module_exit(globalmem_exit);


