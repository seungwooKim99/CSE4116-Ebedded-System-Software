#ifndef TIMER_HEADER_INCLUDE
#define TIMER_HEADER_INCLUDE


/* function prototypes */
void initialize_password_timer(void);
void start_password_timer(void);
void deletedelete_password_timer_timer(void);
void password_timer_handler(unsigned long);
int get_password(void);

void start_session(int);
void initialize_session_timer(int);
void start_session_timer(void);
void delete_session_timer(void);
void session_timer_handler(unsigned long);


/* util function prototypes */
int sec_to_mmss(int);
int update_fnd_value(int, int);
#endif