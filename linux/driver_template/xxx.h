#ifndef __XXX_H__
#define __XXX_H__

#include <linux/cdev.h>
#include <linux/semaphore.h>

struct xxx_dev_struct {
	int val;
	struct semaphore sem;
	struct cdev dev;
};

#endif