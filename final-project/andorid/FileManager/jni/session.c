#include "default.h"
#define START_SESSION _IOW(DEVICE_MAJOR_NUMBER, 1, int *)

extern bool startSession(int sec);

bool startSession(int sec){
	int fd = open(DEVICE_NAME_PATH, O_RDWR);
	int sec_buf[1] = {sec};
	LOGV("session start\n");
	ioctl(fd, START_SESSION, sec_buf);
	close(fd);
	return true;
}
