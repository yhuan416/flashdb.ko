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

struct fal_flash_dev nor_flash0;

static int _init(void)
{
    struct mtd_info *mtd = NULL;
    struct mtd_info *master = NULL;

#ifdef LOCKER_ENABLE
    // create a mutex
    mutex_init(&g_mem_blk_mutex);
#endif

    // init flash info
    mtd = get_mtd_device(NULL, 0);

    // 获取主节点信息
    master = mtd_get_master((struct mtd_info *)mtd);
    if (master)
    {
        nor_flash0.len = master->size;
        nor_flash0.blk_size = master->erasesize;
        strcpy(nor_flash0.name, master->name);
    }
    put_mtd_device(mtd);

    return 0;
}

static int _read(long offset, uint8_t *buf, size_t size)
{
    int ret = 0;
    LOCK();

    UNLOCK();
    return ret;
}

static int _write(long offset, const uint8_t *buf, size_t size)
{
    int ret = 0;
    LOCK();

    UNLOCK();
    return ret;
}

static int _erase(long offset, size_t size)
{
    int ret = 0;
    LOCK();

    UNLOCK();
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
    int i;
    int part_num = 0;
    struct mtd_info *mtd = NULL;
    struct mtd_info *master = NULL;

    for (i = 0; i < 64; i++)
    {
        mtd = get_mtd_device(NULL, i);
        if (!IS_ERR(mtd))
        {
            pr_debug("mtd device found: %s\n", mtd->name);
            pr_debug("\t size: %llu\n", mtd->size);
            pr_debug("\t erasesize: %u\n", mtd->erasesize);
            pr_debug("\t writesize: %u\n", mtd->writesize);
            pr_debug("\t offset: %llu\n", mtd_get_master_ofs((struct mtd_info *)mtd, 0));

            // param_mtd_name
            master = mtd_get_master((struct mtd_info *)mtd);
            if (strcmp(master->name, param_mtd_name) != 0)
            {
                pr_debug("skip mtd device: %s, part: %s\n", master->name, mtd->name);
                put_mtd_device(mtd);
                continue;
            }

            // store partition info
            strcpy(parts[part_num].flash_name, master->name);
            parts[part_num].magic_word = FAL_PART_MAGIC_WORD;
            strcpy(parts[part_num].name, mtd->name);
            parts[part_num].offset = mtd_get_master_ofs((struct mtd_info *)mtd, 0);
            parts[part_num].len = mtd->size;

            part_num++;

            put_mtd_device(mtd);
        }
        else
        {
            break;
        }
    }

    return part_num;
}
