#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

MODULE_AUTHOR("STARKING");
MODULE_LICENSE("GPL");

int nvmd_init(void)
{
	printk("nvmd initialization!\n");
	return 0;
}

void nvmd_exit(void)
{
	printk("nvmd exit!\n");
}

module_init(nvmd_init);
module_exit(nvmd_exit);
