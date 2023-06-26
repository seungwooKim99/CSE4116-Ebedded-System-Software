#include "default.h"
#define START_SESSION _IOW(DEVICE_MAJOR_NUMBER, 1, int *)

extern bool startSession(int sec);
int fd = -1;

bool startSession(int sec){
	fd = open(DEVICE_NAME_PATH, O_RDWR);
	int sec_buf[1] = {sec};
	ioctl(fd, START_SESSION, sec_buf);
	close(fd);
	return true;
}
