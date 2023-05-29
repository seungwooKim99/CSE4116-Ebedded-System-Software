#include "../default.h"

/* wait queue head for for sleeping process */
wait_queue_head_t waitq;
DECLARE_WAIT_QUEUE_HEAD(waitq);

/* declare tasklets */
DECLARE_TASKLET(home_logger_task, bottomhalf_tasklet_logger_fn, "home");
DECLARE_TASKLET(back_logger_task, bottomhalf_tasklet_logger_fn, "back");
DECLARE_TASKLET(volup_logger_task, bottomhalf_tasklet_logger_fn, "vol+");
DECLARE_TASKLET(voldown_logger_task, bottomhalf_tasklet_logger_fn, "vol-");

/* bottom half to notice which button has pressed */
static void bottomhalf_tasklet_logger_fn(unsigned long data) {
    pr_info("[%s] button pressed!\n", (char *)data); 
} 

/* handler for HOME interrupt */
irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg) {
	// printk(KERN_ALERT "interrput: %x\n", gpio_get_value(GPIO_HOME));
	start_timer();
    tasklet_schedule(&home_logger_task);
	return IRQ_HANDLED;
}

/* handler for BACK interrupt */
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg) {
    // printk(KERN_ALERT "interrupt: %x\n", gpio_get_value(GPIO_BACK));
	pause_timer();
    tasklet_schedule(&back_logger_task);
    return IRQ_HANDLED;
}

/* handler for VOL+ interrupt */
irqreturn_t inter_handler_volup(int irq, void* dev_id,struct pt_regs* reg) {
    // printk(KERN_ALERT "interrupt: %x\n", gpio_get_value(GPIO_VOL_UP));
	reset_timer();
    tasklet_schedule(&volup_logger_task);
    return IRQ_HANDLED;
}

/* handler for VOL- interrupt */
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg) {
    // printk(KERN_ALERT "interrupt: %x\n", gpio_get_value(GPIO_VOL_DOWN));
	kill_timer(gpio_get_value(GPIO_VOL_DOWN));
    tasklet_schedule(&voldown_logger_task);
    return IRQ_HANDLED;
}

int register_interrupt(void){
	int ret;
	int irq;

	printk(KERN_ALERT "Register Interrupt\n");

	// interrupt home
	gpio_direction_input(GPIO_HOME);
	irq = gpio_to_irq(GPIO_HOME);
	printk(KERN_ALERT "GPIO HOME IRQ Number : %d\n",irq);
	ret=request_irq(irq, inter_handler_home, IRQF_TRIGGER_RISING, "home", 0);

	// interrupt back
	gpio_direction_input(GPIO_BACK);
	irq = gpio_to_irq(GPIO_BACK);
	printk(KERN_ALERT "GPIO BACK IRQ Number : %d\n",irq);
	ret=request_irq(irq, inter_handler_back, IRQF_TRIGGER_RISING, "back", 0);

	// interrupt vol+
	gpio_direction_input(GPIO_VOL_UP);
	irq = gpio_to_irq(GPIO_VOL_UP);
	printk(KERN_ALERT "GPIO VOL+ IRQ Number : %d\n",irq);
	ret=request_irq(irq, inter_handler_volup, IRQF_TRIGGER_RISING, "volup", 0);

	// interrupt vol-
	gpio_direction_input(GPIO_VOL_DOWN);
	irq = gpio_to_irq(GPIO_VOL_DOWN);
	printk(KERN_ALERT "GPIO VOL- IRQ Number : %d\n",irq);
	ret=request_irq(irq, inter_handler_voldown, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "voldown", 0);
	return 0;
}

int inter_release(void){
	free_irq(gpio_to_irq(GPIO_HOME), NULL);
	free_irq(gpio_to_irq(GPIO_BACK), NULL);
	free_irq(gpio_to_irq(GPIO_VOL_UP), NULL);
	free_irq(gpio_to_irq(GPIO_VOL_DOWN), NULL);
	
	printk(KERN_ALERT "Release Interrupt\n");
	return 0;
}

void sleep_user(void) {
	wait_event_interruptible(waitq, 0); // sleep
}
void wakeup_user(void) {
	__wake_up(&waitq, 1, 1, NULL);
}