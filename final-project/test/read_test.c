#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#include <sys/stat.h>

int main() {
	const char *path = "/sdcard/hello.txt";
    FILE* file = fopen(path, "r");

	    if (file == NULL) {
	        return 0;
	    }

	    struct stat st;
	    if (stat(path, &st) == -1) {
	        fclose(file);
	        return 0;
	    }
	    long fileSize = st.st_size;
	    printf("fiesize : %ld\n", fileSize);
	    printf("fiesize2 : %d\n", fileSize);

	    char* buffer = (char*) malloc(fileSize + 1);
	    if (buffer == NULL) {
	        fclose(file);
	        return 0;
	    }

		size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
	    buffer[bytesRead] = '\0';

	    fclose(file);

		printf("%s", buffer);

	    free(buffer);
		return 0;
}