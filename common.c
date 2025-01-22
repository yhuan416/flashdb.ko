#include "common.h"

#include <fal.h>

// 模块参数, 目标分区名, 默认为: 'flashdb'
char param_part_name[FAL_DEV_NAME_MAX] = "flashdb";
module_param_string(part, param_part_name, FAL_DEV_NAME_MAX, 0);
MODULE_PARM_DESC(part, "Partition name of flashdb (Default: flashdb).");
