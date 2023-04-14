#include <linux/kernel.h>
#include <linux/uaccess.h>

struct seungwoo {
        int a;
	int b;
};

asmlinkage int sys_newcall3(struct seungwoo *sw) {
        struct seungwoo my_sw;
        copy_from_user(&my_sw, sw, sizeof(my_sw));

        int a = my_sw.a;
	int b = my_sw.b;
        printk("%d %d\n", a,b);
	printk("%d\n", (a+b));
        return 22;
}

