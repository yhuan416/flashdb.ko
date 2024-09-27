/**
 * @file
 * @brief configuration file
 */

#ifndef _FDB_CFG_H_
#define _FDB_CFG_H_

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/limits.h>
#include <linux/stddef.h>

/* using KVDB feature */
#define FDB_USING_KVDB

#ifdef FDB_USING_KVDB
/* Auto update KV to latest default when current KVDB version number is changed. @see fdb_kvdb.ver_num */
/* #define FDB_KV_AUTO_UPDATE */
#endif

/* using TSDB (Time series database) feature */
#define FDB_USING_TSDB

/* Using FAL storage mode */
#define FDB_USING_FAL_MODE

/* the flash write granularity, unit: bit
 * only support 1(nor flash)/ 8(stm32f2/f4)/ 32(stm32f1) */
#define FDB_WRITE_GRAN 1 /* @note you must define it for a value */

/* MCU Endian Configuration, default is Little Endian Order. */
/* #define FDB_BIG_ENDIAN  */

/* log print macro using ESP_LOG.
 * Need to include esp_log.h when setting FDB_PRINT to ESP_LOGI().
 * default EF_PRINT macro is printf() */
//#include <esp_log.h>
//#define FDB_PRINT(...) ESP_LOGI("fdb", __VA_ARGS__)

/* print debug information */
#define FDB_DEBUG_ENABLE



#define PRId8   "d"
#define PRId16  "d"
#define PRId32  "d"
#define PRId64  "lld"
#define PRIdMAX "lld" // 或 "ld"，取决于 MAX 的定义

#define PRIi8   "i"
#define PRIi16  "i"
#define PRIi32  "i"
#define PRIi64  "lli"
#define PRIiMAX "lli" // 或 "li"，取决于 MAX 的定义

#define PRIu8   "u"
#define PRIu16  "u"
#define PRIu32  "u"
#define PRIu64  "llu"
#define PRIuMAX "llu" // 或 "lu"，取决于 MAX 的定义

#define PRIx8   "x"
#define PRIx16  "x"
#define PRIx32  "x"
#define PRIx64  "llx"
#define PRIxMAX "llx" // 或 "lx"，取决于 MAX 的定义

#define PRIX8   "X"
#define PRIX16  "X"
#define PRIX32  "X"
#define PRIX64  "llX"
#define PRIXMAX "llX" // 或 "lX"，取决于 MAX 的定义

#define PRIdLEAST16 "d"
#define PRIuLEAST16 "u"

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef u64 uint64_t;

#define UINT32_MAX (0xFFFFFFFFU)

typedef long long int intmax_t; // 定义 intmax_t 为内核的 __intmax_t

#define FDB_PRINT(...) printk(__VA_ARGS__)

#include <uapi/asm-generic/errno-base.h>

#endif /* _FDB_CFG_H_ */
