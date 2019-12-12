如果编译出错，就先make prepare。这个是提前编译libtap.a。

# 01

这个例子里演示了一个类的定义和使用。还进行了接口实现。

这个就是让Media类实现了Cleanable接口。

```
g_type_add_interface_static(type, TYPE_CLEANABLE, &cleanable_info);
```

# 02

这个演示了继承。Point3D继承了Point类。

