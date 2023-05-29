#include "../default.h"

/* stopwatch timer */
struct timer_list timer;
TIMER_STATE curr_state; // stopwatch's state
unsigned long base_time; // jiffies of start time (00:00)
bool base_time_set; // flag for base_time setting
TIMER_MODE mode; // last pressed button
int fnd_out_second; //variable to display on FND

/* terminate timer */
struct timer_list terminate_timer;
bool is_voldown_pressed;
bool is_terminate_timer_init_done = false;

static void bottomhalf_tasklet_fnd_out_fn(unsigned long unused) {
    fnd_out_second++;
    /* print out on FND */
    iom_fpga_fnd_write(fnd_out_second / 60, fnd_out_second % 60);
}

static void bottomhalf_tasklet_terminate_fn(unsigned long unused) {
    // /* release interrupts */
    // inter_release();

    // /* delete timers */
    // del_timer(&timer);
    // del_timer(&terminate_timer);

    /* wake up user app */
    // wake_up_interruptible(waitq);
    printk("wake user up\n");
    wakeup_user();
}

DECLARE_TASKLET(fnd_out_task, bottomhalf_tasklet_fnd_out_fn, 0L);
DECLARE_TASKLET(terminate_task, bottomhalf_tasklet_terminate_fn, 0L);


void initialize_timer(void) {
    init_timer(&timer);
    if (!is_terminate_timer_init_done) {
        is_terminate_timer_init_done = true;
        init_timer(&terminate_timer);
    }

    /* flush fnd */
    iom_fpga_fnd_write(0, 0);

    /* set initial state */
    mode = INIT;
    curr_state.last_called = curr_state.last_paused = 0;
    curr_state.elapsed_total_time = 0;
    curr_state.elapsed_pause_time = 0;
    curr_state.curr_sec = 0; 
    base_time_set = false;
    base_time = 0;
    fnd_out_second = 0;
    is_voldown_pressed = false;
}

/* start stopwatch */
void start_timer(void) {
	if (mode == RUNNING) {
		/* nothing to do */
		return;
	}
    mode = RUNNING;

    /* refresh timer */
    del_timer(&timer);
    timer.expires = get_jiffies_64(); // start timer immediately
    timer.data = (unsigned long)&curr_state;
    timer.function = timer_handler;

    /* add timer */
    add_timer(&timer);
    return;
}

void timer_handler(unsigned long data) {
    TIMER_STATE *state = (TIMER_STATE*)data;
    unsigned long curr_time = get_jiffies_64();
    timer.expires = curr_time + (HZ/10); // timer interrupts after 0.1s

    /* check if base time set */
    if (!base_time_set) { 
        base_time = curr_time; // set base time to current time
        base_time_set = true;
    }

    /* if the stopwatch resumed (switched from pause to run) */
    if(state->last_paused != 0) {
        state->elapsed_pause_time += (curr_time - state->last_paused); // cumulate the pause time
        state->last_paused = 0; // flush last paused time
    }
    state->last_called = curr_time; //update last call time to current time
    state->elapsed_total_time = curr_time - base_time; //update total elapsed time

    /* if 1 second elapsed from "state->curr_sec" */
    if(state->curr_sec < (int)(state->elapsed_total_time - state->elapsed_pause_time) / HZ) {
        /* FND count up required (1 second elapsed) */
        state->curr_sec = (int)(state->elapsed_total_time - state->elapsed_pause_time) / HZ;
        /* schedule taslklet to update FND */
        tasklet_hi_schedule(&fnd_out_task);
    }

    timer.data = (unsigned long)&curr_state;
    timer.function = timer_handler;

    /* add timer again */
    add_timer(&timer);
    return;
}

void pause_timer(void) {
    if ((mode == INIT) || (mode == PAUSED)) {
		/* nothing to do */
		return;
	}
    mode = PAUSED;

    /* delete timer */
    del_timer(&timer);
    curr_state.last_paused = curr_state.last_called; // update last_paused time
}

void reset_timer(void) {
    if (mode == INIT) {
        /* nothing to do */
        return;
    }

    /* delete timer */
    del_timer(&timer);

    /* init all settings */
    initialize_timer();
}

void kill_timer(int value) {
    if (value == PRESSED) {
        is_voldown_pressed = true;
        /* start terminate timer */
        terminate_timer.expires = get_jiffies_64() + 3 * HZ; // start timer after 3 seconds
        terminate_timer.data = (unsigned long)&curr_state;
        terminate_timer.function = terminate_timer_handler;

    } else {
        is_voldown_pressed = false;
        del_timer(&terminate_timer);
    }
}

void terminate_timer_handler(unsigned long data){
    if (!is_voldown_pressed) {
        /* already released */
        return;
    }
    /* terminate stopwatch */
    printk("terminate stopwatch\n");
    tasklet_hi_schedule(&terminate_task);
}

void delete_all_timers(void) {
    del_timer(&terminate_timer);
    del_timer(&timer);
}


// //util functions
// int get_minute(TIMER_STATE state) {
//     return (int)(state.elapsed_total_time - state.elapsed_pause_time) / HZ % MAX_TIME_IN_SEC / 60;
// }

// int get_second(TIMER_STATE state) {
//     return (int)(state.elapsed_total_time - state.elapsed_pause_time) / HZ % MAX_TIME_IN_SEC % 60;
// }