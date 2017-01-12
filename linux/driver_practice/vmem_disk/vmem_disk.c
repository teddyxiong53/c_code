


static int vmem_disk_major = 0;
module_param(vmem_disk_major, int, 0);
static int hardsect_size = 512;
module_param(hardsect_size, int ,0);
static int nsectors = 1024;
module_param(nsectors, int, 0);
static int ndevices = 4;
module_param(ndevices, int, 0);


enum {
	RM_SIMPLE = 0,
	RM_FULL = 1,
	RM_NOQUEUE = 2,
};

static int request_mode = RM_SIMPLE;
module_param(request_mode, int, 0);

#define vmem_disk_MINORS 16
#define MINOR_SHIFT 4
#define DEVNUM(kdevnum)   (MINOR(kdev_t_to_nr(kdevnum)) >> MINOR_SHIFT


#define KERNEL_SECTOR_SIZE 512
#define INVALIDATE_DELAY 3*HZ


struct vmem_disk_dev 
{
	int size;
	u8 *data;
	short users;
	short media_change;
	spinlock_t lock;
	struct request_queue *queue;
	struct gendisk *gd;
	struct timer_list timer;
};

static struct vmem_disk_dev *Devices = NULL;


static void vmem_disk_transfer(struct vmem_disk_dev *dev, unsigned long sector,
	unsigned long nsect, char *buffer, int write)
{
	unsigned long offset = sector*KERNEL_SECTOR_SIZE;
	unsigned long nbytes = nsect*KERNEL_SECTOR_SIZE;
	
	if((offset + nbytes) > dev->size )
	{
		printk(KERN_NOTICE "beyond the end of disk \n");
		return;
	}
	if(write)
	{
		memcpy(dev->data + offset, buffer, nbytes);
	}
	else
	{
		memcpy(buffer, dev->data + offset, nbytes);
	}
	
}

static void vmem_disk_request(struct request_queue *q)
{
	struct request *req;
	while((req = elv_next_request(q) ) != NULL )
	{
		struct vmem_disk_dev *dev = req->rq_disk->private_data;
		if(!blk_fs_request(req))
		{
			printk(KERN_NOTICE "skip non fs request \n");
			end_request(req, 0);
			continue;
		}
		vmem_disk_transfer(dev, req->sector, req->current_nr_sectors, 
			req->buffer, rq_data_dir(req));
		end_request(req, 1);
	}
	
}
static int vmem_disk_xfer_bio(struct vmem_disk_dev *dev, struct bio *bio)
{
	int i;
	struct bio_vec *bvec;
	sector_t sector = bio->bi_sector;
	bio_for_each_segment(bvec, bio, i)
	{
		char *buffer = __bio_kmap_atomic(bio, i, KM_USER0);
		vmem_disk_transfer(dev, sector, bio_cur_sectors(bio), buffer, bio_data_dir(bio)==WRITE );
		sector += bio_cur_sectors(bio);
		return 0;
	}
}

static int vmem_disk_xfer_request(struct vmem_disk_dev *dev, struct request *req)
{
	struct req_iterator iter;
	int nsect = 0;
	struct bio_vec *bvec;
	rq_for_each_segment(bvec, req, iter)
	{
		char *buffer = __bio_kmap_atomic(iter.bio, iter.i, KM_USER0);
		sector_t sector = iter.bio->bi_sector;
		vmem_disk_transfer(dev, sector, )
	}
}

static int vmem_disk_make_request(struct request_queue *q, struct bio *bio)
{
	struct vmem_disk_dev *dev = q->queuedata;
	int status;
	status = vmem_disk_xfer_bio(dev, bio);
	bio_endio(bio, status);
	return 0;
}
static int vmem_disk_open(struct block_device *bdev, fmode_t mode)
{
	struct vmem_disk_dev *dev = bdev->bd_disk>private_data;
	del_timer_sync(&dev->timer);
	spin_lock(&dev->lock);
	dev->users++;
	spin_unlock(&dev->lock);
	return 0;
}

static int vmem_disk_release(struct gendisk * disk , fmode_t mode)
{
	struct vmem_disk_dev *dev = disk->private_data;
	spin_lock(&dev->lock);
	dev->users --;
	if(!dev->users) 
	{
		dev->timer.expires = jiffies + INVALIDATE_DELAY;
		add_timer(&dev->timer);
	}
	spin_unlock(&dev->lock);
	return 0;
}


static void vmem_disk_invalidate(unsigned long ldev)
{
	struct vmem_disk_dev *dev = (struct vmem_disk_dev *)ldev;
	spin_lock(&dev->lock);
	if(dev->users || !dev->data)
	{
		printk(KERN_WARNING "vmem disk: timer check failed \n");
	}
	else
	{
		dev->media_change = 1;
	}
	spin_unlock(&dev->lock);
}

static int vmem_disk_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	long size;
	struct vmem_disk_dev *dev = bdev->bd_disk->private_data;
	size = dev->size *(hardsect_size/KERNEL_SECTOR_SIZE);
	geo->cylinders = (size&~0x3f)>>6;
	geo->heads = 4;
	geo->sectors = 16;
	geo->start = 4;
	return 0;
}



static int vmem_disk_revalidate(struct gendisk * gd)
{
	struct vmem_disk_dev *dev = gd->private_data;
	if(dev->media_change)
	{
		dev->media_change = 0;
		memset(dev->data, 0, dev->data);
	}
	return 0;
}


static int vmem_disk_media_changed(struct gendisk *gd)
{
	struct vmem_disk_dev *dev = gd->private_data;
	return dev->media_change;
}


static struct block_device_operations vmem_disk_ops =
{
	.owenr = THIS_MODULE,
	.open = vmem_disk_open,
	.release = vmem_disk_release,
	.media_changed = vmem_disk_media_changed,
	.revalidate_disk = vmem_disk_revalidate,
	.getgeo = vmem_disk_getgeo,
};



static void setup_device(struct vmem_disk_dev * dev, int which)
{
	memset(dev, 0, sizeof(struct vmem_disk_dev));
	dev->size = nsectors * hardsect_size;
	dev->data = vmalloc(dev->size);
	if(dev->data == NULL) 
	{
		printk(KERN_NOTICE "vmalloc failed \n");
		return ;
	}
	spin_lock_init(&dev->lock);
	init_timer(&dev->timer);
	dev->timer.data = (unsigned long )dev;
	dev->timer.function = vmem_disk_invalidate;
	switch(request_mode)
	{
		case RM_NOQUEUE:
			dev->queue = blk_alloc_queue(GFP_KERNEL);
			if(dev->queue == NULL) 
			{
				goto out_vfree;
			}				
			break;
	}
	blk_queue_hardsect_size(dev->queue, hardsect_size);
	dev->queue->queuedata = dev;
	
	dev->gd = alloc_disk(vmem_disk_MINORS);
	if(!dev->gd) 
	{
		printk(KERN_NOTICE "alloc disk failed \n");
		goto out_vfree;
	}
	dev->gd->major = vmem_disk_major;
	dev->gd->first_minor = which*vmem_disk_MINORS;
	dev->gd->fops = &vmem_disk_fops;
	dev->gd->queue = dev->queue;
	dev->gd->private_data = dev;
	snprintf(dev->gd->disk_name, 32, "vmem_disk%c", which+'a');
	set_capacity(dev->gd, nsectors*(hardsect_size/KERNEL_SECTOR_SIZE));
	add_disk(dev->gd);
	return;
out_vfree:
	if(dev->data)
	{
		vfree(dev->data);
	}
}
static int vmem_disk_init()
{
	int i;
	vmem_disk_major = register_blkdev(vmem_disk_major, "vmem_disk");
	if(vmem_disk_major <= 0) 
	{
		printk(KERN_WARNING "vmem_disk unable to get major number \n");
		return -EBUSY;
	}
	Devices = kmalloc(ndevices*sizeof(struct vmem_disk_dev), GFP_KERNEL);
	if(Devices == NULL)
	{
		goto out;
	}
	for(i=0; i<ndevices; i++) 
	{
		setup_device(Devices+i, i);
	}
	return 0;
out:
	unregister_blkdev(vmem_disk_major, "sbd");
	return -ENOMEM;
}

static void vmem_disk_exit()
{
	
}

module_init(vmem_disk_init);
module_exit(vmem_disk_exit);

