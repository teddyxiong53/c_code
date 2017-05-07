globalfifo1
运行ok。
测试方法
su
mknod /dev/globalfifo c 229 0
cat /dev/globalfifo &
echo "xhl" > /dev/globalfifo

globalfifo2
加入poll轮询机制。
测试方法
用测试程序test.c编译出来的应用进行测试。

globalfifo3
增加异步通知支持。
测试方法

globalfifo4
把globalfifo改造成platform设备。
测试方法