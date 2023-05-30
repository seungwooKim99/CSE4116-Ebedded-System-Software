#ifndef DEVICE_HEADER_INCLUDE
#define DEVICE_HEADER_INCLUDE

#define IOM_FND_ADDRESS 0x08000004 // pysical address

/* function prototypes */
int iom_fpga_init(void);
int iom_fpga_unmap(void);
int iom_fpga_fnd_write(int minute, int second);
#endif