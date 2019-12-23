#ifndef _PCM_BUF_POOL_H_
#define _PCM_BUF_POOL_H_
#include <linux/list.h>

struct pcm_buf_item {
    struct list_head entry;
    char *data;
    int len;
};

struct pcm_buf_pool {
    int max_size;
    struct pcm_buf_item* items;
    int (*init)(int max_size);
    int (*enqueue)(struct pcm_buf_item *item);
    struct pcm_buf_item* (*dequeue)();
};

#endif
