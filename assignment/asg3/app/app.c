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
#define STOPWATCH_DEVICE "/dev/stopwatch"
#define DEVICE_MAJOR_NUMBER 242

int main(int argc, char **argv){
    int fd;

    /* open stopwatch device */
    fd = open(STOPWATCH_DEVICE, O_WRONLY);

    /* start stopwatch device */
    write(fd, NULL, 0);

    /* close device file */
    close(fd);
    return 0;
}