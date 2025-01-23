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
#define VIRT_PARTS_FLASH_DEV_NAME "_virt"

#define FAL_PART_TABLE_FLASH_DEV_NAME VIRT_PARTS_FLASH_DEV_NAME
#define FAL_PART_TABLE_END_OFFSET ((1 * 1024) - 1)

/* ===================== Flash device Configuration ========================= */
extern struct fal_flash_dev virt_parts;
extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE \
    {                       \
        &virt_parts,        \
        &nor_flash0,        \
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

extern void *kvdb_malloc(size_t size);
extern void *kvdb_calloc(size_t n, size_t size);
extern void *kvdb_realloc(void *ptr, size_t size);
extern void kvdb_free(void *ptr);

#define FAL_MALLOC(s) kvdb_malloc(s)
#define FAL_CALLOC(n, s) kvdb_calloc(n, s)
#define FAL_REALLOC(p, s) kvdb_realloc(p, s)
#define FAL_FREE(ptr) kvdb_free(ptr)

#define FAL_PRINTF(...) printk(__VA_ARGS__)

#define log_d(...) pr_debug(__VA_ARGS__)
#define log_i(...) pr_info(__VA_ARGS__)
#define log_e(...) pr_err(__VA_ARGS__)

#endif /* _FAL_CFG_H_ */
