#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h> //contains dev_t type
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/mm.h>


#define NVMD_MAJOR 255

MODULE_AUTHOR("STARKING");
MODULE_LICENSE("GPL");

struct nvmd{
	struct cdev cdev;
	long count;
};

static long phy_start;
static struct nvmd *nvmdp;
static struct class *module_class;
static struct device *module_class_dev;

static int nvmd_open(struct inode *inode, struct file *file)
{
	phy_start = (_AC(1, UL))<<33;
	printk("[DEBUG] nvmd is opened! PHY_START:%lu!\n", phy_start);
	return 0;
}

static int nvmd_mmap(struct file *file, struct vm_area_struct *vma)
{
	printk("[DEBUG] nvmd_mmap() is called !\n");
	vma->vm_flags |= VM_IO;
//	vma->vm_flags |= VM_RESERVED; //kernel 3.10 does not use this flag
	if(remap_pfn_range(vma, vma->vm_start, phy_start >> PAGE_SHIFT, vma->vm_end-vma->vm_start, vma->vm_page_prot))
	{
		printk("[ERROR] nvmd mmap error  !\n");
		return -EAGAIN;
	}
	printk("[DEBUG] nvmd mmap successed! PHY_START:%lu, VIRT_START:%p, SIZE: %lu !\n",phy_start >> PAGE_SHIFT, vma->vm_start, vma->vm_end-vma->vm_start);
	return 0;
}

//the latest kernel does not use ioctl, instead unlocked_ioctl() is used.
static struct file_operations nvmd_fops = {
	.owner = THIS_MODULE,
	.open = nvmd_open,
	.read = NULL,
	.write = NULL,
	.release = NULL,
	.llseek = NULL,
	.mmap = nvmd_mmap,
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
	
	//now the NVMD_MAJOR has been occupied by nvmd in /proc/devices.
	//alloc memory for nvmd struct.
	nvmdp = kmalloc(sizeof(struct nvmd), GFP_KERNEL);
	if(!nvmdp)
	{
		printk("can not malloc memory for struct nvmd!\n");
		goto fail_malloc;
	}	
	
	memset(nvmdp, 0, sizeof(struct nvmd));
	nvmd_setup_cdev(nvmdp, 0);

	module_class = class_create(THIS_MODULE, "module_drv");
	if(IS_ERR(module_class))
	{
		return PTR_ERR(module_class);
	}
	//note, class_device_create() is used before kernel version 2.6.29. But in newer version, device_create() is used instead.  		
	module_class_dev = device_create(module_class, NULL, nvmdno,NULL, "nvmd");	
	if(IS_ERR(module_class_dev))
	{
		return PTR_ERR(module_class_dev);
	}
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
	device_unregister(module_class_dev);
	class_destroy(module_class);
	printk("nvmd exit!\n");
}

module_init(nvmd_init);
module_exit(nvmd_exit);
