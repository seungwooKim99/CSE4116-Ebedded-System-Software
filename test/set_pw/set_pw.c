#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#define HASHED_PW_PATH "hashed_pw"

void set_pw(void) {
    FILE *fp;
    fp = fopen(HASHED_PW_PATH, "w");
    const char pw[5] = "1234";
    fputs(pw, fp);
    fclose(fp);
    return;
}

int main(void) {
    FILE *fp;
    fp = fopen(HASHED_PW_PATH, "r");
    while (!fp) {
        printf("no file : [%s]\n", HASHED_PW_PATH);
        set_pw();
        fp = fopen(HASHED_PW_PATH, "r");
    }
    char hashed_pw[4000];
    fgets(hashed_pw, sizeof(hashed_pw), fp);
    printf("hashed result: %s\n", hashed_pw);
    fclose(fp);
}