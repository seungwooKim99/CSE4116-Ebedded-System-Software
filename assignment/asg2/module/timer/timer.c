#include "../default.h"

struct timer_list timer;
int count;

void initialize_timer() {
    init_timer(&timer);
    count = 0;
}

void start_timer(struct argument arg){
    timer.expires = get_jiffies_64();
    timer.data = (unsigned long)&arg;
    timer.function = timer_handler;
    add_timer(&timer);
}

void timer_handler(unsigned long data){
    struct arguemnt *arg = (struct arguemnt *)data;
    // write to device
    pr_info("count : %d\n", count);

    if (count > 10) {
        pr_info("DONE\n");
        return;
    }
    // count up
    count++;
    timer.expires = get_jiffies_64() + (10 / 10) * HZ; // have to modify
    timer.data = (unsigned long)arg;
    timer.function = timer_handler;
    add_timer(&timer);
}