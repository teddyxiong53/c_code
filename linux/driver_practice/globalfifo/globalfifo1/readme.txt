运行ok。
测试方法
su
mknod /dev/globalfifo c 229 0
cat /dev/globalfifo &
echo "xhl" > /dev/globalfifo
