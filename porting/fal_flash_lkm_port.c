#include <fal.h>

#ifndef FLASH_SIZE
#define FLASH_SIZE (32 * 1024)
#endif

#define SECTOR_SIZE (4096)
#define SECTOR_COUNT (FLASH_SIZE / SECTOR_SIZE)

#define LOCKER_ENABLE

#ifdef LOCKER_ENABLE
static struct mutex flash_mutex;
#define LOCK()                    \
    do                            \
    {                             \
        mutex_lock(&flash_mutex); \
    } while (0)

#define UNLOCK()                    \
    do                              \
    {                               \
        mutex_unlock(&flash_mutex); \
    } while (0)
#else

#define LOCK()
#define UNLOCK()

#endif

static unsigned char *flash_mem;

static int init(void)
{
    flash_mem = kmalloc(FLASH_SIZE, GFP_KERNEL);
    if (!flash_mem)
        return -ENOMEM;
    memset(flash_mem, 0xFF, FLASH_SIZE); // 擦除内存

    // create a mutex
    mutex_init(&flash_mutex);

    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    int ret = 0;

    LOCK();

    memcpy(buf, (uint8_t *)flash_mem + offset, size);

    UNLOCK();

    return ret;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    int ret = 0;

    LOCK();

    memcpy((uint8_t *)flash_mem + offset, buf, size);

    UNLOCK();

    return ret;
}

static int erase(long offset, size_t size)
{
    int ret = 0;
    int32_t erase_size = ALIGN(size, SECTOR_SIZE);

    LOCK();

    memset((uint8_t *)flash_mem + offset, 0xFF, erase_size);

    UNLOCK();

    return ret;
}

const struct fal_flash_dev nor_flash0 = {
    .name = NOR_FLASH_DEV_NAME,
    .addr = 0x0,             // address is relative to beginning of partition; 0x0 is start of the partition
    .len = FLASH_SIZE,       // size of the partition as specified in partitions.csv
    .blk_size = SECTOR_SIZE, // must be 4096 bytes
    .ops = {init, read, write, erase},
    .write_gran = 1, // 1 byte write granularity
};
