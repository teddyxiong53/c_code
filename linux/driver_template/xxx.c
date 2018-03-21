
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>



#include "xxx.h"

static struct class *xxx_class ;
static struct xxx_dev_struct *xxx_dev ;

static struct proc_dir_entry *xxx_proc ;


static dev_t xxx_major, xxx_minor;





static size_t xxx_set_val(struct xxx_dev_struct *dev, const char *buf, size_t count)
{
	int val;
	val = simple_strtol(buf, NULL, 10);
	if(down_interruptible(&(dev->sem))) {
		return -ERESTARTSYS;
	}
	dev->val = val;
	up(&(dev->sem));
	return count;
}
static size_t xxx_get_val(struct xxx_dev_struct *dev, const char *buf)
{
	int val;
	if(down_interruptible(&(dev->sem))) {
		return -ERESTARTSYS;
	}
	val = dev->val;
	up(&(dev->sem));

	return sprintf(buf,  "%d\n",val);
}

ssize_t xxx_val_show(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct xxx_dev_struct *xxx_dev = (struct xxx_dev_struct *)dev_get_drvdata(dev);
	return xxx_get_val(xxx_dev, buf);
}

ssize_t xxx_val_store(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct xxx_dev_struct *xxx_dev = (struct xxx_dev_struct *)dev_get_drvdata(dev);
	return xxx_set_val(xxx_dev, buf, count);
}


static DEVICE_ATTR(val, S_IRUGO|S_IWUSR, xxx_val_show, xxx_val_store);

int xxx_open (struct inode *inode, struct file *file)
{
	struct xxx_dev_struct *dev;
	dev = container_of(inode->i_cdev, struct xxx_dev_struct, dev);
	file->private_data  = dev;
	printk("xxx_open\n");
	return 0;
}
int xxx_release (struct inode *inode, struct file *file)
{
	printk("xxx_release\n");
	return 0;
}

ssize_t xxx_read (struct file *file, char __user * buf, size_t count, loff_t *poff)
{
	struct xxx_dev_struct *dev = (struct xxx_dev_struct *)file->private_data;
	return xxx_get_val(dev, buf);
}


ssize_t xxx_write (struct file *file, const char __user * buf, size_t count, loff_t *poff)
{
	struct xxx_dev_struct *dev = (struct xxx_dev_struct *)file->private_data;
	return xxx_set_val(dev, buf,count);
}

static struct file_operations xxx_fops = {
	.owner = THIS_MODULE,
	.open = xxx_open,
	.release = xxx_release,
	.read = xxx_read,
	.write= xxx_write,
};


static ssize_t xxx_proc_read(struct file *file, char __user *buf, size_t count, loff_t *poff)
{
	ssize_t ret = xxx_get_val(xxx_dev, buf);
	printk("%s", buf);
	return ret;
}
	
static ssize_t	xxx_proc_write (struct file *file, const char __user *buf, size_t count, loff_t *poff)
{
	int ret;
	if(count > PAGE_SIZE) {
		printk("the buff is too large:%lu \n", count);
		return -EFAULT;
	}
	char *tmp;
	tmp = (char *)__get_free_page(GFP_KERNEL);
	if(!tmp) {
		printk("__get_free_page failed\n");
		return -ENOMEM;
	}
	ret = copy_from_user(tmp, buf, count);
	if(ret) {
		printk("copy_from_user failed\n");
		goto error;
	}
	ret = xxx_set_val(xxx_dev, tmp, count);
	if(ret != count) {
		printk("xxx_set_val failed\n");
		goto error;
	}
	return count;
error:
	free_page(tmp);
	return ret;
}
struct file_operations xxx_proc_fops = {
	.owner = THIS_MODULE,
	.read = xxx_proc_read,
	.write = xxx_proc_write,
};


static void xxx_create_proc(void)
{
	xxx_proc = proc_create("xxx", 0644, NULL, &xxx_proc_fops);
	if(!xxx_proc) {
		printk("proc_create failed\n");
		return ;
	}
	
}

static void xxx_remove_proc(void)
{
	if(xxx_proc) {
		proc_remove(xxx_proc);
	}
	
}
static int xxx_setup(struct xxx_dev_struct *dev)
{
	dev->val = 0;
	sema_init(&dev->sem, 1);
	dev_t devno = MKDEV(xxx_major, xxx_minor);
	cdev_init(&(dev->dev), &xxx_fops);
	dev->dev.ops = &xxx_fops;
	dev->dev.owner = THIS_MODULE;
	int ret = cdev_add(&(dev->dev), dev, 1);
	if(ret < 0) {
		printk("cdev_add failed\n");
		return -1;
	}
	return 0;
}

static int xxx_init(void)
{
	printk("xxx_init begin\n");
	int ret;
	struct device *temp;
	dev_t dev;
	ret = alloc_chrdev_region(&dev, 0, 1, "xxx");
	if(ret ) {
		printk("alloc_chrdev_region failed \n");
		goto error1;
	}
	xxx_major = MAJOR(dev);
	xxx_minor = MINOR(dev);
	xxx_dev = kzalloc(sizeof(struct xxx_dev_struct), GFP_KERNEL);
	if(xxx_dev == NULL) {
		printk("kzalloc failed \n");
		goto error2;
	}
	ret = xxx_setup(xxx_dev);
	if(ret) {
		printk("xxx_setup failed\n");
		goto error3;
	}
	// create sys dir
	xxx_class = class_create(THIS_MODULE, "xxx");
	if(IS_ERR(xxx_class)) {
		printk("class_create failed\n");
		goto error3;
	}
	// create file xxx under /dev and /sys/class/xxx 
	temp = device_create(xxx_class, NULL, dev, NULL, "xxx");
	if(IS_ERR(temp)) {
		printk("device_create failed\n");
		goto error4;
	}
	ret = device_create_file(temp, &dev_attr_val);
	if(ret<0) {
		printk("device_create_file failed\n");
		goto error5;
	}
	dev_set_drvdata(temp, xxx_dev);
	xxx_create_proc();
	return 0;
error5:
	device_destroy(xxx_class, dev);
error4:
	class_destroy(xxx_class);
error3:
	kfree(xxx_dev);
error2:
	unregister_chrdev_region(dev, 1);
error1:
	return -1;
}

static void xxx_exit(void)
{
	dev_t devno = MKDEV(xxx_major, xxx_minor);
	printk("xxx_exit \n");
	xxx_remove_proc();
	if(xxx_class) {
		device_destroy(xxx_class, devno);
		class_destroy(xxx_class);
	}
	if(xxx_dev) {
		cdev_del(&(xxx_dev->dev));
		kfree(xxx_dev);
	}
	// release dev number resource
	unregister_chrdev_region(devno, 1);
	
}
module_init(xxx_init);
module_exit(xxx_exit);

MODULE_LICENSE("GPL");

