#ifndef INTERRUPT_HEADER_INCLUDE
#define INTERRUPT_HEADER_INCLUDE

/* GPIO for bottons */
#define GPIO_HOME IMX_GPIO_NR(1,11)
#define GPIO_BACK IMX_GPIO_NR(1,12)
#define GPIO_VOL_UP IMX_GPIO_NR(2,15)
#define GPIO_VOL_DOWN IMX_GPIO_NR(5,14)

/* VOL_DOWN state */
#define PRESSED 0
#define RELEASED 1

/* function prototypes */
int register_interrupt(void);
int inter_release(void);
void sleep_user(void);
void wakeup_user(void);

/* top half */
irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_volup(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg);

/* bottom half*/
static void bottomhalf_tasklet_logger_fn(unsigned long data);

#endif