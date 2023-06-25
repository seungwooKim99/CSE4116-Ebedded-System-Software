#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#include <sys/stat.h>

int main(void) {
    char cwd[1024];
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    getcwd(cwd, sizeof(cwd));

    //test//
    char *root = "";
    if( (dir = opendir(cwd)) == NULL){
        printf("current directory error\n");
        exit(1);
    }

    printf("CWD:%s\n", root);
    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        sprintf(path, "%s/%s", cwd, entry->d_name);

        struct stat fileStat;
        if (stat(path, &fileStat) == -1) {
            perror("stat");
            continue;
        }
        if (S_ISDIR(fileStat.st_mode)) {
            printf("[%s] is a directory\n", entry->d_name);
        } else if (S_ISREG(fileStat.st_mode)) {
            printf("[%s] is a regular file\n", entry->d_name);
        }

        if (fileStat.st_mode & S_IRUSR) {
            printf("%s is readable\n", entry->d_name);
        }


        // printf("%s\n", entry->d_name);
        printf("\n");
    }
    closedir(dir);
    return 0;
}