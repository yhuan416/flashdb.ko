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

#define CLASS_NAME "flashdb" // 类名

// 默认kvdb分区
#ifndef DEFAULT_KVDB_PART
#define DEFAULT_KVDB_PART "kvdb"
#endif

extern char param_kvdb[];

#endif // __COMMON_H__
