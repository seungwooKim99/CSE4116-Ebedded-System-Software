#ifndef IO_HEADER_INCLUDE
#define IO_HEADER_INCLUDE

/* I/O process's buffer (save user input until write on shm until request) */
typedef enum __input_mode { PENDING, KEY, VALUE } _input_mode;
typedef enum __switch_input_type { NONE, SINGLE, FLUSH, CHANGE, REQUEST, RESET } switch_input_type;

typedef struct _local_IO_buffer {
    _input_mode input_mode;
    int key;
    int key_digit;
    char value[LCD_MAX_BUFF];
    int value_idx;
    bool number_value_enter;
} local_IO_buffer;

bool is_led_toggling;

local_IO_buffer *io_buf;

/* function prototypes */
void flush_all_devices();
#endif