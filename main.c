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

#define DEV_MAJOR (0)      // 主设备号
#define DEV_NAME "flashdb" // 设备名

static int major;
static int minor;
static dev_t devid;

static struct cdev flashdb_cdev;

// static struct class *class;
// static struct device *device;

#ifndef DEFAULT_PART_NAME
#define DEFAULT_PART_NAME "flashdb"
#endif

char partition_name[FAL_DEV_NAME_MAX] = DEFAULT_PART_NAME;
module_param_string(part, partition_name, FAL_DEV_NAME_MAX, 0);

static int _open(struct inode *inode, struct file *filp)
{
    printk("flashdb open!\r\n");
    return 0;
}

static ssize_t _read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    int ret = 0;
    printk("flashdb read!\r\n");
    return ret;
}

static ssize_t _write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
    int ret = 0;
    printk("flashdb write!\r\n");
    return ret;
}

static int _release(struct inode *inode, struct file *filp)
{
    printk("flashdb release!\r\n");
    return 0;
}

static struct file_operations flashdb_fops = {
    .owner = THIS_MODULE,
    .open = _open,
    .read = _read,
    .write = _write,
    .release = _release,
};

static int __init _driver_init(void)
{
    int ret;

    printk(KERN_INFO "flashdb init\n");
    printk(KERN_INFO "partition_name: %s\n", partition_name);

    // 创建设备号
    if (DEV_MAJOR)
    {
        // 通过主次设备号创建设备号
        devid = MKDEV(DEV_MAJOR, 0);
        major = DEV_MAJOR;
        minor = 0;
    }
    else
    {
        // 申请设备号
        ret = alloc_chrdev_region(&devid, 0, 1, DEV_NAME);
        if (ret < 0)
        {
            pr_err("flashdb: alloc flashdb region failed!\r\n");
            return -EFAULT;
        }

        major = MAJOR(devid);
        minor = MINOR(devid);
    }

    pr_info("flashdb: major = %d, minor = %d\r\n", major, minor);

    // 注册字符设备驱动
    flashdb_cdev.owner = THIS_MODULE;
    cdev_init(&flashdb_cdev, &flashdb_fops);

    ret = cdev_add(&flashdb_cdev, devid, 1);
    if (ret < 0)
    {
        pr_err("flashdb: cdev add failed!\r\n");
        goto del_unregister;
    }

    return 0;

del_unregister:
    unregister_chrdev_region(devid, 1);
    return -EFAULT;
}

static void __exit _driver_exit(void)
{
    printk(KERN_INFO "flashdb exit\n");

    // 注销字符设备驱动
    cdev_del(&flashdb_cdev);

    // 释放设备号
    unregister_chrdev_region(devid, 1);
}

module_init(_driver_init);
module_exit(_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yhuan416 <yhuan416@foxmail.com>");
MODULE_DESCRIPTION("FlashDB LKM");
