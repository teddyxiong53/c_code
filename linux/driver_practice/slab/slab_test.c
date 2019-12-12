#include <linux/module.h>
#include <linux/slab.h>

struct slab_test {
    int val;
};
struct kmem_cache *test_cachep = NULL;
void fun_ctor(void  *object)
{
    struct slab_test *tmp = (struct slab_test *)object;
    printk(KERN_INFO "ctor function ...\n");
    tmp->val = 1;
}

static  int __init slab_init(void)
{
    struct slab_test *object = NULL;
    test_cachep = kmem_cache_create("test_cachep", sizeof(struct slab_test)*3, 0, SLAB_HWCACHE_ALIGN, fun_ctor);
    if(test_cachep == NULL) {
        printk(KERN_ERR "kmem_cache_create fail\n");
        return -ENOMEM;
    }
    // printk(KERN_INFO "cache name is :%s\n", kmem_cache_name(test_cachep));
    printk(KERN_INFO "cache object size is: %d\n", kmem_cache_size(test_cachep));
    object = kmem_cache_alloc(test_cachep, GFP_KERNEL);//
    if(object) {
        printk(KERN_INFO "alloc one val = %d\n", object->val);
        kmem_cache_free(test_cachep, object);
        printk(KERN_INFO "after free val=%d\n", object->val);
        object = NULL;
    } else {
        printk(KERN_ERR "kmem_cache_alloc fail\n");
        return -ENOMEM;
    }
    return 0;
}

static  void __exit slab_exit(void)
{
    printk(KERN_INFO "slab exit\n");
    if(test_cachep) {
        kmem_cache_destroy(test_cachep);
    }
}

module_init(slab_init);
module_exit(slab_exit);
MODULE_LICENSE("GPL");

