#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/sched.h>


#define GLOBALFIFO_SIZE 0x1000

struct globalfifo_dev
{
	struct cdev cdev;//just like inherit from cdev
	unsigned int current_len;
	char mem[GLOBALFIFO_SIZE];
	struct semaphore sem;
	wait_queue_head_t r_wait;
	wait_queue_head_t w_wait;
};


struct globalfifo_dev *globalfifo_devp;
static int globalfifo_major = 229;

#define MEM_CLEAR 0x01

static ssize_t globalfifo_read(struct file * filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalfifo_dev *dev = filp->private_data;
	
	DECLARE_WAITQUEUE(wait, current);
	down(&dev->sem);
	add_wait_queue(&dev->r_wait, &wait);
	while(dev->current_len == 0)
	{
		if(filp->f_flags & O_NONBLOCK)
		{
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		up(&dev->sem);
		schedule();
		if(signal_pending(current))
		{
			ret = -ERESTARTSYS;
			goto out2;
		}
		down(&dev->sem);//
	}
	
	if(count > dev->current_len)
	{
		count = dev->current_len;
	}
	
	
	if(copy_to_user(buf, (void *)(dev->mem +p), count )) 
	{
		ret = -EFAULT;
		goto out;
	}
	else
	{
		memcpy(dev->mem, dev->mem+count, dev->current_len-count );
		dev->current_len -= count;
		printk(KERN_INFO "read %d bytes from globalfifo, current len:%d \n", size, dev->current_len);
		wake_up_interruptible(&dev->w_wait);
		ret = count;
	}
	
	
out:
	up(&dev->sem);
out2:
	remove_wait_queue(&dev->r_wait, &wait);
	set_current_state(TASK_RUNNING);
	return ret;
}


static ssize_t globalfifo_write (struct file *filp , const char __user *buf , size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalfifo_dev *dev = filp->private_data;
	
	DECLARE_WAITQUEUE(wait, current);
	down(&dev->sem);
	add_wait_queue(&dev->w_wait, &wait);
	while(dev->current_len == GLOBALFIFO_SIZE)
	{
		if(filp->f_flags & O_NONBLOCK)
		{
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		up(&dev->sem);
		schedule();
		if(signal_pending(current))
		{
			ret = -ERESTARTSYS;
			goto out2;
		}
		down(&dev->sem);
	}
	if(count > GLOBALFIFO_SIZE - dev->current_len)
	{
		count = GLOBALFIFO_SIZE - dev->current_len;
	}
	
	if(copy_from_user((dev->mem +dev->current_len), buf, count )) 
	{
		ret = -EFAULT;
		goto out;
	}
	else
	{
		dev->current_len += count;
		
		printk(KERN_INFO "write %d bytes to globalfifo \n", size);
		wake_up_interruptible(&dev->r_wait);
		ret = count;
	}
out:
	up(&dev->sem);
out2:
	remove_wait_queue(&dev->w_wait, &wait);
	set_current_state(TASK_RUNNING);
	return ret;
}

static loff_t globalfifo_llseek (struct file *filp , loff_t offset , int orig)
{
	loff_t ret = 0;
	switch(orig) {
		case 0:
			if(offset < 0)
			{
				ret = -EINVAL;
				break;
			}
			if(offset > GLOBALFIFO_SIZE) 
			{
				ret = -EINVAL;
				break;
			}
			filp->f_pos = offset;
			ret = filp->f_pos;
			
			break;
		case 1:
			if((filp->f_pos + offset > GLOBALFIFO_SIZE) 
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

static int globalfifo_ioctl (struct inode * inodep, struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalfifo_dev *dev = filp->private_data;
	switch(cmd) {
		case MEM_CLEAR:
			if(down_interruptible(&dev->sem))
			{
				return -ERESTARTSYS;
			}
			memset(dev->mem, 0, GLOBALFIFO_SIZE);
			up(&dev->sem);
			printk(KERN_INFO "globalfifo is set to zero \n");
			break;
		default:
			return -EINVAL;
	}
	
	return 0;
}
static unsigned int globalfifo_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	struct globalfifo_dev *dev = filp->private_data;
	down(dev->sem);
	poll_wait(filp, &dev->r_wait, wait);
	poll_wait(filp, &dev->w_wait, wait);
	/* fifo not empty */
	if(dev->current_len != 0)
	{
		mask |= POLLIN | POLLRDNORM;
	}
	/*fifo not full*/
	if(dev->current_len != GLOBALFIFO_SIZE)
	{
		mask |= POLLOUT | POLLWRNORM;
	}
	up(dev->sem);
	return mask;
}
static int globalfifo_open (struct inode * inodep , struct file * filp)
{
	filp->private_data = globalfifo_devp;
	return 0;
}


static int globalfifo_release (struct inode * inodep , struct file * filp)
{

	return 0;
}

struct file_operations globalfifo_fops =
{
	.owner = THIS_MODULE,
	.llseek = globalfifo_llseek,
	.read = globalfifo_read,
	.write = globalfifo_write,
	.compat_ioctl = globalfifo_ioctl,
	.poll = globalfifo_poll,
	.open = globalfifo_open,
	.release = globalfifo_release,
};





int globalfifo_init(void)
{
	dev_t devno = MKDEV(globalfifo_major, 0);
	int ret = -1;
	if(globalfifo_major)
	{
		ret = register_chrdev_region(devno, 1, "globalfifo");
	}
	else
	{
		ret = alloc_chrdev_region(&devno, 0, 1, "globalfifo");
		globalfifo_major = MAJOR(devno);
	}
	if(ret )
	{
		printk(KERN_ERR "register_chrdev_region failed \n");
		return ret;
	}
	globalfifo_devp = kmalloc(sizeof(struct globalfifo_dev), GFP_KERNEL);
	if(globalfifo_devp == NULL)
	{
		printk(KERN_ERR "kmalloc failed \n");
		goto fail_malloc;
	}
	memset(globalfifo_devp, 0, sizeof(struct globalfifo_dev));
	globalfifo_devp->cdev.owner = THIS_MODULE;
	cdev_init(&globalfifo_devp->cdev, &globalfifo_fops);
	ret = cdev_add(&globalfifo_devp->cdev, devno, 1);
	if(ret)
	{
		printk(KERN_ERR "cdev_add failed \n");
		return ret;
	}
	sema_init(&globalfifo_devp->sem, 1);
	init_waitqueue_head(&globalfifo_devp->r_wait);
	init_waitqueue_head(&globalfifo_devp->w_wait);
	printk(KERN_EMERG "xhl -- gm init \n");
	return 0;
	
fail_malloc:
	unregister_chrdev_region(devno, 1);
	return ret;
}

void globalfifo_exit(void)
{
	printk(KERN_EMERG     "xhl -- gm exit \n");
	cdev_del(&globalfifo_devp->cdev);
	kfree(globalfifo_devp);
	unregister_chrdev_region(MKDEV(globalfifo_major, 0), 1);

}

module_init(globalfifo_init);
module_exit(globalfifo_exit);


