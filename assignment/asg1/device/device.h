#ifndef DEVICE_HEADER_INCLUDE
#define DEVICE_HEADER_INCLUDE

// devices number to use
#define MAX_DEVICE_NUMBER 6

// device file path
#define FND_DEVICE "/dev/fpga_fnd"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define SWITCH_DEVICE "/dev/fpga_push_switch"
#define FPGA_STEP_MOTOR_DEVICE "/dev/fpga_step_motor"
#define CONTROL_KEY_DEVICE "/dev/input/event0"
#define MMAP_DEVICE "/dev/mem"

// device number
#define FND 0
#define LCD 1
#define SWITCH 2
#define MOTOR 3
#define CONTROL_KEY 4
#define MMAP 5

// fnd
#define FND_MAX_DIGIT 4
#define FND_OFFSET (unsigned char)0x30
unsigned char fnd_out_buf[FND_MAX_DIGIT];

// text lcd
#define LCD_MAX_BUFF 32
#define LCD_LINE_BUFF 16
unsigned char lcd_out_buf[LCD_LINE_BUFF];

// switch
#define SW_MAX_BUTTON 9
unsigned char sw_in_buf[SW_MAX_BUTTON];

// motor

// control_key
#define KEY_RELEASE 0
#define KEY_PRESS 1

#define HOME 102
#define BACK 158
#define RPOG 116
#define VOL_UP 115
#define VOL_DOWN 114
#define CTL_KEY_BUF_SIZE 64
struct input_event ctl_buf[CTL_KEY_BUF_SIZE];

// led
#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16
#define LED_NUM(n) (int)(1 << (8 - n))

unsigned long *fpga_addr;
unsigned char *led_addr;
unsigned char led_out;

/* global variables */
int device_fds[MAX_DEVICE_NUMBER];

/* function prototypes */
void open_devices();
void init_devices();

/* LED (mmap)*/
pthread_t toggle_tid;
void write_led();

/* FND */
void write_fnd();

/* LCD */
void write_lcd();

/* SWITCH */
void read_switch();

/* MOTOR */
void write_motor();

/* CONTROL_KEY */
void read_control_key();

#endif