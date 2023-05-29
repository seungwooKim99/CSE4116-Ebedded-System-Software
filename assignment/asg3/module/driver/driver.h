#ifndef DRIVER_HEADER_INCLUDE
#define DRIVER_HEADER_INCLUDE

#include <linux/ioctl.h>

#define DEVICE_NAME "stopwatch"
#define DEVICE_MAJOR_NUMBER 242

#define SUCCESS 0 // return value

/* function prototypes */
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off);
#endif