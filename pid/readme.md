# 1. pid1.c
这个是位置型pid最简单的实现，没有考虑死区问题，没有设定上下限。
只是对公式最直观的实现。

# 2. pid2.c
这个是增量型pid最简单的实现。可以对比代码看区别。

# 3. pid3.c
这个是在pid1.c的基础上，进行了积分分离的代码，可以对比看区别。这样导致的效果就是更快速达到目标速度附近。


