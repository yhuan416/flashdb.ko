#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>

// #define FAL_DEBUG 1
// #define FAL_PART_HAS_TABLE_CFG

#define NOR_FLASH_DEV_NAME "norflash0"
#define VIRT_PARTS_FLASH_DEV_NAME "virtparts"
#define MEM_FLASH_DEV_NAME "memblk"

#define FAL_PART_TABLE_FLASH_DEV_NAME VIRT_PARTS_FLASH_DEV_NAME
#define FAL_PART_TABLE_END_OFFSET ((1 * 1024) - 1)

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev virt_parts;
extern const struct fal_flash_dev mem_blk;

/* flash device table */
#define FAL_FLASH_DEV_TABLE \
    {                       \
        &virt_parts,        \
        &mem_blk,           \
    }
/* ===================== Flash device Configuration ========================= */

/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                         \
    {                                                                          \
        {FAL_PART_MAGIC_WORD, PART_NAME, NOR_FLASH_DEV_NAME, 0, 32 * 1024, 0}, \
    }
#endif /* FAL_PART_HAS_TABLE_CFG */
/* ====================== Partition Configuration ========================== */

#define PRIdLEAST16 "d"

#define FAL_MALLOC(s) kmalloc(s, GFP_KERNEL)
#define FAL_CALLOC(n, s) kzalloc((n * s), GFP_KERNEL)
#define FAL_REALLOC(p, s) krealloc(p, s, GFP_KERNEL)
#define FAL_FREE(ptr) kfree(ptr)

#define FAL_PRINTF(...) printk(__VA_ARGS__)

#endif /* _FAL_CFG_H_ */
