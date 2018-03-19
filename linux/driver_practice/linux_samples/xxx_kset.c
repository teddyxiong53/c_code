#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

struct xxx_obj {
	struct kobject kobj;
	int aa,bb,cc;
};

#define to_xxx_obj(x) container_of(x, struct xxx_obj, kobj)

struct xxx_attribute {
	struct attribute attr;
	ssize_t (*show)(struct xxx_obj *obj, struct xxx_attribute *attr, char *buf);
	ssize_t (*store)(struct xxx_obj *obj, struct xxx_attribute *attr, const char *buf, size_t count);
	
};

#define to_xxx_attr(x) container_of(x, struct xxx_attribute, attr)


static struct kset *xxx_kset;

extern struct kobject *kernel_kobj;

static struct xxx_obj *aa_obj, *bb_obj, *cc_obj;


static ssize_t aa_show(
	struct xxx_obj *xxx_obj,
	struct xxx_attribute *xxx_attr,
	char *buf)
{
	return sprintf(buf,"%d\n", xxx_obj->aa);
}
static ssize_t aa_store(
	struct xxx_obj *xxx_obj,
	struct xxx_attribute *xxx_attr,
	const char *buf,
	size_t count
)
{
	int ret;
	ret = kstrtoint(buf, 10, &xxx_obj->aa);
	if(ret < 0) {
		return ret;
	}
	return count;
}


static struct xxx_attribute aa_attr = 
	__ATTR(aa, 0664, aa_show, aa_store);


static ssize_t xxx_attr_show(
	struct kobject *kobj,
	struct attribute *attr,
	char *buf
)
{
	struct xxx_attribute *xxx_attr;
	struct xxx_obj *xxx;
	xxx_attr = to_xxx_attr(attr);
	xxx = to_xxx_obj(kobj);
	if(!xxx_attr) {
		return -EIO;
	}
	return xxx_attr->show(xxx, xxx_attr, buf);
}


static ssize_t xxx_attr_store(
	struct kobject *kobj,
	struct attribute *attr,
	const char *buf, 
	size_t count
)
{
	struct xxx_obj *xxx;
	struct xxx_attribute *xxx_attr;
	xxx= to_xxx_obj(kobj);
	xxx_attr = to_xxx_attr(attr);
	if(!xxx_attr->store) {
		return -EIO;
	}
	return xxx_attr->store(xxx, xxx_attr, buf, count);
}

static struct sysfs_ops xxx_sysfs_ops = {
	.show = xxx_attr_show,
	.store = xxx_attr_store,
};

static struct attribute *xxx_default_attrs[] = {
	&aa_attr.attr,
	NULL,
};

static void xxx_release(struct kobject *kobj)
{
	struct xxx_obj *xxx;
	xxx = to_xxx_obj(kobj);
	kfree(xxx);
}


static struct kobj_type xxx_ktype =  {
	.sysfs_ops = &xxx_sysfs_ops,
	.release = xxx_release,
	.default_attrs = xxx_default_attrs,
};


static struct xxx_obj *create_xxx_obj(
	const char *name
)
{
	struct xxx_obj *xxx;
	int ret;
	xxx = kzalloc(sizeof(struct xxx_obj), GFP_KERNEL);
	if(xxx == NULL) {
		printk("kzalloc failed \n");
		return -ENOMEM;
	}
	xxx->kobj.kset = xxx_kset;
	ret = kobject_init_and_add(&xxx->kobj, &xxx_ktype, NULL, "%s", name);
	if(ret) {
		kobject_put(&xxx->kobj);
		return NULL;
	}
	kobject_uevent(&xxx->kobj, KOBJ_ADD);
	return xxx;
}

static void destroy_xxx_obj(struct xxx_obj *xxx)
{
	kobject_put(&xxx->kobj);
}
static int xxx_kset_init(void)
{
	xxx_kset = kset_create_and_add("xxx_kset", NULL, kernel_kobj);
	if(!xxx_kset) {
		printk("kset_create_and_add failed \n");
		return -ENOMEM;
	}
	aa_obj = create_xxx_obj("aa");
	if(!aa_obj) {
		goto aa_error;
	}
	bb_obj = create_xxx_obj("bb");
	if(!bb_obj) {
		goto bb_error;
	}
	cc_obj = create_xxx_obj("cc");
	if(!cc_obj) {
		goto cc_error;
	}
	return 0;
cc_error:
	destroy_xxx_obj(bb_obj);
bb_error:
	destroy_xxx_obj(aa_obj);
aa_error:
	kset_unregister(xxx_kset);
	return -1;
}

static void xxx_kset_exit(void)
{
	destroy_xxx_obj(cc_obj);
	destroy_xxx_obj(bb_obj);
	destroy_xxx_obj(aa_obj);
	kset_unregister(xxx_kset);
}


module_init(xxx_kset_init);
module_exit(xxx_kset_exit);
MODULE_AUTHOR("teddyxiong53 <1073167306@qq.com>");
MODULE_LICENSE("Dual BSD/GPL");
