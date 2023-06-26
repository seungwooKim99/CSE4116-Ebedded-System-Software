#include "../default.h"

/* wait queue head for sleeping process */
wait_queue_head_t waitq;
DECLARE_WAIT_QUEUE_HEAD(waitq);

/* sleep user process */
void sleep_user(void) {
	interruptible_sleep_on(&waitq);
}

/* wake up user process */
void wakeup_user(void) {
	wake_up_interruptible(&waitq);
}