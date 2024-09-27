#include <fal.h>

#include <linux/mtd/mtd.h>

#include "common.h"

#define FLASH_SIZE (1 * 1024)
#define SECTOR_SIZE (1 * 1024)

static unsigned char flash_mem[FLASH_SIZE];
static fal_partition_t parts;

extern int fal_flash_mem_blk_detect(fal_partition_t parts);
extern int fal_flash_nor_flash_detect(fal_partition_t parts);

static int detect_partition(void)
{
    int ret = 0;
    int count = 0;

    pr_debug("virt_parts start detect partition.\n");

    parts = (fal_partition_t)flash_mem;

#ifdef STORE_VIRT_PARTS_TO_TABLE
    // store the partition of virt_parts
    parts[0].magic_word = FAL_PART_MAGIC_WORD;
    strcpy(parts[0].name, "VirtParts");
    strcpy(parts[0].flash_name, VIRT_PARTS_FLASH_DEV_NAME);
    parts[0].offset = 0;
    parts[0].len = FLASH_SIZE;

    count++;
#endif

    ret = fal_flash_nor_flash_detect(&parts[count]);
    if (ret < 0)
    {
        pr_err("fal_flash_nor_flash_detect fail.\n");
        return -1;
    }
    count += ret;

    // 保底采用mem_blk模拟一个flash
    ret = fal_flash_mem_blk_detect(&parts[count]);
    if (ret < 0)
    {
        pr_err("fal_flash_mem_blk_detect fail.\n");
        return -1;
    }
    count += ret;

    return count;
}

static int _init(void)
{
    return detect_partition();
}

static int _read(long offset, uint8_t *buf, size_t size)
{
    int ret = size;

    if (offset < 0)
    {
        return 0;
    }

    if (offset + size > FLASH_SIZE)
    {
        return 0;
    }

    memcpy(buf, (uint8_t *)flash_mem + offset, size);
    return ret;
}

static int _write(long offset, const uint8_t *buf, size_t size)
{
    // 不支持写入
    return 0;
}

static int _erase(long offset, size_t size)
{
    // 不支持擦除
    return 0;
}

struct fal_flash_dev virt_parts = {
    .name = VIRT_PARTS_FLASH_DEV_NAME,
    .addr = 0x0,             // address is relative to beginning of partition; 0x0 is start of the partition
    .len = FLASH_SIZE,       // flash total size
    .blk_size = SECTOR_SIZE, // blk size
    .ops = {_init, _read, _write, _erase},
    .write_gran = 1, // 1 byte write granularity
};
