#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#include <sys/stat.h>

int isSymbolicLinkDirectory(const char *path) {
    struct stat fileStat;
    if (lstat(path, &fileStat) == -1) {
        perror("lstat");
        return 0;
    }

    if (S_ISLNK(fileStat.st_mode)) {
        // 파일이 심볼릭 링크인 경우
        struct stat linkStat;
        if (stat(path, &linkStat) == -1) {
            perror("stat");
            return 0;
        }

        return S_ISDIR(linkStat.st_mode);  // 링크가 디렉터리인지 확인
    }

    return S_ISDIR(fileStat.st_mode);  // 일반 파일인 경우
}

int main() {
    const char *cwd = "/";
    DIR *dir = NULL;
	struct dirent *entry = NULL;

    if((dir = opendir(cwd)) == NULL){
		exit(1);
	}
    
    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
		sprintf(path, "%s%s", cwd, entry->d_name);
        if (isSymbolicLinkDirectory(path)) {
            printf("%s : TRUE.\n", entry->d_name);
        } else {
            printf("%s : false.\n", entry->d_name);
        }
    }
    return 0;
}