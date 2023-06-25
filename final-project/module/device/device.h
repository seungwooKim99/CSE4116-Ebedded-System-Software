#ifndef DEVICE_HEADER_INCLUDE
#define DEVICE_HEADER_INCLUDE

/* physical address */
#define IOM_FND_ADDRESS 0x08000004
#define IOM_SWITCH_ADDRESS 0x08000050	

/* switch spec */
#define MAX_BUTTON 9

/* function prototypes */
int iom_fpga_init(void);
int iom_fpga_unmap(void);
int iom_fpga_fnd_write(int val);
bool iom_fpga_push_switch_read(int buf[]);
#endif