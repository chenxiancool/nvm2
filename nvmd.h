#ifndef PMBD_HEAD
#define PMBD_HEAD

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

struct nvmd{
	struct cdev cdev;
	long count;
};
//the layout on PM device, for better allocation granularity, we choose 4KB as the allocation unit.
//global metadata block
struct gmb{
	char name[24]; //name and verison, such as "PMBD V0.1"
	unsigned long size; //the size of PM
	unsigned long free_size; //free size of PM
	unsigned long disks; //the number of disks in the PM device
	unsigned long data_start; //the start address of data area
	unsigned long sat_start; //the start address of SAP area
	
};


//By default, only 63 disks can be allocated from one PM device.
//SCM allocation table
struct sat{
	char name[24];//name of the disk
	unsigned long flag; //indicate the features of this disk
	unsigned long size; //the size of this disk
	unsigned long map_address;//address of the mapping table
	unsigned long reserved1;//reserved 
	unsigned long reserved2; //reserved
};




#endif
