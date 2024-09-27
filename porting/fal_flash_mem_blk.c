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

static unsigned char *flash_mem;

struct fal_flash_dev mem_blk;
static int _init(void)
{
    pr_info("mem_blk init.\n");

#ifdef LOCKER_ENABLE
    // create a mutex
    mutex_init(&g_mem_blk_mutex);
#endif

    // update mem_blk.len
    pr_info("part_size: %d\n", _part_size);
    mem_blk.len = _part_size;

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

#ifndef FLASH_SIZE
#define FLASH_SIZE (32 * 1024)
#endif

#define SECTOR_SIZE (4096)

struct fal_flash_dev mem_blk = {
    .name = MEM_FLASH_DEV_NAME,
    .addr = 0x0,             // address is relative to beginning of partition; 0x0 is start of the partition
    .len = FLASH_SIZE,       // size of the partition as specified in partitions.csv
    .blk_size = SECTOR_SIZE, // must be 4096 bytes
    .ops = {_init, _read, _write, _erase},
    .write_gran = 1, // 1 byte write granularity
};

int fal_flash_mem_blk_detect(fal_partition_t parts, const char *part_name)
{
    int len = mem_blk.len;

    // 初始化flash_mem
    flash_mem = flashdb_malloc(len);
    if (!flash_mem)
        return -ENOMEM;
    memset(flash_mem, 0xFF, len); // 擦除内存

    // 初始化part
    parts[0].magic_word = FAL_PART_MAGIC_WORD;
    strcpy(parts[0].name, part_name);
    strcpy(parts[0].flash_name, MEM_FLASH_DEV_NAME);
    parts[0].offset = 0;
    parts[0].len = len;

    return 0;
}
