#include <fal.h>

#define LOCKER_ENABLE

#ifdef LOCKER_ENABLE
static struct mutex g_mem_blk_mutex;
#define LOCK()                    \
    do                            \
    {                             \
        mutex_lock(&g_mem_blk_mutex); \
    } while (0)

#define UNLOCK()                    \
    do                              \
    {                               \
        mutex_unlock(&g_mem_blk_mutex); \
    } while (0)
#else

#define LOCK()
#define UNLOCK()

#endif

#ifndef FLASH_SIZE
#define FLASH_SIZE (32 * 1024)
#endif

#define SECTOR_SIZE (4096)

#define SECTOR_COUNT (FLASH_SIZE / SECTOR_SIZE)

static int _init(void)
{
#ifdef LOCKER_ENABLE
    // create a mutex
    mutex_init(&g_mem_blk_mutex);
#endif

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

const struct fal_flash_dev mem_blk = {
    .name = MEM_FLASH_DEV_NAME,
    .addr = 0x0,             // address is relative to beginning of partition; 0x0 is start of the partition
    .len = FLASH_SIZE,       // size of the partition as specified in partitions.csv
    .blk_size = SECTOR_SIZE, // must be 4096 bytes
    .ops = {_init, _read, _write, _erase},
    .write_gran = 1, // 1 byte write granularity
};

int fal_flash_mem_blk_detect(fal_partition_t part, const char *part_name)
{
    return 0;
}
