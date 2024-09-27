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

#include "common.h"

static int major;
static int minor;
static dev_t devid;

static struct cdev flashdb_cdev;

static struct class *class;
static struct device *device;

static struct file_operations flashdb_fops;

void *flashdb_malloc(size_t size)
{
    if (device)
        return devm_kmalloc(device, size, GFP_KERNEL);

    return NULL;
}

static int __init _driver_init(void)
{
    int ret;

    pr_info("flashdb init.\n");
    pr_info("part: %s\n", _part_name);

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

    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class))
    {
        pr_err("flashdb: class create failed!\r\n");
        goto del_cdev;
    }

    device = device_create(class, NULL, devid, NULL, DEV_NAME);
    if (IS_ERR(device))
    {
        pr_err("flashdb: device create failed!\r\n");
        goto destroy_class;
    }

    ret = fal_init();
    if (ret < 0) {
        pr_err("fal init failed!\r\n");
        goto destroy_device;
    }

    return 0;

destroy_device:
    device_destroy(class, devid);

destroy_class:
    class_destroy(class);

del_cdev:
    cdev_del(&flashdb_cdev);

del_unregister:
    unregister_chrdev_region(devid, 1);

    return -EFAULT;
}

static void __exit _driver_exit(void)
{
    pr_info("flashdb exit.\n");

    // 销毁设备
    device_destroy(class, devid);

    // 销毁类
    class_destroy(class);

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

char _part_name[FAL_DEV_NAME_MAX] = DEFAULT_PART_NAME;
module_param_string(part, _part_name, FAL_DEV_NAME_MAX, 0);
MODULE_PARM_DESC(part, "Partition name of flashdb (Default: flashdb).");

int _part_size = (32 * 1024);
module_param_named(size, _part_size, int, 0444);
MODULE_PARM_DESC(size, "Partition size of memblk (Default: 32K).");

static int _open(struct inode *inode, struct file *filp)
{
    pr_info("flashdb open\r\n");
    return 0;
}

static ssize_t _read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    int ret = 0;
    pr_info("flashdb read\r\n");
    return ret;
}

static ssize_t _write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
    int ret = 0;
    pr_info("flashdb write\r\n");
    return ret;
}

static int _release(struct inode *inode, struct file *filp)
{
    pr_info("flashdb release\r\n");
    return 0;
}

static struct file_operations flashdb_fops = {
    .owner = THIS_MODULE,
    .open = _open,
    .read = _read,
    .write = _write,
    .release = _release,
};
