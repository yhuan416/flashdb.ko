#include <fal.h>

#include <linux/mtd/mtd.h>

#include "common.h"

#define LOCKER_ENABLE

#ifdef LOCKER_ENABLE
static struct mutex g_mem_blk_mutex;
#define LOCK()                        \
    do                                \
    {                                 \
        mutex_lock(&g_mem_blk_mutex); \
    } while (0)

#define UNLOCK()                        \
    do                                  \
    {                                   \
        mutex_unlock(&g_mem_blk_mutex); \
    } while (0)
#else
#define LOCK()
#define UNLOCK()
#endif

struct mtd_info *kvdb_mtd = NULL;
struct fal_flash_dev nor_flash0;

/**
 * @brief 初始化flash设备
 *
 * @return int
 */
static int _init(void)
{
    struct mtd_info *mtd = NULL;
    struct mtd_info *master = NULL;

#ifdef LOCKER_ENABLE
    // create a mutex
    mutex_init(&g_mem_blk_mutex);
#endif

    mtd = get_mtd_device_nm(param_part_name);
    if (IS_ERR(mtd))
    {
        pr_err("get_mtd_device_nm fail.\n");
        return 0;
    }

    pr_debug("mtd device found: %s\n", mtd->name);
    pr_debug("\t size: %llu\n", mtd->size);
    pr_debug("\t erasesize: %u\n", mtd->erasesize);
    pr_debug("\t writesize: %u\n", mtd->writesize);
    pr_debug("\t offset: %llu\n", mtd_get_master_ofs((struct mtd_info *)mtd, 0));
    master = mtd_get_master(mtd);

    // mtd设备直接作为flash设备管理
    nor_flash0.len = mtd->size;
    nor_flash0.blk_size = mtd->erasesize;
    snprintf(nor_flash0.name, FAL_DEV_NAME_MAX, "%s@%d", master->name, mtd->index);

    // 保存mtd信息
    kvdb_mtd = mtd;

    return 0;
}

/**
 * @brief 读操作
 *
 * @param offset
 * @param buf
 * @param size
 * @return int
 */
static int _read(long offset, uint8_t *buf, size_t size)
{
    int ret = 0;
    struct mtd_info *mtd = kvdb_mtd;

    // pr_debug("_read offset: %ld, size: %zu\n", offset, size);

    LOCK();

    ret = mtd_read(mtd, offset, size, &size, buf);
    if (ret < 0)
    {
        pr_err("mtd_read fail.\n");
        ret = -1;
    }

    UNLOCK();

    return ret;
}

/**
 * @brief 写操作
 *
 * @param offset
 * @param buf
 * @param size
 * @return int
 */
static int _write(long offset, const uint8_t *buf, size_t size)
{
    int ret = 0;
    struct mtd_info *mtd = kvdb_mtd;

    // pr_debug("_write offset: %ld, size: %zu\n", offset, size);

    LOCK();

    ret = mtd_write(mtd, offset, size, &size, buf);
    if (ret < 0)
    {
        pr_err("mtd_write fail.\n");
        ret = -1;
    }

    UNLOCK();
    return ret;
}

/**
 * @brief 擦除操作
 *
 * @param offset
 * @param size
 * @return int
 */
static int _erase(long offset, size_t size)
{
    int ret = 0;
    struct mtd_info *mtd = kvdb_mtd;
    struct erase_info *erase;

    // pr_debug("_erase offset: %ld, size: %zu\n", offset, size);

    erase = kzalloc(sizeof(struct erase_info), GFP_KERNEL);
    if (!erase)
        ret = -ENOMEM;

    LOCK();

    erase->addr = offset;
    erase->len = size;

    ret = mtd_erase(mtd, erase);
    if (ret < 0)
    {
        pr_err("mtd_erase fail.\n");
        ret = -1;
    }

    UNLOCK();

    if (erase)
    {
        kfree(erase);
        erase = NULL;
    }

    return ret;
}

struct fal_flash_dev nor_flash0 = {
    .name = NOR_FLASH_DEV_NAME,
    .addr = 0x0,   // address is relative to beginning of partition; 0x0 is start of the partition
    .len = 0,      // size of the partition as specified in partitions.csv
    .blk_size = 0, // must be 4096 bytes
    .ops = {_init, _read, _write, _erase},
    .write_gran = 1, // 1 byte write granularity
};

int fal_flash_nor_flash_detect(fal_partition_t parts)
{
    // 没有找到分区
    if (nor_flash0.len == 0)
    {
        pr_err("fal_flash_nor_flash_detect: no partition found.\n");
        return 1;
    }

    // store partition info
    parts[0].magic_word = FAL_PART_MAGIC_WORD;
    strcpy(parts[0].flash_name, nor_flash0.name);
    strcpy(parts[0].name, "kvdb");
    parts[0].offset = 0;
    parts[0].len = nor_flash0.len;

    return 0;
}
