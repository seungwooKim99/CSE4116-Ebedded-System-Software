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
        return;
    }
    memset(io_buf->value, 0,sizeof(io_buf->value));
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
        if (io_buf->value[i] != 0) return false;
    }
    return true;
}

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

/* switch buffer operations */
unsigned char get_character() {
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


/* mode */
void put(){
    int i;
    bool is_key_input;
    unsigned char num;
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

            if ((is_reset_pressed()) && (!is_buffer_empty(is_key_input))) {
                // change Key input mode to Value input mode
                io_buf->input_mode = VALUE;
                stop_toggle_led();
                is_led_toggling = false;
                break;
            }

            if ((num = get_number()) == 0){
                // no input (waiting for input)
                break;
            }
            /* write on local memory buffer */
            io_buf->key[io_buf->key_idx++] = num;
            if (io_buf->key_idx == FND_MAX_DIGIT) io_buf->key_idx--;
            
            /* write on FND */
            write_fnd(io_buf->key);
            break;
        case VALUE:
            is_key_input = false;
            if (!is_led_toggling){
                printf("toggle led\n");
                toggle_led(is_key_input); // create thread to toggle led concurrently
                is_led_toggling = true;
            }
            if ((is_reset_pressed()) && (!is_buffer_empty(is_key_input))) {
                printf("chg mode\n");
                io_buf->input_mode = KEY;
                stop_toggle_led();
                is_led_toggling = false;
                break;
            }

            if ((num = get_number()) == 0){
                // no input (waiting for input)
                break;
            }
            printf("num: %d\n", num);

            /* write on local memory buffer */
            io_buf->value[io_buf->value_idx++] = num + '0';
            if (io_buf->value_idx == LCD_MAX_BUFF) io_buf->value_idx--;

            /* write on LCD */
            write_lcd(io_buf->value);
            break;
    }

    // entering "key" input

    // entering "value" input
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

    is_led_toggling = false;
    return;
}

void io_process() {
    int i;
    init_local_io_buffer();
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
        
        //strcpy(shmIOtoMainBuffer->value, sw_in_buf); // <- read to shm

        usleep(100000);
        //stop_toggle_led();
        semop(sem_id, &v[READ], 1);
        semop(sem_id, &p[WRITE], 1);
    }
    return;
}