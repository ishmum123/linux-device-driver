#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ishmum Jawad Khan");
MODULE_DESCRIPTION("Mimics the functionality of a character device");

static char storage[1023];
static uint pointer;

static dev_t device_number;
static struct class *device_class;
static struct cdev device;

#define DRIVER_NAME "char_driver"
#define DRIVER_CLASS "char_driver_class"

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	int to_copy = min(count, pointer);
	int not_copied = copy_to_user(user_buffer, storage, to_copy);
	int delta = to_copy - not_copied;
	return delta;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	uint to_copy = min(count, sizeof(storage));
	uint not_copied = copy_to_user(storage, user_buffer, to_copy);
	pointer = to_copy;
	int delta = (int) to_copy - not_copied;
	return delta;
}

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
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

static int __init ModuleInit(void) {
	printk("%s - inserting module", DRIVER_NAME);


	if (alloc_chrdev_region(&device_number, 0, 1, DRIVER_NAME) < 0) {
		printk("Device could not be allocated!\n");
	}
	printk("%s - Device major: %d and minor: %d registered", DRIVER_NAME, device_number >> 20, device_number && 0xfffff);

	if ((device_class = class_create(DRIVER_CLASS)) == NULL) {
		printk("Device class could not be created");
		goto ClassError;
	}

	if (device_create(device_class, NULL, device_number, NULL, DRIVER_NAME) == NULL) {
		printk("Could not create a device file");
		goto FileError;
	}

	cdev_init(&device, &fops);

	if (cdev_add(&device, device_number, 1) == -1) {
		printk("Could not register device to Kernel");
		goto RegisterError;
	}

	return 0;

RegisterError:
	device_destroy(device_class, device_number);
FileError:
	class_destroy(device_class);
ClassError:
	unregister_chrdev(device_number, DRIVER_NAME);
	return -1;
}

static void __exit ModuleExit(void) {
	cdev_del(&device);
	device_destroy(device_class, device_number);
	class_destroy(device_class);
	unregister_chrdev(device_number, DRIVER_NAME);
	printk("%s - removing module", DRIVER_NAME);
}

module_init(ModuleInit);
module_exit(ModuleExit);
