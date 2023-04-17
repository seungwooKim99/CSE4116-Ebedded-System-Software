#include "../default.h"

const char switch_keypad[][3] = {
    ".QZ", "ABC", "DEF",
    "GHI", "JKL", "MNO",
    "PRS", "TUV", "WXY" 
};

/* switch input */

void flush_buffer(bool is_key_buffer) {
    if (is_key_buffer){
        memset(io_buf->key, 0,sizeof(io_buf->key));
        io_buf->key_idx = 0;
        return;
    }
    memset(io_buf->value, ' ',sizeof(io_buf->value));
    io_buf->value_idx = 0;
    return;
}

bool is_buffer_empty(bool is_key_buffer) {
    int i;
    if (is_key_buffer) {
        for(i=0;i<FND_MAX_DIGIT;i++) {
            if (io_buf->key[i] != 0) return false;
        }
        return true;
    }
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (io_buf->value[i] != ' ') return false;
    }
    return true;
}

/* switch buffer operations */

bool is_switch_pressed() {
    int i;
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (sw_in_buf[i] != 0) return true;
    }
    return false;
}

bool is_reset_pressed() {
    int i;
    unsigned char base = 80;
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (sw_in_buf[i] != base + 2*i) return false;
    }
    return true;
}

int get_latest_value_idx() {
    if (io_buf->value_idx == 0) {
        return 0;
    }
    int latest_idx = io_buf->value_idx - 1;
    if (io_buf->value[io_buf->value_idx] != ' ') {
        // buffer is full. last character is 'io_buf->value_idx'
        latest_idx++;
    }
    return latest_idx;
}

bool is_same_char_btn_pressed() {
    /* check the last character in buffer(io_buf->value_idx) */
    if (io_buf->value_idx == 0) {
        return false;
    }
    int prev_idx = io_buf->value_idx - 1;
    if (io_buf->value[io_buf->value_idx] != ' ') {
        // buffer is full. last character is 'io_buf->value_idx'
        prev_idx++;
    }

    /* if prev char is number */
    if (('0' <= io_buf->value[prev_idx]) && (io_buf->value[prev_idx] <= '9')) {
        return false;
    }

    /* get which number is pressed */
    int i, j;
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (sw_in_buf[i] == 1) {
            break;
        }
    }

    if (i == SW_MAX_BUTTON) {
        printf("something's wrong!\n");
        return false;
    }

    for(j=0;j<3;j++){
        if (switch_keypad[i][j] == io_buf->value[prev_idx]) return true;
    }
    return false;
}

unsigned char set_character() {
    int i, j;
    unsigned char ch;
    int latest_idx;
    /* get which number is pressed */
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (sw_in_buf[i] == 1) {
            break;
        }
    }
    if (i == SW_MAX_BUTTON) {
        printf("something's wrong!\n");
        return false;
    }

    if (is_same_char_btn_pressed()) {
        latest_idx = get_latest_value_idx();
        /* get next char in same button */
        for (j=0;j<3;j++) {
            if (switch_keypad[i][j] == io_buf->value[latest_idx]) break;            
        }
        if (j == 3) {
            printf("something's wrong!\n");
            return;
        }
        ch = switch_keypad[i][(j+1)%3]; // next character
        /* write on local memory buffer */
        io_buf->value[latest_idx] = ch;
    } else {
        /* get the first charecter of i-th button */
        ch = switch_keypad[i][0];
        /* write on local memory buffer */
        io_buf->value[io_buf->value_idx++] = ch;
        if (io_buf->value_idx == LCD_MAX_BUFF) io_buf->value_idx--;
    }
    return;
}

unsigned char get_number() {
    int i;
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (sw_in_buf[i] == 1){
            return (unsigned char)(i+1);
        }
    }
    return (unsigned char)0;
}

bool is_same_input(unsigned char *buf) {
    int i;
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (sw_in_buf[i] != buf[i]) return false;
    }
    return true;
}

switch_input_type get_switch_input_type() {
    int i;
    unsigned char flush[SW_MAX_BUTTON] = {0,1,1,0,0,0,0,0,0};
    unsigned char change[SW_MAX_BUTTON] = {0,0,0,0,1,1,0,0,0};
    unsigned char request[SW_MAX_BUTTON] = {0,0,0,0,0,0,0,1,1};
    
    int pressed_cnt = 0;
    for(i = 0 ; i < SW_MAX_BUTTON ; i++) {
        if (sw_in_buf[i] != 0) {
            pressed_cnt++;
        }
    }

    switch(pressed_cnt) {
        case 0:
            return NONE;
        case 1:
            return SINGLE;
        case 2:
            if (is_same_input(flush)){
                printf("flush\n");
                return FLUSH;
            }
            else if (is_same_input(change)){
                printf("change\n");
                return CHANGE;
            }
            else if (is_same_input(request)){
                printf("request\n");
                return REQUEST;
            }
            else
                return NONE;
        case SW_MAX_BUTTON:
            if (is_reset_pressed()) {
                return RESET;
            } 
            return NONE;
        default:
            return NONE;
    };
    return NONE;
}



/* mode */
void put(){
    int i;
    bool is_key_input;
    if(is_switch_pressed() && (io_buf->input_mode == PENDING)) {
        io_buf->input_mode = KEY;
    }
    switch(io_buf->input_mode) {
        case PENDING:
            write_led(1);
            break;
        case KEY:
            is_key_input = true;
            if (!is_led_toggling){
                toggle_led(is_key_input); // create thread to toggle led concurrently
                is_led_toggling = true;
            }

            unsigned char num;
            switch(get_switch_input_type()) {
                case NONE:
                    break;
                case SINGLE:
                    if ((num = get_number()) == 0){
                        printf("something's wrong!\n");
                        break;
                    }
                    /* write on local memory buffer */
                    io_buf->key[io_buf->key_idx++] = num;
                    if (io_buf->key_idx == FND_MAX_DIGIT) io_buf->key_idx--;
                    
                    /* write on FND */
                    write_fnd(io_buf->key);
                    break;
                case RESET: // change mode from KEY to VALUE
                    if (!is_buffer_empty(is_key_input)) {
                        io_buf->input_mode = VALUE;
                        stop_toggle_led();
                        is_led_toggling = false;
                        break;
                    }
                    break;
                case FLUSH:
                    flush_buffer(is_key_input);
                    /* write on FND */
                    write_fnd(io_buf->key);
                    break;
                case CHANGE: // character input not supported
                    break;
                case REQUEST:
                    // key, value buffer must be not empty
                    if (!is_buffer_empty(true) && !is_buffer_empty(false)) {
                        // PUT request to main with IPC
                        strcpy(shmIOtoMainBuffer->key, io_buf->key);
                        strcpy(shmIOtoMainBuffer->value, io_buf->value);
                        shmIOtoMainBuffer->request = true; // announce PUT request to main process
                    }
                    break;
                default:
                    printf("default\n");
                    break;
            }
            break;
        case VALUE:
            is_key_input = false;
            if (!is_led_toggling){
                toggle_led(is_key_input); // create thread to toggle led concurrently
                is_led_toggling = true;
            }

            unsigned char ch;
            switch(get_switch_input_type()) {
                case NONE:
                    break;
                case SINGLE:
                    if (!io_buf->number_value_enter) {
                        set_character();
                    } else {
                        ch = get_number();
                        /* write on local memory buffer */
                        io_buf->value[io_buf->value_idx++] = ch + '0';
                        if (io_buf->value_idx == LCD_MAX_BUFF) io_buf->value_idx--;
                    }
                    /* write on LCD */
                    write_lcd(io_buf->value);
                    break;
                case RESET:
                    if (!is_buffer_empty(is_key_input)) {
                        printf("chg mode\n");
                        io_buf->input_mode = KEY;
                        stop_toggle_led();
                        is_led_toggling = false;
                    }
                    break;
                case FLUSH:
                    flush_buffer(is_key_input);
                    /* write on LCD */
                    write_lcd(io_buf->value);
                    break;
                case CHANGE:
                    io_buf->number_value_enter = !(io_buf->number_value_enter);
                    break;
                case REQUEST:
                    // key, value buffer must be not empty
                    if (!is_buffer_empty(true) && !is_buffer_empty(false)) {
                        // PUT request to main with IPC
                        strcpy(shmIOtoMainBuffer->key, io_buf->key);
                        strcpy(shmIOtoMainBuffer->value, io_buf->value);
                        shmIOtoMainBuffer->request = true; // announce PUT request to main process
                    }
                    break;
                default:
                    printf("default\n");
                    break;
            }
            break;
    }
}

/* I/O process */
void init_local_io_buffer() {
    /* init */
    io_buf = (local_IO_buffer *)malloc(sizeof(local_IO_buffer));
    io_buf->input_mode = PENDING;
    memset(io_buf->key, 0, sizeof(io_buf->key));
    memset(io_buf->value, ' ', sizeof(io_buf->value));
    io_buf->key_idx = 0;
    io_buf->value_idx = 0;
    io_buf->number_value_enter = false;

    is_led_toggling = false;
    return;
}

void flush_all_devices() {
    /* flush lcd / led / fnd */
    write_lcd(io_buf->value);
    write_led(1);
    write_fnd(io_buf->key);
    return;
}

void io_process() {
    int i;
    init_local_io_buffer();
    flush_all_devices();
    while(true) {
        /* read key with non-block */
        read_control_key();
        if (ctl_buf[0].value == KEY_PRESS){
            // key pressed
            shmIOtoMainBuffer->control_key = ctl_buf[0].code;
            //shmIOtoMainBuffer->mode = 
            // flush key, value 
            continue;
        }

        /* read switch */
        read_switch();

        switch(shmIOtoMainBuffer->mode) {
            case PUT:
                put();
                break;
            case GET:
                break;
            case MERGE:
                break;
            default:
                printf("somethings wrong!\n");
                exit(0);
        }
        
        usleep(150000); //sleep for 0.15s
        semop(sem_id, &v[READ], 1);
        semop(sem_id, &p[WRITE], 1);
    }
    return;
}