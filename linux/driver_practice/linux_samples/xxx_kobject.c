#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sysfs.h>

static struct kobject *xxx_kobject;

extern struct kobject *kernel_kobj;
static int aa,bb,cc;
static ssize_t aa_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", aa);
}

static ssize_t aa_store(struct kobject *kobj, 
    struct kobj_attribute *attr, 
    const char *buf, size_t count)
{
    int ret;
    ret = kstrtoint(buf,10,&aa);
    if(ret<0) {
        return ret;
    }
    return count;
}

static ssize_t bb_show(struct kobject *kobj,
	struct kobj_attribute *attr,
	char *buf)
{
	return sprintf(buf, "%d\n", bb);
}

static ssize_t bb_store(struct kobject *kobj, 
	struct kobj_attribute *attr,
	const char *buf, size_t count)
{
	int ret;
	ret = kstrtoint(buf, 10, &bb);
	if(ret <0) {
		return ret;
	}
	return count;
}

static ssize_t cc_show(
	struct kobject *kobj,
	struct kobj_attribute *attr,
	char *buf
)
{
	return sprintf(buf, "%d\n", cc);
}

static ssize_t cc_store(
	struct kobject *kobj,
	struct kobj_attribute *attr,
	const char *buf, 
	size_t count
)
{
	int ret;
	ret = kstrtoint(buf, 10, &cc);
	if(ret <0) {
		return ret;
	}
	return count;
}


static struct kobj_attribute aa_attribute = 
    __ATTR(aa, 0644, aa_show, aa_store);

static struct kobj_attribute bb_attribute = 
	__ATTR(bb, 0644, bb_show, bb_store);


static struct kobj_attribute cc_attribute = 
	__ATTR(cc, 0644, cc_show, cc_store);



static struct attribute *xxx_attr[] = {
      &aa_attribute.attr,
      &bb_attribute.attr,
      &cc_attribute.attr,
      NULL, // this is very nessesary
};


static struct attribute_group xxx_attr_group = {
    .attrs = xxx_attr,
};

int xxx_kobject_init(void)
{
    printk("xxx kobject init\n");
    xxx_kobject = kobject_create_and_add("xxx_kobject", kernel_kobj);
    if(!xxx_kobject) {
        printk("create kobject failed\n");
        return -ENOMEM;
    }
    int ret ;
    ret = sysfs_create_group(xxx_kobject, &xxx_attr_group);
	if(ret < 0) {
		printk("sysfs create group failed\n");
		kobject_put(xxx_kobject);
		
	}
	return ret;
}


void xxx_kobject_exit(void)
{
    printk("xxx kobject exit\n");
	kobject_put(xxx_kobject);
}


module_init(xxx_kobject_init);
module_exit(xxx_kobject_exit);
MODULE_AUTHOR("teddyxiong53 <1073167306@qq.com>");
MODULE_LICENSE("Dual BSD/GPL");
