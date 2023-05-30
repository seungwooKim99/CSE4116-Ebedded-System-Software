#ifndef TIMER_HEADER_INCLUDE
#define TIMER_HEADER_INCLUDE

#define MAX_TIME_IN_SEC 3600 //maximum time to display

/* current timer mode (last pressed btn) */
typedef enum TIMER_MODE_ { 
    INIT,
    RUNNING,
    PAUSED,
} TIMER_MODE;

/* current timer's state */
typedef struct TIMER_STATE_ {
    unsigned long last_called; //jiffies of last timer interrupt called
    unsigned long last_paused; //jiffies of pause interrupt called
    unsigned long elapsed_total_time; //range of start time(00:00) to last called
    unsigned long elapsed_pause_time; //range of total paused time
    int curr_sec; //time to display on FND (in seconds)
} TIMER_STATE;


/* function prototypes */
void initialize_timer(void);
void start_timer(void);
void timer_handler(unsigned long data);
void pause_timer(void);
void reset_timer(void);
void kill_timer(int value);
void terminate_timer_handler(unsigned long data);
void delete_all_timers(void);

/* bottomhalf prototypes */
static void bottomhalf_tasklet_fnd_clout_fn(unsigned long data);
static void bottomhalf_tasklet_terminate_fn(unsigned long unused);
#endif