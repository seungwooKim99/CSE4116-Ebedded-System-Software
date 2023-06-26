#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/ioctl.h>

#define HASH_KEY 31

unsigned int customHash(int number) {
    char pw[5];    
    sprintf(pw, "%d", number); 
    printf("%s\n", pw);
    printf("len : %d\n", strlen(pw));

    unsigned int hash = 0;
    int i = 0;

    while (pw[i] != '\0') {
        hash += pw[i];
        hash %= 31; // 소수인 31로 나머지 연산
        i++;
    }

    return hash;
}

int main() {
    int hashed;
    int input;
    input = 123;
    hashed = customHash(input);
    printf("res : %d\n", hashed);
    return 0;
}