#include "../default.h"

struct timer_list timer;
int count;

struct argument initial_state;

void initialize_timer(struct argument arg) {
    init_timer(&timer);
    count = 0;
    /* save initial argument */
    initial_state.timer_interval = arg.timer_interval;
    initial_state.timer_cnt = arg.timer_cnt;
    initial_state.timer_init = arg.timer_init;
    initial_state.start_idx = arg.start_idx;
}

void start_timer(void){
    timer.expires = get_jiffies_64(); //set current jiffies to start timer handler immediately
    timer.data = (unsigned long)&count; // set initial count (0)
    timer.function = timer_handler;
    add_timer(&timer);
}

void delete_timer(void) { 
    del_timer_sync(&timer);
}

void timer_handler(unsigned long data){
    int base_number, base_idx;
    int curr_idx, curr_number;
    int *cnt = (int *)data;

    if ((*cnt == initial_state.timer_cnt)) {
        /* Flush all */
        iom_fpga_fnd_write(0, 0);
        iom_led_write(0);
        iom_fpga_text_lcd_write(-1);
        iom_fpga_dot_write(-1);
        return;
    }

    base_idx = initial_state.start_idx;
    base_number = initial_state.timer_init;
    while(base_idx != 3){
        base_idx++;
        base_number /= 10;
    }
    base_number %= 10;
    curr_idx = get_idx(initial_state.start_idx, *cnt);
    curr_number = get_number(base_number, *cnt);

    // write to device
    iom_fpga_fnd_write(curr_number, curr_idx);
    iom_led_write(curr_number);
    iom_fpga_text_lcd_write(*cnt);
    iom_fpga_dot_write(curr_number);
    
    // count up
    (*cnt)++;
    timer.expires = get_jiffies_64() + initial_state.timer_interval * HZ / 10; // set expires
    timer.data = (unsigned long)cnt; // set cnt data
    timer.function = timer_handler; // set handler
    add_timer(&timer); // add timer
}

/* Util functions */
int get_idx(int base_idx, int cnt){
    /* calulate digit's index after "cnt" number of timer interrupt occurs 
     * (idx : 0 ~ 3)
     * ex) FND : 0200 -> base_idx = 1
     */
    return (base_idx + (cnt / 8)) % 4;
}

int get_number(int base_number, int cnt) {
    /* calulate digit after "cnt" number of timer interrupt occurs 
     * (digit : 1 ~ 8)
     * ex) FND : 0040 -> base_number = 4
     */
    return (base_number - 1 + (cnt % 8)) % 8 + 1;
}