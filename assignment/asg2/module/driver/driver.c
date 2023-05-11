#include "../default.h"

/* Prototypes - this would normally go in a .h file */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);

struct argument arg;

/* Global variables are declared as static, so are global within the file. */
enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

/* Is device open? Used to prevent multiple access to device */
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static struct class *cls;
static struct file_operations dev_driver_fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl,
};

static int __init dev_driver_init(void)
{
    int register_result = register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &dev_driver_fops);
    if (register_result < 0) {
        pr_alert("Registering dev_device failed with %d\n", register_result);
        return register_result;
    }
    pr_info("Assigned major number : %d.\n", DEVICE_MAJOR_NUMBER);
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(DEVICE_MAJOR_NUMBER, 0), NULL, DEVICE_NAME);
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    return SUCCESS;
}

static void __exit dev_driver_exit(void)
{
    device_destroy(cls, MKDEV(DEVICE_MAJOR_NUMBER, 0));
    class_destroy(cls);
    /* Unregister the device */
    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    pr_info("Exit device on /dev/%s\n", DEVICE_NAME);
}

/* Methods */
/* Called when a process tries to open the device file, like
* "sudo cat /dev/chardev"
*/
static int device_open(struct inode *inode, struct file *file)
{
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;
    printk(KERN_INFO "hello\n");
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{
    /* We're now ready for our next caller */
    atomic_set(&already_open, CDEV_NOT_USED);
    /* Decrement the usage count, or else once you opened the file, you will
    * never get rid of the module.
    */
    module_put(THIS_MODULE);
    printk(KERN_INFO "Goodbye\n");
    return SUCCESS;
}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
    switch(ioctl_num) {
        case SET_OPTION:
            /* Set parameter */
            pr_info("Got SET_OPTION : %d\n", ioctl_num);
            copy_from_user(&arg, (struct argument __user *)ioctl_param, sizeof(struct argument));
            pr_info("got arg : %d/%d/%d/%d\n", arg.timer_interval, arg.timer_cnt, arg.timer_init, arg.start_idx);
            break;
        case COMMAND:
            /* start timer */
            pr_info("Got COMMAND : %d\n", ioctl_num);
            break;
        default:
            pr_alert("Invalid ioctl_num : %d\n", ioctl_num);
            break;
    }
    return SUCCESS;
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);
MODULE_LICENSE("GPL");