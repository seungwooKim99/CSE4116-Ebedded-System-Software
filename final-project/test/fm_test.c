#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "/dev/filemanager"
#define DEVICE_MAJOR_NUMBER 242
#define ENTER_PASSWORD _IOR(DEVICE_MAJOR_NUMBER, 0, int *)
#define START_SESSION _IOW(DEVICE_MAJOR_NUMBER, 1, int *)

int main(void) {
    int fd;
    int pw[1];
    int sec[1] = {12};
    printf("[user] open start\n");
    fd = open(DEVICE_NAME, O_RDWR);
    printf("[user] ioctl enter password start\n");
    ioctl(fd, ENTER_PASSWORD, pw);
    printf("[user] PASSWORD: %d\n", pw[0]);
    printf("[user] ioctl session start\n");
    ioctl(fd, START_SESSION, sec);
    return 0;
}