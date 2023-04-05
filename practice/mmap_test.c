#include <ctype.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define FPGA_BASE_ADDRESS 0x08000000 //0x11400000
#define LED_ADDR 0x16 //0x44

int main(int argc, char **argv){
    unsigned char val[] = {0x70, 0xB0, 0xD0, 0xE0, 0x00, 0xF0};
    int i, fd;
    unsigned long *fpga_addr;
    unsigned char *led_addr;
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) exit(1);

    fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FPGA_BASE_ADDRESS);
    led_addr = (unsigned char *)((void *)fpga_addr + LED_ADDR);
    
    //*led_con |= 0x11110000;
    for(i = 0 ; i < 5 ; i++) {
        printf("%x\n", val[i]);
        *led_addr = (val[i]);
        usleep(500000);
    }
    munmap(fpga_addr, 4096);
    close(fd);
    return 0;
}