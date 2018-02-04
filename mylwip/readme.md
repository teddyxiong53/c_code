# 简介

mylwip是我自己从零开始实现的lwip。尽量简化。

只涉及ipv4的。

第一步先实现loop的ping。

第二步实现udp。

#环境

基于qemu运行rt-thread ，bsp用的是vexpress-a9 。

rt-thread版本是3.0 。

# 编译运行

1、使用我提供的config/rtconfig.h覆盖本来的配置。

2、使用config/SConscript覆盖rt-thread/components/net/SConscript。强制编译mylwip。

3、只能放在rt-thread/components/net/目录下，跟rt-thread整体编译。



