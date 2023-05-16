#ifndef TIMER_HEADER_INCLUDE
#define TIMER_HEADER_INCLUDE


/* function prototypes */

void initialize_timer(struct argument arg);
void start_timer(void);
void delete_timer(void);
void timer_handler(unsigned long data);

/* util function prototypes */
int get_idx(int base_idx, int cnt);
int get_number(int base_number, int cnt);

#endif