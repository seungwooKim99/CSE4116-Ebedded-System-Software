#ifndef TIMER_HEADER_INCLUDE
#define TIMER_HEADER_INCLUDE

#define MAX_TIME_IN_SEC 3600 //maximum time to display

/* current timer's state */
typedef enum TIMER_MODE_ { 
    INIT,
    RUNNING,
    PAUSED,
} TIMER_MODE;

typedef struct TIMER_STATE_ {
    unsigned long last_called;
    unsigned long last_paused;
    unsigned long elapsed_total_time;
    unsigned long elapsed_pause_time;
    int curr_sec;
} TIMER_STATE;

// typedef struct fnd_out_work_ {
//     struct work_struct my_work;
//     int minute;
//     int second;
// } fnd_out_work;

/* function prototypes */
void initialize_timer(void);
void start_timer(void);
void timer_handler(unsigned long data);
void pause_timer(void);
void reset_timer(void);
void kill_timer(int value);
void terminate_timer_handler(unsigned long data);
void delete_all_timers(void);
static void bottomhalf_tasklet_fnd_clout_fn(unsigned long data);
static void bottomhalf_tasklet_terminate_fn(unsigned long unused);

// /* util functions */
// int get_minute(TIMER_STATE state);
// int get_second(TIMER_STATE state);

#endif