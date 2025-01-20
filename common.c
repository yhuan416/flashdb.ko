#include "common.h"

#include <fal.h>

// 模块参数, 目标分区名, 默认为: 'flashdb'
char param_part_name[FAL_DEV_NAME_MAX] = "mtdram test device";
module_param_string(part, param_part_name, FAL_DEV_NAME_MAX, 0);
MODULE_PARM_DESC(part, "Partition name of flashdb (Default: kvdb).");

// // 模块参数, MEM分区大小, 默认为: 32K
// // 当物理分区中没有找到 param_part_name 对应的分区时, 会使用 MEM 分区. 该参数用于配置 MEM 分区大小
// int param_part_size = (32 * 1024);
// module_param_named(size, param_part_size, int, 0444);
// MODULE_PARM_DESC(size, "Partition size of memblk (Default: 32K).");

// // 模块参数, MTD设备名, 默认为: 'kvdb'
// // 用于指定 MTD 设备
// char param_mtd_name[FAL_DEV_NAME_MAX] = "kvdb";
// module_param_string(mtd, param_mtd_name, FAL_DEV_NAME_MAX, 0);
// MODULE_PARM_DESC(mtd, "Using MTD device name, (Default: kvdb).");
