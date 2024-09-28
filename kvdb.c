#include "kvdb.h"

static int major;
static int minor;
static dev_t devid;

static struct cdev kvdb_cdev;
static struct device *kvdb_device;

static struct file_operations kvdb_fops;

int kvdb_create(struct class *class)
{
    int ret;

    pr_debug("flashdb: kvdb create.\n");

    // 申请设备号
    ret = alloc_chrdev_region(&devid, 0, 1, KVDB_NODE_NAME);
    if (ret < 0)
    {
        pr_err("flashdb: alloc kvdb region failed!\r\n");
        return -EFAULT;
    }

    major = MAJOR(devid);
    minor = MINOR(devid);

    pr_debug("flashdb: kvdb major = %d, minor = %d\r\n", major, minor);

    // 注册字符设备驱动
    kvdb_cdev.owner = THIS_MODULE;
    cdev_init(&kvdb_cdev, &kvdb_fops);

    // 注册字符设备
    ret = cdev_add(&kvdb_cdev, devid, 1);
    if (ret < 0)
    {
        pr_err("flashdb: kvdb cdev add failed!\r\n");
        goto del_unregister;
    }

    // 创建设备
    kvdb_device = device_create(class, NULL, devid, NULL, KVDB_NODE_NAME);
    if (IS_ERR(kvdb_device))
    {
        pr_err("flashdb: kvdb device create failed!\r\n");
        goto del_unregister;
    }

    return 0;

del_unregister:
    unregister_chrdev_region(devid, 1);

    return ret;
}

void kvdb_destory(struct class *class)
{
    pr_debug("flashdb: kvdb destory.\n");

    device_destroy(class, devid);
    cdev_del(&kvdb_cdev);
    unregister_chrdev_region(devid, 1);
}

void *kvdb_malloc(size_t size)
{
    if (kvdb_device)
        return devm_kmalloc(kvdb_device, size, GFP_KERNEL);

    return NULL;
}

static int _open(struct inode *inode, struct file *filp)
{
    pr_info("kvdb open\r\n");
    return 0;
}

static ssize_t _read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    int ret = 0;
    pr_info("kvdb read\r\n");
    return ret;
}

static ssize_t _write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
    int ret = 0;
    pr_info("kvdb write\r\n");
    return ret;
}

static int _release(struct inode *inode, struct file *filp)
{
    pr_info("kvdb release\r\n");
    return 0;
}

static struct file_operations kvdb_fops = {
    .owner = THIS_MODULE,
    .open = _open,
    .read = _read,
    .write = _write,
    .release = _release,
};
