#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/err.h>

#include "fal.h"

#ifndef LINUX_KERNEL_VERSION
#define LINUX_KERNEL_VERSION 510
#endif

#ifndef DEFAULT_PART_NAME
#define DEFAULT_PART_NAME "flashdb"
#endif

char partition_name[FAL_DEV_NAME_MAX] = DEFAULT_PART_NAME;
module_param_string(part, partition_name, FAL_DEV_NAME_MAX, 0);

static int __init _driver_init(void)
{
    printk(KERN_INFO "flashdb init\n");
    printk(KERN_INFO "partition_name: %s\n", partition_name);

    return 0;
}

static void __exit _driver_exit(void)
{
    printk(KERN_INFO "flashdb exit\n");
}

module_init(_driver_init);
module_exit(_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yhuan416 <yhuan416@foxmail.com>");
MODULE_DESCRIPTION("FlashDB LKM");
