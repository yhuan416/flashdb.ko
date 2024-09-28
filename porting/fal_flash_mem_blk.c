#include <fal.h>

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

struct fal_flash_dev mem_blk;
static int _init(void)
{
    pr_debug("mem_blk init.\n");

#ifdef LOCKER_ENABLE
    // create a mutex
    mutex_init(&g_mem_blk_mutex);
#endif

    // update mem_blk.len
    pr_debug("MEM Size(%d).\n", param_part_size);
    mem_blk.len = param_part_size;

    return 0;
}

// 懒加载
static char *_get_mem_blk_addr(void)
{
    static char *mem_blk_addr = NULL;
    if (!mem_blk_addr)
    {
        pr_debug("mem_blk_addr is NULL, malloc(%d).\n", mem_blk.len);

        // 初始化mem_blk_addr
        mem_blk_addr = flashdb_malloc(mem_blk.len);
    }
    return mem_blk_addr;
}

static int _read(long offset, uint8_t *buf, size_t size)
{
    int ret = 0;
    char *mem_blk_addr;

    LOCK();

    mem_blk_addr = _get_mem_blk_addr();
    if (mem_blk_addr)
    {
        memcpy(buf, mem_blk_addr + offset, size);
        ret = size;
    }

    UNLOCK();

    return ret;
}

static int _write(long offset, const uint8_t *buf, size_t size)
{
    int ret = 0;
    char *mem_blk_addr;

    LOCK();

    mem_blk_addr = _get_mem_blk_addr();
    if (mem_blk_addr)
    {
        memcpy(mem_blk_addr + offset, buf, size);
        ret = size;
    }

    UNLOCK();

    return ret;
}

static int _erase(long offset, size_t size)
{
    int ret = 0;
    char *mem_blk_addr;

    LOCK();

    mem_blk_addr = _get_mem_blk_addr();
    if (mem_blk_addr)
    {
        memset(mem_blk_addr + offset, 0xFF, size);
        ret = size;
    }

    UNLOCK();

    return ret;
}

#ifndef FLASH_SIZE
#define FLASH_SIZE (32 * 1024)
#endif

#define SECTOR_SIZE (4096)

struct fal_flash_dev mem_blk = {
    .name = "MemKV",
    .addr = 0x0,             // address is relative to beginning of partition; 0x0 is start of the partition
    .len = FLASH_SIZE,       // size of the partition as specified in partitions.csv
    .blk_size = SECTOR_SIZE, // must be 4096 bytes
    .ops = {_init, _read, _write, _erase},
    .write_gran = 1, // 1 byte write granularity
};

int fal_flash_mem_blk_detect(fal_partition_t parts)
{
    // 初始化part
    parts[0].magic_word = FAL_PART_MAGIC_WORD;
    strcpy(parts[0].name, param_part_name);
    strcpy(parts[0].flash_name, "MemKV");
    parts[0].offset = 0;
    parts[0].len = mem_blk.len;

    return 0;
}
