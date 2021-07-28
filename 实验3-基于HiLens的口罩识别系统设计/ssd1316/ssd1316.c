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

#include <linux/jiffies.h>

#define  DEVICE_RS	1

#define USE_DEVFS	0

#define dbg(a...)  printk(a)



#define DEVICE_NAME	"ssd1316"

#if DEVICE_RS
#define DEV_I2C_BUS       2 
#define DEV_I2C_ADDRESS   (0x78>>1)


static int dev_i2c_register(void)
{
	// TODO: 注册I2C设备
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
#define CMD_LED_ON	0xAF
#define CMD_LED_OFF	0xAE

static char led_buf[4][96];

static struct i2c_client *i2c_dev;

static int ssd1316_write_command(char c)
{
	char cmd[] = {0x00, c};// TODO: 填充CMD字段
	return i2c_master_send(i2c_dev, cmd, 2);
}
static int ssd1316_write_data(char *p)
{
	char cmd[97];
	// TODO: 填充CMD字段
	cmd[0] = 0x40;
	memcpy(cmd+1, p, 96);

	return i2c_master_send(i2c_dev, cmd, 97);
}
static void ssd1316_flush(void)
{
	int m;
	for (m = 0; m < 4; m++) {
		ssd1316_write_command(0xb0 + m);
		ssd1316_write_command(0x00);
		ssd1316_write_command(0x10);
		ssd1316_write_data(led_buf[m]);	
	}
}


ssize_t buffer_read(struct file *f, struct kobject *k, struct bin_attribute *a,
                        char *buf, loff_t of, size_t len)
{
	// TODO:
	if (len > a->size) len = a->size;
	memcpy(buf, led_buf, len);

	return len;
}
ssize_t buffer_write(struct file *f, struct kobject *k, struct bin_attribute *a,
                         char *buf, loff_t of, size_t len)
{
	// TODO:
	if (len > a->size) len = a->size;
	memcpy(led_buf, buf, len);
	ssd1316_flush();
	return len;
}

static  BIN_ATTR_RW(buffer, 4*96);


static int dev_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	char init_temp_cmd[] = {0xAE, 0x00, 0x10, 0x40, 0xb0, 
							0x81, 0x60, 0xA1, 0xA6, 0xA8, 
							0x1F, 0xC0, 0xD3, 0x00, 0xD5, 
							0x80, 0xD9, 0x22, 0xDA, 0x12, 
							0xdb, 0x40, 0x8d, 0x15, 0xAF};
	int i;

	dbg(DEVICE_NAME":dev_i2c_probe()\r\n");
	i2c_dev = client;
	//check device is connect ok!
	memset(led_buf, 0, sizeof led_buf);

	// TODO: 对OLED进行初始化
	
	for (i = 0; i < 25; i++)
	{
		ssd1316_write_command(init_temp_cmd[i]);
	}

	ssd1316_flush();

	ssd1316_write_command(CMD_LED_ON);

	
	ret = device_create_bin_file(&client->dev, &bin_attr_buffer);
	if (ret < 0) {
		printk(DEVICE_NAME":error create bin file\r\n");
		return -1;	
	}
   
	return 0;
}
static int dev_i2c_remove(struct i2c_client *client)
{
	dbg(DEVICE_NAME":dev_i2c_remove()\r\n");
	
	device_remove_bin_file(&client->dev, &bin_attr_buffer);
	ssd1316_write_command(CMD_LED_OFF);

	return 0;
}
static const struct i2c_device_id dev_i2c_id[] = {
    // TODO:
    { DEVICE_NAME, 0, },
    { }
};
static struct i2c_driver dev_i2c_driver = {
	// TODO:
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
	// TODO:
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
	// TODO:
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


