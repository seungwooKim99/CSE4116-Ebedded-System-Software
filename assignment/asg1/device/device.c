#include "../default.h"

void open_devices(){
    char paths[MAX_DEVICE_NUMBER][50] = {
        FND_DEVICE, FPGA_TEXT_LCD_DEVICE, SWITCH_DEVICE,
        FPGA_STEP_MOTOR_DEVICE, CONTROL_KEY_DEVICE, MMAP_DEVICE
    };

    /* open device files */
    int device_num;
    for(device_num = 0 ; device_num < MAX_DEVICE_NUMBER ; device_num++) {
        switch(device_num) {
            case FND:
                device_fds[device_num] = open(FND_DEVICE, O_RDWR);
                break;
            case LCD:
                device_fds[device_num] = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
                break;
            case SWITCH:
                device_fds[device_num] = open(SWITCH_DEVICE, O_RDWR);
                break;
            case MOTOR:
                device_fds[device_num] = open(FPGA_STEP_MOTOR_DEVICE, O_WRONLY);
                break;
            case CONTROL_KEY:
                device_fds[device_num] = open(CONTROL_KEY_DEVICE, O_RDONLY);
                break;
            case MMAP:
                device_fds[device_num] = open(MMAP_DEVICE, O_RDWR | O_SYNC);
                break;
        }
    }

    /* check if open failed */
    for(device_num = 0 ; device_num < MAX_DEVICE_NUMBER ; device_num++) {
        if (device_fds[device_num] == -1){
            printf("device open failed : %s\n", paths[device_num]);
            exit(0);
        }
    }

    /* init led with mmap */
    fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, device_fds[MMAP], FPGA_BASE_ADDRESS);
    led_addr = (unsigned char *)((void *)fpga_addr + LED_ADDR);
    return;
}