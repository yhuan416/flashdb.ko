#include <fal.h>

#include <linux/mtd/mtd.h>

#define FLASH_SIZE (1 * 1024)
#define SECTOR_SIZE (1 * 1024)
#define FAL_PART_MAGIC_WORD (0x45503130)

static unsigned char flash_mem[FLASH_SIZE];
static fal_partition_t parts;

extern char partition_name[FAL_DEV_NAME_MAX];

int detech_partition(void)
{
    struct mtd_info *mtd = NULL;
    int i = 0;

    parts = (fal_partition_t)flash_mem;

    for (i = 0; i < 64; i++)
    {
        mtd = get_mtd_device(NULL, i);
        if (!IS_ERR(mtd))
        {
            printk(KERN_INFO "mtd device found: %s\n", mtd->name);
            printk(KERN_INFO "\tdevice size: %llu\n", mtd->size);
            printk(KERN_INFO "\tdevice erasesize: %u\n", mtd->erasesize);
            printk(KERN_INFO "\tdevice writesize: %u\n", mtd->writesize);

            put_mtd_device(mtd);
        }
        else
        {
            continue;
        }
    }

    parts[0].magic_word = FAL_PART_MAGIC_WORD;
    strcpy(parts[0].name, partition_name);
    strcpy(parts[0].flash_name, MEM_FLASH_DEV_NAME);
    parts[0].offset = 0;
    parts[0].len = 32 * 1024;

    parts[1].magic_word = FAL_PART_MAGIC_WORD;
    strcpy(parts[1].name, "a");
    strcpy(parts[1].flash_name, MEM_FLASH_DEV_NAME);
    parts[1].offset = 0;
    parts[1].len = 32 * 1024;

    parts[2].magic_word = FAL_PART_MAGIC_WORD;
    strcpy(parts[2].name, "b");
    strcpy(parts[2].flash_name, MEM_FLASH_DEV_NAME);
    parts[2].offset = 0;
    parts[2].len = 32 * 1024;

    return 0;
}

static int _init(void)
{
    detech_partition();
    return 0;
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

const struct fal_flash_dev virt_parts = {
    .name = VIRT_PARTS_FLASH_DEV_NAME,
    .addr = 0x0,             // address is relative to beginning of partition; 0x0 is start of the partition
    .len = FLASH_SIZE,       // flash total size
    .blk_size = SECTOR_SIZE, // blk size
    .ops = {_init, _read, _write, _erase},
    .write_gran = 1, // 1 byte write granularity
};
