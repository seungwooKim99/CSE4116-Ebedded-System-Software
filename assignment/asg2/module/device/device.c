#include "../default.h"

static unsigned char *iom_fpga_addr[FPGA_DEVICE_NUMBER];

/* fpga dot font */
unsigned char fpga_number[10][10] = {
	{0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
	{0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
	{0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
	{0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
	{0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
	{0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
	{0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
	{0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
	{0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03} // 9
};

unsigned char fpga_set_blank[10] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

int iom_fpga_init(void) {
    /* iomap fpga devices */
    iom_fpga_addr[FND] = ioremap(IOM_FND_ADDRESS, 0x4);
    iom_fpga_addr[LED] = ioremap(IOM_LED_ADDRESS, 0x1);
    iom_fpga_addr[TEXT_LCD] = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);
    iom_fpga_addr[DOT] = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
    return 0;
}

int iom_fpga_unmap(void) {
    int i;
    for (i = 0; i < FPGA_DEVICE_NUMBER; i++) {
        iounmap(iom_fpga_addr[i]);
    }
    return 0;
}

int iom_fpga_dot_write(int curr_number) {
    int i;
    unsigned short int _s_value;
    if (curr_number == -1){
        /* Flush dots */
        for(i=0;i<sizeof(fpga_set_blank);i++) {
            _s_value = fpga_set_blank[i] & 0x7F;
            outw(_s_value,(unsigned int)iom_fpga_addr[DOT]+i*2);
        }
        return 0;
    }
    for(i=0;i<sizeof(fpga_number[curr_number]);i++) {
        _s_value = fpga_number[curr_number][i] & 0x7F;
		outw(_s_value,(unsigned int)iom_fpga_addr[DOT]+i*2);
    }
    return 0;
}

int iom_fpga_fnd_write(int curr_number, int curr_idx) {
    unsigned short int value_short = curr_number << (12 - 4*curr_idx);
    outw(value_short,(unsigned int)iom_fpga_addr[FND]);	    

	return 0;
}

int iom_led_write(int curr_number) {
	unsigned short value = 0;
    if (curr_number > 0){
        value = 1 << (8 - curr_number);
    }
    outw(value, (unsigned int)iom_fpga_addr[LED]);
	return 0;
}

int iom_fpga_text_lcd_write(int shift_num){
    unsigned char my_id[LINE_BUFF + 1] = "20181610";
    unsigned char my_name[LINE_BUFF + 1] = "seungwookim";
    int id_len, name_len, i;
    unsigned short int _s_value = 0;
    unsigned char string[33] = {'\0',};;
    id_len = strlen(my_id);
    name_len = strlen(my_name);

    int line1_len, line2_len;
    line1_len = LINE_BUFF - id_len;
    line2_len = LINE_BUFF - name_len;

    if (shift_num == -1) {
        /* Flush lcd */
        memset(string, ' ', MAX_BUFF);
    } else {
        /* Line 1*/
        memset(my_id + id_len, ' ', LINE_BUFF - id_len);
        if ((shift_num / line1_len) % 2 == 0){
            /* to right */
            memset(string, ' ', shift_num % line1_len);
            strncat(string, my_id, LINE_BUFF - (shift_num % line1_len));
        } else {
            /* to left */
            memset(string, ' ', line1_len - (shift_num % line1_len));
            strncat(string, my_id, LINE_BUFF - line1_len + (shift_num % line1_len));
        }

        
        /* Line 2 */
        memset(my_name + name_len, ' ', LINE_BUFF - name_len);
        if ((shift_num / line2_len) % 2 == 0){
            /* to right */
            memset(string + LINE_BUFF, ' ', shift_num % line2_len);
            strncat(string + LINE_BUFF, my_name, LINE_BUFF - (shift_num % line2_len));
        } else {
            /* to left */
            memset(string + LINE_BUFF, ' ', line2_len - (shift_num % line2_len));
            strncat(string + LINE_BUFF, my_name, LINE_BUFF - line2_len + (shift_num % line2_len));
        }
    }
    string[MAX_BUFF] = '\0';
    for(i=0;i<MAX_BUFF;i++){
        _s_value = (string[i] & 0xFF) << 8 | string[i + 1] & 0xFF;
		outw(_s_value,(unsigned int)iom_fpga_addr[TEXT_LCD]+i);
        i++;
    }
    return 0;
}