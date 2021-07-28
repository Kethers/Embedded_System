/*头文件*/
#include<linux/init.h>
#include<linux/module.h>
#include<linux/sched.h>
#include<linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/fs.h>    
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <asm/device.h>
#include <linux/workqueue.h> 

#include <linux/delay.h>

#include <linux/jiffies.h>

#define  DEVICE_RS	1

#define USE_DEVFS	0

#define dbg(a...)  printk(a)



#define DEVICE_NAME	"ht16k33"
#if DEVICE_RS
#define DEV_I2C_BUS       2 
#define DEV_I2C_ADDRESS   (0xE0>>1)


static int dev_i2c_register(void)
{
	struct i2c_board_info 	info;
	struct i2c_adapter 	*adapter;
	struct i2c_client	*client;
	
	memset(&info, 0, sizeof (struct i2c_board_info));
	info.addr = DEV_I2C_ADDRESS;
	strcpy(info.type, DEVICE_NAME);

	adapter = i2c_get_adapter(DEV_I2C_BUS);
	if (adapter == NULL) {
		return -ENODEV;
	}
	client = i2c_new_device(adapter, &info);
	i2c_put_adapter(adapter);
	if (client == NULL) {
		return -ENODEV;
	}
      
	dbg(DEVICE_NAME":dev register devices ok\n");
	return 0;
}

#endif
#define CMD_SYS_ON	0x21
#define CMD_ROW_INT(x)	(0xA0|(x))
#define CMD_LED_ON	0x81
#define CMD_LED_OFF	0x80
#define CMD_BRIGHTNESS(x) (0xE0|(x))

static char led_buf[16];

static struct i2c_client *i2c_dev;

static int ht16k33_write_command(char c)
{
	return i2c_master_send(i2c_dev, &c, 1);
}
/*
static int ht16k33_write_cmd_data(char c, char d)
{
	char v[] = {c, d};
	return i2c_master_send(i2c_dev, v, 2);
}*/
static int ht16k33_flush(void)
{
	int r;
	char buf[17];
	buf[0] = 0;
	memcpy(buf+1, led_buf, 16);
	r = i2c_master_send(i2c_dev, buf, 17);
	printk(KERN_ERR"w r %d\n", r);
	return r;
}


ssize_t buffer_read(struct file *f, struct kobject *k, struct bin_attribute *a,
                        char *buf, loff_t of, size_t len)
{
	//char cmd = 0;
	// int r;
	// char txbuf[1] = {0};
    // char rxbuf[16] = {0};
    // struct i2c_msg msg[2] = {
    //     [0] = {
    //         .addr = i2c_dev->addr,
    //         .flags = 0, //W_FLG,
    //         .len = sizeof(txbuf),
    //         .buf = txbuf,
    //     },
    //     [1] = {
    //         .addr = i2c_dev->addr,
    //         .flags = I2C_M_RD,
    //         .len = sizeof(rxbuf),
    //         .buf = rxbuf,
    //     },
    // };

    // r = i2c_transfer(i2c_dev->adapter, msg, ARRAY_SIZE(msg));
    // printk(KERN_ERR"xxxxh i2c r %d\n", r);
    // memcpy(buf, rxbuf, 16);
	if (len > a->size) len = a->size;
	memcpy(buf, led_buf, len);



	//i2c_master_send(i2c_dev, &cmd, 1);
	//i2c_master_recv(i2c_dev, buf, len);
	return len;
}
ssize_t buffer_write(struct file *f, struct kobject *k, struct bin_attribute *a,
                         char *buf, loff_t of, size_t len)
{
	if (len > a->size) len = a->size;
	memcpy(led_buf, buf, len);
	ht16k33_flush();
	return len;
}
static int brightness = 16;
ssize_t brightness_show(struct device *dev, struct device_attribute *attr,
                        char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", brightness);	
}
ssize_t brightness_store(struct device *dev, struct device_attribute *attr,
                         const char *buf, size_t count)
{

	int new = simple_strtol(buf, NULL, 0);
	if (new >= 0 && new <= 16 && brightness!=new) {
		if (brightness == 0 && new != 0) {
			ht16k33_write_command(CMD_LED_ON);
		}
		brightness = new;
		if (brightness > 0) {
			ht16k33_write_command(CMD_BRIGHTNESS(brightness-1));
		} else {
			ht16k33_write_command(CMD_LED_OFF);	
		}	
	}
	return count;
}

static  DEVICE_ATTR_RW(brightness);
static  BIN_ATTR_RW(buffer, 16);
/*
static struct bin_attribute bin_attr_buffer = {
	.attr = {
		.name = "buffer",
		.mode = S_IRUGO|S_IWUGO,
	},
	.size = 16,
	.read = buffer_read,
	.write = buffer_write,
};
*/


static int dev_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	
	dbg(DEVICE_NAME":dev_i2c_probe()\r\n");
	i2c_dev = client;
	//check device is connect ok!
	memset(led_buf, 0, sizeof led_buf);
	brightness = 16;
	
	ht16k33_write_command(CMD_SYS_ON);
	msleep(10);
	ht16k33_write_command(CMD_ROW_INT(0));
	ht16k33_flush();
	ht16k33_write_command(CMD_BRIGHTNESS(brightness-1));
	ht16k33_write_command(CMD_LED_ON);

	
	ret = device_create_bin_file(&client->dev, &bin_attr_buffer);
	if (ret < 0) {
		printk(DEVICE_NAME":error create bin file\r\n");
		return -1;	
	}
        ret = device_create_file(&client->dev, &dev_attr_brightness);
	if (ret < 0) {
		printk(DEVICE_NAME":error create brightness file\r\n");
		return -1;	
	}

	return 0;
}
static int dev_i2c_remove(struct i2c_client *client)
{
	dbg(DEVICE_NAME":dev_i2c_remove()\r\n");
	device_remove_file(&client->dev, &dev_attr_brightness);
	device_remove_bin_file(&client->dev, &bin_attr_buffer);
	ht16k33_write_command(CMD_LED_OFF);

	return 0;
}
static const struct i2c_device_id dev_i2c_id[] = {
    { DEVICE_NAME, 0, },
    { }
};
static struct i2c_driver dev_i2c_driver = {
	.driver = {
		.name	= DEVICE_NAME,
       	.owner  = THIS_MODULE,
	},
	.probe		= dev_i2c_probe,
	.remove		= dev_i2c_remove,
	.id_table	= dev_i2c_id,
};

static int __init dev_init(void)
{
	int ret;
	
	dbg(DEVICE_NAME":dev_init()\r\n");
#if DEVICE_RS
 	dev_i2c_register();
#endif
	ret = i2c_add_driver(&dev_i2c_driver);
	if (ret < 0) {
		printk("dev error: i2c_add_driver\n");
		goto e1;	
	}
	return ret;
e1:
	return -1;
}

static void __exit dev_exit(void)
{
	dbg(DEVICE_NAME":dev_exit()\r\n");
    i2c_del_driver(&dev_i2c_driver);
	
#if DEVICE_RS
	i2c_unregister_device(i2c_dev);
#endif
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DEVICE_NAME" driver");

module_init(dev_init);
module_exit(dev_exit);


