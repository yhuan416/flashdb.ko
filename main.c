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

ssize_t partition_show(struct class *class, struct class_attribute *attr,
                       char *buf)
{
    return fal_print_part_table(buf, PAGE_SIZE);
}
static CLASS_ATTR_RO(partition);

static struct class *flashdb_create_class(void)
{
    int ret;
    struct class *new_class = NULL;

    new_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class))
    {
        pr_err("flashdb: class create failed!\r\n");
        return NULL;
    }

    ret = class_create_file(new_class, &class_attr_partition);
    if (ret < 0)
    {
        pr_err("flashdb: class create file failed!\r\n");
        class_destroy(new_class);
        return NULL;
    }

    return new_class;
}

static void _print_params(void)
{
    pr_info("flashdb: part_name = %s, part_size = %d, mtd_name = %s\r\n",
            param_part_name, param_part_size, param_mtd_name);
}

static int __init _driver_init(void)
{
    int ret;

    pr_info("flashdb init.\n");

    _print_params();

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

    pr_debug("flashdb: major = %d, minor = %d\r\n", major, minor);

    // 注册字符设备驱动
    flashdb_cdev.owner = THIS_MODULE;
    cdev_init(&flashdb_cdev, &flashdb_fops);

    // 注册字符设备
    ret = cdev_add(&flashdb_cdev, devid, 1);
    if (ret < 0)
    {
        pr_err("flashdb: cdev add failed!\r\n");
        goto del_unregister;
    }

    // 创建类
    class = flashdb_create_class();
    if (!class)
    {
        pr_err("flashdb: create class failed!\r\n");
        goto del_cdev;
    }

    // 创建设备
    device = device_create(class, NULL, devid, NULL, DEV_NAME);
    if (IS_ERR(device))
    {
        pr_err("flashdb: device create failed!\r\n");
        goto destroy_class;
    }

    // 初始化fal
    ret = fal_init();
    if (ret < 0)
    {
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

void *flashdb_malloc(size_t size)
{
    if (device)
        return devm_kmalloc(device, size, GFP_KERNEL);

    return NULL;
}

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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yhuan416 <yhuan416@foxmail.com>");
MODULE_DESCRIPTION("FlashDB LKM");
