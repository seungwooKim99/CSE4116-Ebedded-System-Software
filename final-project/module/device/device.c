#include "../default.h"

static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_switch_addr;

int iom_fpga_init(void) {
    /* iomap fpga device */
    pr_alert("iom fpga init\n");
    iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
    iom_fpga_switch_addr = ioremap(IOM_SWITCH_ADDRESS, 0x18);

    
    if (iom_fpga_fnd_addr == NULL) {
        pr_alert("fnd ioremap error\n");
    }
    if (iom_fpga_switch_addr == NULL) {
        pr_alert("switch ioremap error\n");
    }
    return 0;
}

int iom_fpga_unmap(void) {
    iounmap(iom_fpga_fnd_addr);
    iounmap(iom_fpga_switch_addr);
    return 0;
}

int iom_fpga_fnd_write(int value) {
    pr_alert("fnd write :%d\n", value);
    unsigned short int value_short = 0;
    value_short |= (value / 1000) << 12;
    value_short |= (value % 1000 / 100) << 8;
    value_short |= (value % 100 / 10) << 4;
    value_short |= (value % 10);
    outw(value_short,(unsigned int)iom_fpga_fnd_addr);	    
	return 0;
}

bool iom_fpga_push_switch_read(int buf[]) {
    int i;
    unsigned short int _s_value;
    bool is_pushed = false;
    //pr_alert("switch read\n");
	for(i=0;i<MAX_BUTTON;i++){
        //pr_alert("[%d] read\n", i);
		_s_value = inw((unsigned int)iom_fpga_switch_addr+i*2);
        //pr_alert("read done : %d\n", (_s_value & 0xFF));
        if ((_s_value & 0xFF) != 0) {
            is_pushed = true;
            buf[i] = i+1;
        } else {
            buf[i] = -1;
        }
    }
	return is_pushed;
}