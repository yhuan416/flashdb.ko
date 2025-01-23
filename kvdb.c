#include "kvdb.h"

#include "flashdb.h"

static int major;
static int minor;
static dev_t devid;

static struct cdev kvdb_cdev;
static struct device *kvdb_device;
static struct file_operations kvdb_fops;

static struct fdb_kvdb g_kvdb;

// 静态初始化一个 g_kvdb_mutex
static DEFINE_MUTEX(g_kvdb_mutex); // 定义并初始化 mutex

static uint32_t boot_count = 0;
static uint32_t boot_time[10] = {0, 1, 2, 3};
static struct fdb_default_kv_node default_kv_table[] = {
    {"username", "armink", 0},                       /* string KV */
    {"password", "123456", 0},                       /* string KV */
    {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
    {"boot_time", &boot_time, sizeof(boot_time)},    /* int array type KV */
};

static void g_kvdb_lock(fdb_db_t db)
{
    struct mutex *_mutex = (struct mutex *)db->user_data;
    mutex_lock(_mutex);
}

static void g_kvdb_unlock(fdb_db_t db)
{
    struct mutex *_mutex = (struct mutex *)db->user_data;
    mutex_unlock(_mutex);
}

void kvdb_sample(void)
{
#if defined(DEBUG) && defined(DEBUG_SAMPLE)
    extern void kvdb_basic_sample(fdb_kvdb_t kvdb);
    extern void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
    extern void kvdb_type_string_sample(fdb_kvdb_t kvdb);
    kvdb_basic_sample(&g_kvdb);
    kvdb_type_blob_sample(&g_kvdb);
    kvdb_type_string_sample(&g_kvdb);
#endif
}

static int kvdb_init(void)
{
    fdb_err_t result;
    struct fdb_default_kv default_kv;

    pr_debug("flashdb: kvdb init.\n");

    default_kv.kvs = default_kv_table;
    default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);

    fdb_kvdb_control(&g_kvdb, FDB_KVDB_CTRL_SET_LOCK, g_kvdb_lock);
    fdb_kvdb_control(&g_kvdb, FDB_KVDB_CTRL_SET_UNLOCK, g_kvdb_unlock);

    result = fdb_kvdb_init(&g_kvdb, "kvdb", param_kvdb, &default_kv, (void *)&g_kvdb_mutex);
    if (result != FDB_NO_ERR)
    {
        pr_err("flashdb: kvdb init failed!\r\n");
        return -1;
    }

    pr_info("flashdb: kvdb init success.\r\n");

    kvdb_sample();

    return 0;
}

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
        goto del_cdev_del;
    }

    // 初始化kvdb
    if (kvdb_init()) {
        pr_err("flashdb: kvdb init failed!\r\n");
        goto del_device_destroy;
    }

    return 0;

del_device_destroy:
    device_destroy(class, devid);

del_cdev_del:
    cdev_del(&kvdb_cdev);

del_unregister:
    unregister_chrdev_region(devid, 1);

    return -EFAULT;
}

int fal_flash_linux_port_close(void);
void kvdb_destory(struct class *class)
{
    pr_debug("flashdb: kvdb destory.\n");

    fal_flash_linux_port_close();

    fdb_kvdb_deinit(&g_kvdb);

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

void *kvdb_calloc(size_t n, size_t size)
{
    if (kvdb_device)
        return devm_kcalloc(kvdb_device, n, size, GFP_KERNEL);

    return NULL;
}

void *kvdb_realloc(void *ptr, size_t size)
{
    if (kvdb_device)
        return devm_krealloc(kvdb_device, ptr, size, GFP_KERNEL);

    return NULL;
}

void kvdb_free(void *ptr)
{
    if (kvdb_device)
        devm_kfree(kvdb_device, ptr);
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
