#include "../default.h"

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

/* for ioctl */
#define ENTER_PASSWORD _IOR(DEVICE_MAJOR_NUMBER, 0, int *)
#define START_SESSION _IOW(DEVICE_MAJOR_NUMBER, 1, int *)

static int __init filemanager_driver_init(void)
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

static void __exit filemanager_driver_exit(void)
{
    device_destroy(cls, MKDEV(DEVICE_MAJOR_NUMBER, 0));
    class_destroy(cls);

    /* Unregister the device */
    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    pr_info("Exit device on /dev/%s\n", DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file)
{
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;
    printk(KERN_INFO "Device open\n");

    /* iomap fpga fnd device */
    iom_fpga_init();

    /* init timer */
    initialize_password_timer();

    /* flush fnd */
    iom_fpga_fnd_write(0);

    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{
    /* flush fnd to 0000 */
    iom_fpga_fnd_write(0);


    /* unmap fnd */
    iom_fpga_unmap();

    /* ready for next caller */
    atomic_set(&already_open, CDEV_NOT_USED);
    module_put(THIS_MODULE);
    printk(KERN_INFO "Device release\n");
    return SUCCESS;
}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
    switch(ioctl_num) {
        case ENTER_PASSWORD:
            /* start enter password */
            pr_alert("[module] ioctl enter password\n");
            start_password_timer();
            put_user(get_password(), (int __user *)ioctl_param);
            break;
        case START_SESSION:
            pr_alert("[module] ioctl start session\n");
            int __user *sec_ = (int __user *)ioctl_param;
            int sec;
            get_user(sec, sec_);
            start_session(sec);
            break;
        default:
            pr_alert("Invalid ioctl_num : %d\n", ioctl_num);
            break;
    }
    return SUCCESS;
}

module_init(filemanager_driver_init);
module_exit(filemanager_driver_exit);
MODULE_LICENSE("GPL");