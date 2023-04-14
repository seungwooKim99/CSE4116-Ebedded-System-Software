#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

struct my_data{
	int a,b,c;
};

void mynew_function(int a){
	int A,B,C,D;
	A = a / 1000;
	B = a % 1000 / 100;
	C = a % 100 / 10;
	D = a % 10;
	printk("Thousands = %d / hundreds = %d / tens = %d / units = %d\n", A,B,C,D);
	return 0;
}

long newcall(struct my_data *data){
	int a, b;

	a=data->a;
	b=data->b;

	data->a=a+b;
	data->b=a-b;
	data->c=a%b;

	return 313;
}

int funcdev_init( void ) {
	return 0;
}
void funcdev_exit( void ) { }

EXPORT_SYMBOL( newcall );
EXPORT_SYMBOL( mynew_function );

module_init ( funcdev_init );
module_exit ( funcdev_exit );
MODULE_LICENSE("Dual BSD/GPL");
