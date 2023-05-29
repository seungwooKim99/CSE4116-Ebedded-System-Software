#include "../default.h"

static unsigned char *iom_fpga_fnd_addr;

int iom_fpga_init(void) {
    /* iomap fpga led device */
    iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
    return 0;
}

int iom_fpga_unmap(void) {
    iounmap(iom_fpga_fnd_addr);
    return 0;
}

int iom_fpga_fnd_write(int minute, int second) {
    unsigned short int value_short = 0; //curr_number << (12 - 4*curr_idx);
    value_short |= (minute / 10) << 12;
    value_short |= (minute % 10) << 8;
    value_short |= (second / 10) << 4;
    value_short |= (second % 10);
    outw(value_short,(unsigned int)iom_fpga_fnd_addr);	    
	return 0;
}