#include <unistd.h>
#include <syscall.h>
#include <stdio.h>

struct seungwoo {
        int a;
        int b;
};

int main(void){
        struct seungwoo my_sw;
        my_sw.a = 2018;
        my_sw.b = 1610;
        syscall(378, &my_sw);
        return 0;
}

