#ifndef TIMER_HEADER_INCLUDE
#define TIMER_HEADER_INCLUDE


/* function prototypes */

void initialize_timer(void);
void start_timer(struct argument arg);
void timer_handler(unsigned long data);

#endif