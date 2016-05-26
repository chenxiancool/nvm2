#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h> //contains dev_t type
#include <linux/cdev.h>
#include <linux/slab.h>

#define NVMD_MAJOR 255

struct nvmd{
	struct cdev cdev;
	long count;
};

struct nvmd *nvmdp;

MODULE_AUTHOR("STARKING");
MODULE_LICENSE("GPL");
//the latest kernel does not use ioctl, instead unlocked_ioctl() is used.
static struct file_operations nvmd_fops = {
	.owner = THIS_MODULE,
	.open = NULL,
	.read = NULL,
	.write = NULL,
	.release = NULL,
	.llseek = NULL,
	.mmap = NULL,
};
void nvmd_setup_cdev(struct nvmd *dev, int minorIndex)
{
	int err;
	int devno = MKDEV(NVMD_MAJOR, minorIndex);
	cdev_init(&dev->cdev, &nvmd_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev,devno,1);
	
	if(err)
	{
		printk("error %d cdev file added !\n", err);
	}
}

int nvmd_init(void)
{
	int result = 0;
	dev_t nvmdno;
	printk("nvmd initialization!\n");
	nvmdno = MKDEV(NVMD_MAJOR,0);
	result = register_chrdev_region(nvmdno,1,"nvmd");
	if(result < 0)
	{
		printk("register chardev failed!\n");
		return result;
	}
	
	//now the major number has been occupied by nvmd in /proc/devices.
	//alloc memory for nvmd struct.
	nvmdp = kmalloc(sizeof(struct nvmd), GFP_KERNEL);
	if(!nvmdp)
	{
		printk("can not malloc memory for struct nvmd!\n");
		goto fail_malloc;
	}	
	
	memset(nvmdp, 0, sizeof(struct nvmd));
	nvmd_setup_cdev(nvmdp, 0);

	return 0;
	
	fail_malloc:
		unregister_chrdev_region(nvmdno,1);
		return -ENOMEM;
}

void nvmd_exit(void)
{
	cdev_del(&nvmdp->cdev);
	kfree(nvmdp);
	unregister_chrdev_region(MKDEV(NVMD_MAJOR,0),1);
	printk("nvmd exit!\n");
}

module_init(nvmd_init);
module_exit(nvmd_exit);
