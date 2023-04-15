#ifndef IO_HEADER_INCLUDE
#define IO_HEADER_INCLUDE

/* I/O process's buffer (save user input until write on shm until request) */
typedef enum __input_mode { PENDING, KEY, VALUE } _input_mode;

typedef struct _local_IO_buffer {
    _input_mode input_mode;
    unsigned char key[FND_MAX_DIGIT];
    int key_idx;
    unsigned char value[SW_MAX_BUTTON];
    int value_idx;
} local_IO_buffer;

bool is_led_toggling;

local_IO_buffer *io_buf;
#endif