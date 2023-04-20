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
/* I/O process */
void io_process();

/* util */
void init_local_io_buffer();
void flush_all_devices();


/* io_buf operations */
void flush_buffer(bool is_key_buffer);
bool is_buffer_empty(bool is_key_buffer);
int get_latest_value_idx();
bool is_same_char_btn_pressed();

/* switch buffer operations */
bool is_switch_pressed();
bool is_reset_pressed();
void set_character();
int get_number();
bool is_same_input(unsigned char *buf);
switch_input_type get_switch_input_type();

/* mode */
void get();
void put();
void request_merge();



void flush_all_devices();
#endif