#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <linux/ioctl.h>

#define MAX_DIGIT 4
#define TIMER_DEVICE "/dev/dev_driver"
#define DEVICE_MAJOR_NUMBER 242

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


void print_error(char *msg) {
    printf("[Error] %s\n", msg);
    printf("Usage: ./app TIMER_INTERVAL[1-100] TIMER_CNT[1-100] TIMER_INIT[0001-8000]\n");
    return;
}

int main(int argc, char **argv){
    int i;
    struct argument arg;
    
    /* validate arguments number */
    if (argc != 4){
        print_error("argument number must be 4");
        return -1;
    }

    /* validate arg TIMER_INTERVAL */
    if ((atoi(argv[1]) < 1) || (100 < atoi(argv[1]))) {
        print_error("Invalid TIMER_INTERVAL value");
        return -1;
    }
    arg.timer_interval = atoi(argv[1]);


    /* validate arg TIMER_CNT */
    if ((atoi(argv[2]) < 1) || (100 < atoi(argv[2]))) {
        print_error("Invalid TIMER_CNT value");
        return -1;
    }
    arg.timer_cnt = atoi(argv[2]);


    /* validate arg TIMER_INIT */
    int str_size;
    bool non_zero_exist = false;
    str_size = (strlen(argv[3]));
    if (str_size > MAX_DIGIT) {
        print_error("TIMER_INIT must be 4 digit number only");
        return -1;
    }
    for(i=0;i<str_size;i++) {
        if((argv[3][i]<'0')||(argv[3][i])>'8') {
            print_error("Invalid Value TIMER_INIT value");
            return -1;
        } else if (argv[3][i] != '0') {
            if (non_zero_exist) {
                print_error("Only 1 digit can be non-zero in TIMER_INIT");
                return -1;
            }
            non_zero_exist = true;
            arg.start_idx = i;
        }
    }
    // check if all-zero
    if (!non_zero_exist) {
        print_error("1 digit must be non-zero in TIMER_INIT");
        return -1;
    }
    arg.timer_init = atoi(argv[3]);

    printf("%d/%d/%d/start_idx:%d\n", arg.timer_interval, arg.timer_cnt, arg.timer_init, arg.start_idx);

    /* open timer device */
    int fd = open(TIMER_DEVICE, O_WRONLY);

    /* ioctl to device driver */
    ioctl(fd, SET_OPTION, &arg);
    ioctl(fd, COMMAND);

    /* close device file */
    close(fd);
    return 0;
}