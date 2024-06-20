#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ishmum Jawad Khan");
MODULE_DESCRIPTION("Registers a device nr. and implement some callback functions");

static int driver_open(struct inode *device_file, struct file *instance) {
	printk("driver - open was called!\n");
	return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
	printk("driver - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};

#define PROC_ADDR 90

static int __init ModuleInit(void) {
	printk("Hello, Kernel!\n");
	int retval = register_chrdev(PROC_ADDR, "driver", &fops);
	if (retval == 0) 
		printk("driver - registered device number: %d", PROC_ADDR);
	else if (retval > 0)
		printk("driver - registered device number: %d", retval >> 20);
	else {
		printk("Could not register device");
		return -1;
	}
	return 0;
}

static void __exit ModuleExit(void) {
	unregister_chrdev(PROC_ADDR, "driver");
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
