# flashdb.ko

基于开源[FlashDB](https://github.com/armink/FlashDB)库, 实现一个内核态的FlashDB模块, 用于存储数据.

``` sh
ARCH: architecture
KERNEL: kernel path
CROSS_COMPILE: toolchain

make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabihf- KERNEL=/home/yhuan/workspace/stm32mp13/linux/linux-5.15.24
```

---
TODO:
- [x] 支持MTD Flash读写操作
- [ ] 支持FlashDB的基本操作(增删改查)
- [ ] MTD Flash擦除动作通过异步方式实现(否则加载时间过长)
- [ ] FAL库申请的内容没有全部被释放(该库没有设计退出接口, 是否需要实现, 或者通过devm_kzalloc来实现内存自动管理?)
- [ ] 需要梳理 类/设备/字符设备, 以及各个模块之间的关系, 以及各种资源的释放(rmmod时)
