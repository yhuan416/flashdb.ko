#ifndef __COMMON_H__
#define __COMMON_H__

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/limits.h>
#include <linux/stddef.h>

#ifndef FAL_PART_MAGIC_WORD
#define FAL_PART_MAGIC_WORD (0x45503130)
#endif

#define DEV_MAJOR (0)        // 主设备号
#define NODE_NAME "flashdb"  // 设备节点名
#define DEV_NAME "kvdb"   // 设备名
#define CLASS_NAME "flashdb" // 类名

extern int param_part_size;
extern char param_part_name[];
extern char param_mtd_name[];

extern int is_valid_device(const char *name);
extern int is_valid_partition(const char *name);

extern void *flashdb_malloc(size_t size);

#endif // __COMMON_H__
