#ifndef DEVICE_HEADER_INCLUDE
#define DEVICE_HEADER_INCLUDE

#define FPGA_DEVICE_NUMBER 4

#define IOM_FND_ADDRESS 0x08000004 // pysical address
#define IOM_LED_ADDRESS 0x08000016 // pysical address
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090 // pysical address - 32 Byte (16 * 2)
#define IOM_FPGA_DOT_ADDRESS 0x08000210 // pysical address

/* lcd buffer */
#define MAX_BUFF 32
#define LINE_BUFF 16

/* device index number */
#define FND 0
#define LED 1
#define TEXT_LCD 2
#define DOT 3

/* function prototypes */
int iom_fpga_init(void);
int iom_fpga_unmap(void);
int iom_fpga_fnd_write(int curr_number, int curr_idx);
int iom_led_write(int curr_number);
int iom_fpga_text_lcd_write(int shift_num);
int iom_fpga_dot_write(int curr_number);
#endif