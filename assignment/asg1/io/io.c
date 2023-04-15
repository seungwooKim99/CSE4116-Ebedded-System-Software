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

/* buffer operations */
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
    // if((!is_switch_pressed()) && (is_buffer_empty(true))) {
    //     // not pressed and buffer's empty -> pending input
    //     printf("pending\n");
    //     return;
    // }

    unsigned char num;
    if(is_switch_pressed()) {
        io_buf->input_mode = KEY;
    }
    switch(io_buf->input_mode) {
        case PENDING:
            write_led(1);
            break;
        case KEY:
            if (!is_led_toggling){
                toggle_led(true); // create thread to toggle led concurrently
                is_led_toggling = true;
            }

            if ((is_reset_pressed()) && (!is_buffer_empty(true))) {
                io_buf->input_mode = VALUE;
                stop_toggle_led();
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
    memset(io_buf->value, 0, sizeof(io_buf->value));
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