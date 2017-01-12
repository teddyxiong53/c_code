#include <linux/module.h>

#include <linux/proc_fs.h>


static struct proc_dir_entry *proc_sample;
#define BUFSIZE 256
static char sample_buf[256];

static int proc_sample_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len;
	len = sprintf(page, "%s\n", "xhl proc sample");
	return len;
}
static int proc_sample_write_proc(struct file *file, const char __user *buf, unsigned long count, void *data)
{
	if(count > 256)
	{
		count = 256;
	}
	if(copy_from_user(sample_buf, buf, count))
	{
		return -EFAULT;
	}
	return count;
}

static int proc_sample_init()
{
	int ret = 0;
	proc_sample = create_proc_entry("proc_sample", 0644, NULL);
	if(proc_sample == NULL)
	{
		ret = -ENOMEM;
		goto err;
		
	}
	sample->read_proc = proc_sample_read_proc;
	sample->write_proc = proc_sample_write_proc;
	
	return 0;
err:
	return ret;
}

static void proc_sample_exit()
{
	remove_proc_entry("proc_sample", NULL);
	
}
module_init(proc_sample_init);
module_exit(proc_sample_exit);