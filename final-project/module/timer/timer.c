#include "../default.h"

struct timer_list password_timer;
int password_value;

struct timer_list session_timer;
int remain_time;

/* password timer functions */
void initialize_password_timer(void) {
    pr_alert("init password timer\n");
    password_value = 0;
    init_timer(&password_timer);
}

void start_password_timer(void){
    pr_alert("start password_timer\n");
    password_timer.expires = get_jiffies_64(); //set current jiffies to start timer handler immediately
    password_timer.data = (unsigned long)&password_value; // set initial count (0)
    password_timer.function = password_timer_handler;
    add_timer(&password_timer);
    sleep_user();
}

void delete_password_timer(void) { 
    del_timer(&password_timer);
}

void password_timer_handler(unsigned long data){
    int i;
    int *cnt = (int *)data;
    int sw_buf[MAX_BUTTON];
    int num = 0;
    //pr_alert("inside timer hander. value: %d\n", *cnt);
    iom_fpga_fnd_write(*cnt);
    if(iom_fpga_push_switch_read(sw_buf)) {
        /* if switch pressed */
        if ((sw_buf[4] == 5) && (sw_buf[5] == 6)) {
            // flush
            *cnt = 0;
        } else if ((sw_buf[7] == 8) && (sw_buf[8] == 9)) {
            // submit
            pr_alert("wake user up\n");
            wakeup_user();
            iom_fpga_fnd_write(0); //flush FND
            delete_password_timer();
            return;
        } else {
            for(i=0; i<MAX_BUTTON; i++) {
                if (sw_buf[i] != -1) {
                    num = sw_buf[i];
                    break;
                }
            }
            *cnt = update_fnd_value(*cnt, num);
        }

    };
    password_timer.data = (unsigned long)&password_value; // set cnt data
    password_timer.expires = get_jiffies_64() + (HZ / 10);
    password_timer.function = password_timer_handler; // set handler
    add_timer(&password_timer); // add timer
}

int get_password() {
    return password_value;
}

/* session timer functions */
void start_session(int sec) {
    initialize_session_timer(sec);
    start_session_timer();
}

void initialize_session_timer(int sec) {
    pr_alert("init session timer : [%d]sec\n", sec);
    remain_time = sec;
    init_timer(&session_timer);
}

void start_session_timer(void){
    pr_alert("start session timer\n");
    session_timer.expires = get_jiffies_64(); //set current jiffies to start timer handler immediately
    session_timer.data = (unsigned long)&remain_time; // set initial count (0)
    session_timer.function = session_timer_handler;
    add_timer(&session_timer);
    sleep_user();
}

void delete_session_timer(void) {
    iom_fpga_fnd_write(0);
    del_timer(&session_timer);
}

void session_timer_handler(unsigned long data){
    int i;
    int *time = (int *)data;
    iom_fpga_fnd_write(sec_to_mmss(*time));
    if (*time == 0) {
        /* session expire */
        wakeup_user();
        delete_session_timer();
        return;
    }
    *time = *time - 1;
    session_timer.expires = get_jiffies_64() + HZ;
    session_timer.data = (unsigned long)&remain_time;
    session_timer.function = session_timer_handler; // set handler
    add_timer(&session_timer); // add timer
}


/* util function */
int sec_to_mmss(int sec) {
    return sec / 60 * 100 + sec % 60;
}

int update_fnd_value(int curr, int num) {
    int n = curr;
    int digit = 0;
    if (curr == 0) {
        return num;
    }
    while(n != 0) {
        n /= 10;
        digit++;
    }
    if(digit == 4) {
        return (curr / 10) * 10 + num;
    } else {
        return curr*10 + num;
    }
}