#ifndef __KV_DB_H__
#define __KV_DB_H__

#include "common.h"

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/err.h>

#define KVDB_NODE_NAME "kvdb"

extern int kvdb_create(struct class *class);
extern void kvdb_destory(struct class *class);

extern void *kvdb_malloc(size_t size);

#endif // __KV_DB_H__
