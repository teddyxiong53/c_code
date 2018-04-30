static xxx_i2c {
    struct i2c_adapter adap;
    struct i2c_algo_bit_data algo;
    void *base;
};


#define I2C_CONTROL 0X00
#define I2C_CONTROLS 0X00
#define I2C_CONTROLC 0X04

#define SCL (1<<0)
#define SDA (1<<1)



static void xxx_i2c_setsda (void *data, int state)
{
    struct xxx_i2c *i2c = data;
    writel(SDA, i2c->base+(state?I2C_CONTROLS:I2C_CONTROLC));
}

static const struct i2c_algo_bit_data xxx_i2c_algo = {
    .setsda = xxx_i2c_setsda,
    .setscl = xxx_i2c_setscl,
    .getsda = xxx_i2c_getsda,
    .getscl = xxx_i2c_getscl,
    .udelay = 30,
    .timeout = HZ,
};


static int xxx_i2c_probe (struct platform_device *dev)
{
    struct xxx_i2c *i2c;
    struct resource *res;
    int ret;
    i2c = devm_kzalloc(&dev->dev, sizeof(struct xxx_i2c));
    if(!i2c) {
        return -ENOMEM;
    }
    res = platform_get_resource(dev, IORESOURCE_MEM, 0);
    i2c->base = devm_ioremap_resource(&dev->dev, res);
    if(IS_ERR(i2c->base)) {
        return PTR_ERR(i2c->base);
    }
    writel(SCL|SDA, i2c->base+I2C_CONTROL);
    i2c->adap.owner = THIS_MODULE;
    strlcpy(i2c->adap.name, "xxx i2c adapter", sizeof(i2c->adap.name));
    i2c->adap.algo_data = &i2c->algo;
    i2c->adap.dev.parent = &dev->dev;
    i2c->adap.dev.of_node = dev->dev.of_node;
    i2c->algo = xxx_i2c_algo;
    i2c->algo.data = i2c;
    
    i2c->adap.nr = dev->id;
    ret = i2c_bit_add_numbered_bus(&i2c->adap);
    if(ret < 0) {
        return ret;
    }
    platform_set_drvdata(dev,i2c);
    return 0;
}

static int xxx_i2c_remove(struct platform_device *dev)
{
    struct xxx_i2c *i2c = platform_get_drvdata(dev);
    i2c_del_adpter(&i2c->adap);
    return 0;
}

static const struct of_device_id xxx_i2c_match[] = {
    {
        .compatible = "arm,versatile-i2c",
    },
    {},
};

MODUEL_DEVICE_TABLE (of, xxx_i2c_match);

static struct platform_driver xxx_i2c_driver = {
    .probe = xxx_i2c_probe,
    .remove = xxx_i2c_remove,
    .driver = {
        .name = "xxx-i2c",
        .of_match_table = xxx_i2c_match,
    },
};



static int xxx_i2c_init(void)
{
    return platform_driver_register(&xxx_i2c_driver);
}

static void xxx_i2c_exit(void)
{
    platform_driver_unregister(&xxx_i2c_driver);
}

