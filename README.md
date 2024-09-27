
# 下载改动过的FlashDB源码
``` sh
cd flashdb
git clone -b dev_lkm git@github.com:yhuan416/FlashDB.git FlashDB
```

ARCH: architecture
KERNEL: kernel path
CROSS_COMPILE: toolchain

``` sh
make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf- KERNEL=/home/yhuan/workspace/stm32mp13/linux/linux-5.15.24
```
