#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/dcache.h>

#include <linux/fcntl.h>
#include <linux/kfifo.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/serial_reg.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/debugfs.h>
#include <linux/cdev.h>

#include <linux/io.h>
#include <linux/ioport.h>

static int port = 0x3f8;
module_param(port, int, 0);
MODULE_PARM_DESC(port, "io port number, default 0x3f8");

static int port_size = 8;
module_param(port_size, int, 0);
MODULE_PARM_DESC(port_size, "io port size, default 8");


static int irq = 4;
module_param(irq , int, 0);
MODULE_PARM_DESC(irq , "irq nubmer,  default 4(uart)");

static int loopback = 0;
module_param(loopback , int, 0);
MODULE_PARM_DESC(loopback , "loopback mode for testing,default 0");


#define FIFO_SIZE 128
static int bufsize = 8*FIFO_SIZE;

struct ldt_data {
    void *in_buf;
    void *out_buf;
    DECLARE_KFIFO(in_fifo, char, FIFO_SIZE);
    DECLARE_KFIFO(out_fifo, char, FIFO_SIZE);
    spinlock_t fifo_lock;
    wait_queue_head_t readable, writeable;
    struct mutex read_lock, write_lock;
    void *port_ptr;
    int uart_detected;
};

static struct ldt_data *drvdata;
static int isr_counter = 0;
static void ldt_tasklet_func(unsigned long d);

static DECLARE_TASKLET(ldt_tasklet, ldt_tasklet_func, 0);


static irqreturn_t ldt_isr(int irq, void *dev_id)
{
	isr_counter ++;
	tasklet_schedule(&ldt_tasklet);
	return IRQ_HANDLED;
}


static struct dentry *debugfs;
static void ldt_tasklet_func(unsigned long d);


static void ldt_timer_func(unsigned long data);

static DEFINE_TIMER(ldt_timer, ldt_timer_func, 0, 0);

static void ldt_timer_func(unsigned long data)
{
    if(loopback) {
        tasklet_schedule(&ldt_tasklet);
    }
    mod_timer(&ldt_timer, jiffies +HZ/100);
    
}


static inline u8 tx_ready(void)
{
	return ioread8(drvdata->port_ptr+UART_LSR)&UART_LSR_THRE;
}

static inline u8 rx_ready(void)
{
	return ioread8(drvdata->port_ptr+UART_LSR)&UART_LSR_DR;
}

static void ldt_received(char data)
{
	kfifo_in_spinlocked(&drvdata->in_fifo, &data,
		sizeof(data), &drvdata->fifo_lock);
	wake_up_interruptible(&drvdata->readable);
}

static void ldt_send(char data)
{
	if(drvdata->uart_detected) {
		iowrite8(data, drvdata->port_ptr+UART_TX);
	} else {
		if(loopback) {
			ldt_received(data);
		}
	}
}
static void ldt_tasklet_func(unsigned long d)
{
	char data_out, data_in;
	if(drvdata->uart_detected) {
		while(tx_ready()
			&& kfifo_out_spinlocked(&drvdata->out_fifo, 
				&data_out, sizeof(data_out), 
				&drvdata->fifo_lock))
		{
			wake_up_interruptible(&drvdata->writeable);
			printk("data_out:%c\n", data_out);
			ldt_send(data_out);
		}
		while(rx_ready()) {
			data_in = ioread8(drvdata->port_ptr+UART_RX);
			printk("data_in:%c\n", data_in);
			ldt_received(data_in);
		}
	} else {
		while(kfifo_out_spinlocked(
			&drvdata->out_fifo, &data_out,sizeof(data_out), &drvdata->fifo_lock)) {
			wake_up_interruptible(&drvdata->writeable);
			ldt_send(data_out);
		}
	}
}
static struct ldt_data *ldt_data_init(void)
{
    struct ldt_data *drvdata;
    drvdata = kzalloc(sizeof(*drvdata), GFP_KERNEL);
    if(!drvdata) {
        return NULL;
    }
    init_waitqueue_head(&drvdata->readable);
    init_waitqueue_head(&drvdata->writeable);
    INIT_KFIFO(drvdata->in_fifo);
    INIT_KFIFO(drvdata->out_fifo);
    mutex_init(&drvdata->read_lock);
    mutex_init(&drvdata->write_lock);
    return drvdata;
}

static void pages_flag
    (struct page *page, 
    int page_num,
    int mask,
    int value)
{
    for(;page_num; page_num--, page++) {
        if(value) {
            __set_bit(mask, &page->flags);
        } else {
            __clear_bit(mask, &page->flags);
        }
    }
}
static int uart_probe(void)
{
    int ret = 0;
    if(port) {
        drvdata->port_ptr = ioport_map(port, port_size);
        if(!drvdata->port_ptr) {
            printk("ioport_map failed\n");
            return -ENODEV;
        }
    }

    if(!irq || !drvdata->port_ptr) {
        goto exit;
    }
    ret = request_irq(irq, ldt_isr, IRQF_SHARED, "ldt-uart", THIS_MODULE);
    if(ret < 0) {
        printk("request_irq failed \n");
        goto exit;
    }
    iowrite8(UART_MCR_RTS|UART_MCR_OUT2|UART_MCR_LOOP, drvdata->port_ptr+UART_MCR);
    drvdata->uart_detected = ((ioread8(drvdata->port_ptr+UART_MSR) &0XF0) == (UART_MSR_DCD|UART_MSR_CTS));

    if(drvdata->uart_detected) {
        iowrite8(UART_IER_RDI | UART_IER_RLSI | UART_IER_THRI, drvdata->port_ptr+UART_IER);
        iowrite8(UART_MCR_DTR | UART_MCR_RTS | UART_MCR_OUT2, drvdata->port_ptr+UART_MCR);
        iowrite8(UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT, drvdata->port_ptr+UART_FCR);
        printk("loopback:%d \n", loopback);
        if(loopback) {
            iowrite8(ioread8(drvdata->port_ptr+UART_MCR)|UART_MCR_LOOP, drvdata->port_ptr+UART_MCR);
        }
    }     
    if(!drvdata->uart_detected && loopback) {
        printk("emulating loopback in software \n");
    }
    
exit:
    return ret;
}

static int ldt_open(struct inode *inode, struct file *file)
{
    printk("opened by %s \n", current->comm);
    return 0;
}

static int ldt_release(struct inode *inode, struct file *file)
{
    printk("closed by %s \n", current->comm);
    return 0;
}


static ssize_t ldt_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    int ret = 0;
	unsigned int copied = 0;
	printk("read from %s \n", current->comm);
    if(kfifo_is_empty(&drvdata->in_fifo)) {
        if(file->f_flags & O_NONBLOCK) {
            return -EAGAIN;
        } else {
            printk("now the in fifo is empty, now wait\n");
            ret = wait_event_interruptible(drvdata->readable, !kfifo_is_empty(&drvdata->in_fifo));
            if(ret == -ERESTARTSYS) {
                printk("interrupted \n");
                return -EINTR;
            }
        }
        
    }
    if(mutex_lock_interruptible(&drvdata->read_lock)) {
         return -EINTR;
    }
	printk("can read now \n");
	ret = kfifo_to_user(&drvdata->in_fifo, buf, count, &copied);
	mutex_unlock(&drvdata->read_lock);
	return ret?ret:copied;
	

}

static ssize_t ldt_write(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	int ret =0;
	unsigned int copied;
	printk("write from %s \n", current->comm);
	if(kfifo_is_full(&drvdata->out_fifo)) {
		if(file->f_flags & O_NONBLOCK) {
			return -EAGAIN;
		}
		printk("now the out fifo is full, please wait\n");
		ret = wait_event_interruptible(drvdata->writeable, !kfifo_is_full(&drvdata->out_fifo));
		if(ret == -ERESTARTSYS) {
			printk("interrupted \n");
			return -EINTR;
		}
		
	}
	if(mutex_lock_interruptible(&drvdata->write_lock)) {
		return -EINTR;
	}
	printk("now write\n");
	ret = kfifo_from_user(&drvdata->out_fifo, buf, count, &copied);
	mutex_unlock(&drvdata->write_lock);
	tasklet_schedule(&ldt_tasklet);
	if(ret < 0) {
		return ret;
	}
	return copied;
}

static unsigned int ldt_poll(struct file *file, poll_table *pt)
{
	unsigned int mask = 0;
	poll_wait(file, &drvdata->readable, pt);
	poll_wait(file, &drvdata->writeable, pt);

	if(!kfifo_is_empty(&drvdata->in_fifo)) {
		mask |= POLLIN | POLLRDNORM;
	}
	mask |= POLLOUT | POLLWRNORM;
	return mask;
}

static int ldt_mmap(struct file *file, struct vm_area_struct *vma)
{
	void *buf = NULL;
	if(vma->vm_flags & VM_WRITE) {
		buf = drvdata->in_buf;
	} else if(vma->vm_flags & VM_READ) {
		buf = drvdata->out_buf;
	}
	if(!buf) {
		return -EINVAL;
	}
	int ret = 0;
	ret = remap_pfn_range(vma, vma->vm_start, virt_to_phys(buf)>>PAGE_SHIFT, vma->vm_end-vma->vm_start, vma->vm_page_prot);
	if(ret < 0) {
		printk("remap pfn failed \n");
		return -EAGAIN;
	}
	return 0;
}

static long ldt_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}


static struct file_operations ldt_fops = {
    .owner = THIS_MODULE,
    .open = ldt_open,
    .release = ldt_release,
    .read = ldt_read,
    .write = ldt_write,
    .poll =ldt_poll,
    .mmap = ldt_mmap,
    .unlocked_ioctl = ldt_ioctl,
};

static struct miscdevice ldt_miscdev = {
    .name = KBUILD_MODNAME,
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &ldt_fops,
};
static void ldt_cleanup(void);

static  int ldt_init(void)
{
    int ret = 0;

    drvdata = ldt_data_init();
    if(!drvdata) {
        printk(KERN_ERR "ldata init failed\n");
        goto exit;
    }
    drvdata->in_buf = alloc_pages_exact(bufsize, GFP_KERNEL|__GFP_ZERO);
    if(!drvdata->in_buf) {
        ret = -ENOMEM;
        goto exit;
    }
    pages_flag(virt_to_page(drvdata->in_buf), PFN_UP(bufsize), PG_reserved, 1);
    drvdata->out_buf = alloc_pages_exact(bufsize, GFP_KERNEL|__GFP_ZERO);
    if(!drvdata->out_buf) {
        ret = -ENOMEM;
        goto exit;
    }
    
    pages_flag(virt_to_page(drvdata->out_buf), PFN_UP(bufsize), PG_reserved, 1);

    isr_counter = 0;
    ret = uart_probe();
    if(ret < 0) {
        printk("uart probe failed\n");
        goto exit;
    }

    mod_timer(&ldt_timer, jiffies+HZ/10);
    //debugfs = debugfs_create_file(KBUILD_MODNAME, S_IRUGO, NULL, NULL, &ldt_fops);
	#if 0
	if(IS_ERR(debugfs)) {
        ret = PTR_ERR(debugfs);
        printk("debugfs create file failed \n");
        goto exit;
    }
	#endif
    ret = misc_register(&ldt_miscdev);
    if(ret<0) {
        printk("misc register failed\n");
        goto exit;
    }
    
exit:
    if(ret <0) {
        ldt_cleanup();
    }
    return ret;
}


static void ldt_cleanup(void)
{
    //debugfs_remove(debugfs);
	if(ldt_miscdev.this_device) {
		misc_deregister(&ldt_miscdev);
	}
	del_timer(&ldt_timer);
	if(irq) {
		if(drvdata->uart_detected) {
			iowrite8(0, drvdata->port_ptr+UART_IER);
			iowrite8(0, drvdata->port_ptr+UART_MCR);
			iowrite8(0, drvdata->port_ptr+UART_FCR);
			ioread8(drvdata->port_ptr+UART_RX);
		}
		free_irq(irq, THIS_MODULE);
	}
	tasklet_kill(&ldt_tasklet);
	if(drvdata->in_buf) {
		pages_flag(virt_to_page(drvdata->in_buf), PFN_UP(bufsize), PG_reserved, 0);
		free_pages_exact(drvdata->in_buf, bufsize);
		
	}
	if(drvdata->out_buf) {
		pages_flag(virt_to_page(drvdata->out_buf), PFN_UP(bufsize), PG_reserved, 0);
		free_pages_exact(drvdata->out_buf, bufsize);
		
	}
	printk("isr counter :%d \n", isr_counter);
	if(drvdata->port_ptr) {
		ioport_unmap(drvdata->port_ptr);
	}
	kfree(drvdata);
}



module_init(ldt_init);
module_exit(ldt_cleanup);

MODULE_AUTHOR("teddyxiong53 <1073167306@qq.com>");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("this is a linux driver template");

