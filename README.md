# flashdb.ko

基于开源[FlashDB](https://github.com/armink/FlashDB)库, 实现一个内核态的FlashDB模块, 用于存储数据.

``` sh
ARCH: architecture
KERNEL: kernel path
CROSS_COMPILE: toolchain

make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf- KERNEL=/home/yhuan/workspace/stm32mp13/linux/linux-5.15.24
```
