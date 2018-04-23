#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>



volatile unsigned long *vir_GPM4CON;
volatile unsigned long *vir_GPM4DAT;

static int led_drv_open(struct inode *in,struct file *fp)
{
	*vir_GPM4CON &= ~0xffff;	//GPM4配置寄存器把低16位清0
	*vir_GPM4CON |=  0x1111;	//GPM4配置寄存器把低16位置为0001000100010001,即把GPM4_0-GPM4_3设置为输出模式
	*vir_GPM4DAT &= ~0xf;		//GPM4数据寄存器的低4位置为1111,即把GPM4_0-GPM4_3设置为灯亮
	  
	printk("all led on!\n");
	return 0;
}

static int led_drv_release(struct inode *in,struct file *fp)
{
	*vir_GPM4DAT |= 0xf;		//GPM4数据寄存器的低4位置为1111,即把GPM4_0-GPM4_3设置为灯灭

	printk("all led off!\n");	
	return 0;
}


static ssize_t led_drv_write(struct file *fp, const char __user *buf, size_t len, loff_t *offset)
{	
//	copy_from_user(&data, buf, 1);
	
//	char reciver[20];
	
//	copy_from_user(reciver, buf, len);
//	reciver[len]='\0';
	
//	printk("%s\n",reciver);	
	
	char reciver[2];
	
	copy_from_user(reciver,buf,len);
	
	if(reciver[0]<0||reciver[0]>3)
	{
		printk("灯的编号不对劲!%d\n",reciver[0]);
	}
	else
	{
		if(reciver[1]<0||reciver[1]>1)
		{
			printk("灯的操作不对劲!%d\n",reciver[1]);
		}
		else
		{
			if(reciver[1]==0)     //灯灭
			{
				*vir_GPM4DAT |= 1 << reciver[0]	;
			}
			else                  //灯亮
			{
				*vir_GPM4DAT &= ~(1 << reciver[0]);
			}
		}
	}
	
	return 1;

}
static ssize_t led_drv_read(struct file *fp, char __user *buf, size_t len, loff_t * offset)
{
//	copy_to_user(buf, &data, 1);

//	char *sender="123456789";
	
//	copy_to_user(buf,sender,len);
	
	return 1;
}

static struct file_operations led_drv_fops={	//定义操作集合结构体
	.owner=THIS_MODULE,
	.open = led_drv_open,
	.release = led_drv_release,
	.write = led_drv_write,
	.read = led_drv_read,
};

static struct cdev *led_drv_cdev;
struct class *led_class;
static dev_t dev=0;

static int __init led_drv_init(void)
{
	//由于开启了MMU，则需要访问虚拟地址，使用ioremap函数根据物理地址获取映射得到的虚拟地址
	vir_GPM4CON= ioremap(0x110002e0, 8);
	vir_GPM4DAT= vir_GPM4CON +1;

	//初始化设备文件(字符设备驱动框架的基本套路)
	alloc_chrdev_region(&dev, 0, 1, "led_device");      //动态分配设备号
	led_drv_cdev = cdev_alloc();						//动态分配cdev结构体
	cdev_init(led_drv_cdev, &led_drv_fops);				//把cdev结构体和操作集合绑定在一起
	led_drv_cdev->owner=THIS_MODULE;					//把cdev的结构体的owner成员设置成THIS_MODULE
	cdev_add(led_drv_cdev,dev, 1);						//把cdev结构体和设备号绑定在一起并且注册

	//自动创建设备文件
	led_class = class_create(THIS_MODULE,"led_dir");//sys/class/led_dir/    //自动创建类文件
	device_create(led_class, NULL, dev, NULL,"led_dev_file");//"/dev/led_dev_file" //根据自动创建的类文件自动创建设备文件

	return 0;	
}

static void __exit led_drv_exit(void)
{
	//自动销毁设备文件
	device_destroy(led_class, dev);		//销毁设备文件
	class_destroy(led_class);			//销毁类文件
	
	cdev_del(led_drv_cdev);				//回收cdev结构体
	unregister_chrdev_region(dev, 1);	//回收设备号
	
	iounmap(vir_GPM4CON);				//消除虚拟地址与物理地址的映射关系

}


module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");


