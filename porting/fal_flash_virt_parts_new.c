/**
 * @file fal_flash_virt_parts.c
 *
 * FlashDB虚拟分区模拟
 *
 * 这个模拟出来的flash用于保存解析完成的分区表, 用于后续的分区操作
 */
#include <fal.h>

#include <linux/mtd/mtd.h>

#include "common.h"

#define FLASH_SIZE (1 * 1024)
#define SECTOR_SIZE (1 * 1024)

// 使用一个静态数组模拟flash
static unsigned char flash_mem[FLASH_SIZE];

// 分区表
static int part_count = 0;
static fal_partition_t parts;
static volatile int partition_table_is_initialed = 0;

// 静态初始化一个s_mutex
static DEFINE_MUTEX(s_mutex); // 定义并初始化 mutex

// 初始化
fal_partition_t fal_flash_get_partition(void);
static int _init(void)
{
    fal_partition_t part = NULL;

    // 保存虚拟分区表本体
    part = fal_flash_get_partition();
    if (part == NULL)
    {
        return -1;
    }

    // 保存虚拟分区表本体
    part->magic_word = FAL_PART_MAGIC_WORD;
    strcpy(part->name, "tables");
    strcpy(part->flash_name, VIRT_PARTS_FLASH_DEV_NAME);
    part->offset = 0;
    part->len = FLASH_SIZE;

    return 0;
}

// 读取
static int _read(long offset, uint8_t *buf, size_t size)
{
    int ret = 0;

    if (offset < 0)
    {
        return 0;
    }

    if (offset + size > FLASH_SIZE)
    {
        return 0;
    }

    memcpy(buf, (uint8_t *)flash_mem + offset, size);
    ret = size;

    return ret;
}

// 写入
static int _write(long offset, const uint8_t *buf, size_t size)
{
    // 不支持写入
    return 0;
}

// 擦除
static int _erase(long offset, size_t size)
{
    // 不支持擦除
    return 0;
}

// 虚拟分区flash设备
struct fal_flash_dev virt_parts = {
    .name = VIRT_PARTS_FLASH_DEV_NAME,
    .addr = 0x0,             // address is relative to beginning of partition; 0x0 is start of the partition
    .len = FLASH_SIZE,       // flash total size
    .blk_size = SECTOR_SIZE, // blk size
    .ops = {_init, _read, _write, _erase},
    .write_gran = 1, // 1 byte write granularity
};

fal_partition_t fal_flash_get_partition(void)
{
    fal_partition_t ret = NULL;

    mutex_lock(&s_mutex);

    if (partition_table_is_initialed == 0)
    {
        memset(flash_mem, 0xFF, FLASH_SIZE);
        parts = (fal_partition_t)flash_mem;

        partition_table_is_initialed = 1;
    }

    ret = &parts[part_count];
    part_count++;

    mutex_unlock(&s_mutex);

    return ret;
}
