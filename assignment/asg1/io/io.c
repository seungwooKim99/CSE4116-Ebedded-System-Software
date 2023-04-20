#include "../default.h"

const char switch_keypad[][3] = {
    ".QZ", "ABC", "DEF",
    "GHI", "JKL", "MNO",
    "PRS", "TUV", "WXY" 
};

/* io_buf operations */
void flush_buffer(bool is_key_buffer) {
    if (is_key_buffer){
        io_buf->key = 0;
        io_buf->key_digit = 0;
        return;
    }
    memset(io_buf->value, 0,sizeof(io_buf->value));
    io_buf->value_idx = 0;
    return;
}

bool is_buffer_empty(bool is_key_buffer) {
    int i;
    if (is_key_buffer) {
        if(io_buf->key == 0) return true;
        return false;
    }
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (io_buf->value[i] != 0) return false;
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
    if (io_buf->value[io_buf->value_idx] != 0) {
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
    if (io_buf->value[io_buf->value_idx] != 0) {
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

void set_character() {
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
        return;
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

int get_number() {
    int i;
    for(i=0;i<SW_MAX_BUTTON;i++) {
        if (sw_in_buf[i] == 1){
            return (int)(i+1);
        }
    }
    return 0;
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
                return FLUSH;
            }
            else if (is_same_input(change)){
                return CHANGE;
            }
            else if (is_same_input(request)){
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
void get() {
    int i;
    bool is_key_input = true;
    if(is_switch_pressed() && (io_buf->input_mode == PENDING)) {
        io_buf->input_mode = KEY;
    }

    switch(io_buf->input_mode) {
        case PENDING:
            write_led(1);
            break;
        case KEY:
            if (!is_led_toggling){
                toggle_led(is_key_input); // create thread to toggle led concurrently
                is_led_toggling = true;
            }

            /* if GET Request done */
            if(shmIOtoMainBuffer->request) {
                shmIOtoMainBuffer->request = false; // GET done
                unsigned char result_lcd_buf[LCD_MAX_BUFF] = {'\0'};
                int idx = kvs->num - 1;

                if (shmIOtoMainBuffer->get_order == -1){ // key not found
                    strcpy(result_lcd_buf, "Error");
                } else {
                    sprintf(result_lcd_buf, "(%d, %d, %s)", shmIOtoMainBuffer->get_order, shmIOtoMainBuffer->get_key, shmIOtoMainBuffer->get_value);
                }
                /* write result on lcd */
                usleep(500000);
                write_lcd(result_lcd_buf);

                /* stop toggle led */
                stop_toggle_led();
                is_led_toggling = false;

                /* all led on for 1s */
                *led_addr = 255;
                usleep(1000000);
                write_led(1);

                /* reset mode */
                io_buf->input_mode = PENDING;
                flush_all_devices();
            }
            int num;
            int base = 1;
            switch(get_switch_input_type()) {
                case NONE:
                    break;
                case SINGLE:
                    if ((num = get_number()) == 0){
                        printf("something's wrong!\n");
                        break;
                    }
                    /* write on local memory buffer */
                    if (io_buf->key_digit == FND_MAX_DIGIT) {
                        io_buf->key = (io_buf->key / 10) * 10 + num;
                    } else {
                        io_buf->key = (io_buf->key)*10 + num;
                        io_buf->key_digit++;
                    }
                    
                    /* write on FND */
                    write_fnd(io_buf->key);
                    break;
                case RESET:
                    // GET request to main with IPC
                    if (!is_buffer_empty(is_key_input)) { // key buffer must not be empty
                        stop_toggle_led();
                        shmIOtoMainBuffer->key = io_buf->key;
                        shmIOtoMainBuffer->request = true; // announce PUT request to main process
                    }
                    break;
                default:
                    break;
            }
            break;
    }
}

void put(){
    int i;
    bool is_key_input;
    if(is_switch_pressed() && (io_buf->input_mode == PENDING)) {
        /* start input key mode */
        io_buf->input_mode = KEY;
    }

    /* KVS is full */
    if(kvs->is_full) {
        /* flush request to Merge process */
        shm_merge->request = REQ_FLUSH;

        /* copy KVS info */
        shm_merge->num = kvs->num;
        for(i=0;i<MAX_KVS_NUMBER;i++) {
            shm_merge->keys[i] = kvs->keys[i];
            strcpy(shm_merge->values[i], kvs->values[i]);
        }
        /* Block I/O Process until FLUSH done */
        semop(sem_kvs_id, &p_kvs[0], 1);
        return;
    }

    /* if PUT Request done */
    if(shmIOtoMainBuffer->request) {
        /* If shmIOtoMainBuffer->request has been true, Main process must succeeded Request.
         * Thus, set request resolved (set false)
         */
        shmIOtoMainBuffer->request = false;

        /* write the Put result on LCD */
        char result_lcd_buf[LCD_MAX_BUFF]= {'\0'};
        int idx = kvs->num - 1;
        sprintf(result_lcd_buf, "(%d, %d, %s)", kvs->num, kvs->keys[idx], kvs->values[idx]);
        write_lcd(result_lcd_buf);

        /* stop toggle led */
        stop_toggle_led();
        is_led_toggling = false;

        /* set all leds on for 1s */
        *led_addr = 255;
        usleep(1000000);
        write_led(1);

        /* set input_mode to PENDING */
        io_buf->input_mode = PENDING;
        flush_all_devices();
        return;
    }

    /* input_mode (key input / value input / pending(waiting)) */
    switch(io_buf->input_mode) {
        case PENDING:
            /* pending (waiting for input) */
            write_led(1);
            break;
        case KEY:
            /* Key input started or working in progess */
            is_key_input = true;
            if (!is_led_toggling){
                toggle_led(is_key_input); // create thread to toggle led concurrently
                is_led_toggling = true;
            }

            int num;
            /* switch input type (no input / character or number / special request key) */
            switch(get_switch_input_type()) {
                case NONE: // got no input (waiting)
                    break;
                case SINGLE: // got single input
                    if ((num = get_number()) == 0){
                        printf("something's wrong!\n"); // number 0 means no input 
                        break;
                    }

                    /* write on local memory buffer */     
                    if (io_buf->key_digit == FND_MAX_DIGIT) {
                        io_buf->key = (io_buf->key / 10) * 10 + num;
                    } else {
                        io_buf->key = (io_buf->key)*10 + num;
                        io_buf->key_digit++;
                    }

                    /* write on FND */
                    write_fnd(io_buf->key);
                    break;
                case RESET: // change mode from KEY to VALUE
                    if (!is_buffer_empty(is_key_input)) { // must write key first
                        io_buf->input_mode = VALUE;
                        stop_toggle_led();
                        is_led_toggling = false;
                    }
                    break;
                case FLUSH: // got flush input
                    flush_buffer(is_key_input);
                    /* write on FND */
                    write_fnd(io_buf->key);
                    break;
                case CHANGE: // character input not supported
                    break;
                case REQUEST: // got PUT request input
                    // key, value buffer must be not empty
                    if (!is_buffer_empty(true) && !is_buffer_empty(false)) {
                        /* copy io_buf to shared memory */
                        shmIOtoMainBuffer->key = io_buf->key;
                        strcpy(shmIOtoMainBuffer->value, io_buf->value);
                        shmIOtoMainBuffer->request = true; // announce PUT request to main process
                    }
                    break;
                default:
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
                        io_buf->number_value_enter = false; // set value_enter mode to character
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
                        shmIOtoMainBuffer->key = io_buf->key;
                        strcpy(shmIOtoMainBuffer->value, io_buf->value);
                        shmIOtoMainBuffer->request = true; // announce PUT request to main process
                    }
                    break;
                default:
                    break;
            }
            break;
    }
    return;
}

void request_merge() {
    switch(get_switch_input_type()) {
        case NONE:
            write_led(0);
            break;
        case RESET:
            /* request to merge process */
            shm_merge->request = REQ_MERGE;
            semop(sem_kvs_id, &p_kvs[0], 1);            
            break;
        default:
            break;
    }
}

/* I/O process */
void init_local_io_buffer() {
    /* init */
    io_buf = (local_IO_buffer *)malloc(sizeof(local_IO_buffer));
    io_buf->input_mode = PENDING;
    io_buf->key = 0;
    memset(io_buf->value, 0, sizeof(io_buf->value));
    io_buf->key_digit = 0;
    io_buf->value_idx = 0;
    io_buf->number_value_enter = false;

    is_led_toggling = false;
    return;
}

void flush_all_devices() {
    /* flush lcd / led / fnd */
    flush_buffer(true);
    flush_buffer(false);
    write_lcd(io_buf->value);
    write_led(1);
    write_fnd(io_buf->key);
    return;
}

void io_process() {
    init_local_io_buffer();
    flush_all_devices();
    bool quit = false;
    while(true) {
        /* read key with non-block mode */
        read_control_key();
        if (ctl_buf[0].value == KEY_PRESS){
            // key pressed
            // flush key, value buffer and devices
            if (is_led_toggling) {
                is_led_toggling = false;
                stop_toggle_led();
            }
            flush_all_devices();
            io_buf->input_mode = PENDING;
            switch(ctl_buf[0].code) {
                case BACK:
                    // shut down the program
                    shmIOtoMainBuffer->quit_program = true;
                    quit = true;

                    // clear devices
                    if (is_led_toggling) {
                        is_led_toggling = false;
                        stop_toggle_led();
                    }
                    flush_all_devices();
                    usleep(150000);
                    write_led(0);
                    break;
                case VOL_UP:
                    // change to next mode (PUT->GET->MERGE->PUT)
                    shmIOtoMainBuffer->mode = ((shmIOtoMainBuffer->mode + 1) % 3);
                    usleep(150000); //wait for key release
                    break;
                case VOL_DOWN:
                    // change to prev mode (PUT->MERGE->GET->PUT)
                    if ((shmIOtoMainBuffer->mode) == 0) {
                        shmIOtoMainBuffer->mode = 2;
                    } else {
                        shmIOtoMainBuffer->mode--;
                    };
                    usleep(150000); //wait for key release
                    break;
                default:
                    break;
            }
            if (quit){
                // break the loop
                usleep(150000); //sleep for 0.15s
                semop(sem_id, &v[READ], 1);
                semop(sem_id, &p[WRITE], 1);
                break;
            }
            continue;
        }

        /* read switch */
        read_switch();
        switch(shmIOtoMainBuffer->mode) {
            case PUT:
                put();
                break;
            case GET:
                get();
                break;
            case MERGE:
                request_merge();
                break;
            default:
                exit(0);
                break;
        }
        usleep(150000); //sleep for 0.15s
        semop(sem_id, &v[READ], 1);
        semop(sem_id, &p[WRITE], 1);
    }
    return;
}