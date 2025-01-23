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
#include "kvdb.h"

#ifndef DEFAULT_KVDB_PART
#define DEFAULT_KVDB_PART "kvdb"
#endif

char param_kvdb[FAL_DEV_NAME_MAX] = DEFAULT_KVDB_PART;
module_param_string(kvdb, param_kvdb, FAL_DEV_NAME_MAX, 0);
MODULE_PARM_DESC(kvdb, "Partition name of kvdb (Default: " DEFAULT_KVDB_PART ").");

static struct class *class;

/**
 * flashdb类的partition属性, 用于打印分区表
 */
ssize_t fal_show_part_table_buf(char *buf, size_t len);
static ssize_t partition_show(struct class *class, struct class_attribute *attr,
                              char *buf)
{
    return fal_show_part_table_buf(buf, PAGE_SIZE);
}
static CLASS_ATTR_RO(partition);

/**
 * 创建flashdb类
 */
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

/**
 * 打印模块参数
 */
static inline void _print_params(void)
{
    pr_info("flashdb: param_kvdb = %s\r\n", param_kvdb);
}

static int __init _driver_init(void)
{
    int ret;

    pr_info("flashdb: init.\n");

    _print_params();

    // 创建类
    class = flashdb_create_class();
    if (!class)
    {
        pr_err("flashdb: create class failed!\r\n");
        return -EFAULT;
    }

    // 在flashdb类下创建kvdb对应的设备
    ret = kvdb_create(class);
    if (ret < 0)
    {
        pr_err("flashdb: kvdb register failed!\r\n");
        goto destroy_class;
    }

    pr_info("flashdb: init done.\n");

    return 0;

destroy_class:
    class_destroy(class);

    return -EFAULT;
}

static void __exit _driver_exit(void)
{
    pr_info("flashdb: exit.\n");

    kvdb_destory(class);

    // 销毁类
    class_destroy(class);
}

module_init(_driver_init);
module_exit(_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yhuan416 <yhuan416@foxmail.com>");
MODULE_DESCRIPTION("FlashDB LKM");
