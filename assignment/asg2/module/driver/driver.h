#ifndef DRIVER_HEADER_INCLUDE
#define DRIVER_HEADER_INCLUDE

#include <linux/ioctl.h>

#define DEVICE_NAME "dev_driver"
#define DEVICE_MAJOR_NUMBER 242

/* argument passed by user program with ioctl */
struct argument {
    int timer_interval; // 1 ~ 100
    int timer_cnt; // 1 ~ 100
    int timer_init; // 0001 ~ 8000
    int start_idx;
};

/* Set option of the device driver */
#define SET_OPTION _IOW(DEVICE_MAJOR_NUMBER, 0, struct argument *)
/* Command of the device driver */
#define COMMAND _IO(DEVICE_MAJOR_NUMBER, 1)

#define SUCCESS 0 // return value

#endif