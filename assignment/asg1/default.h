#ifndef DEFAULT_HEADER_INCLUDE
#define DEFAULT_HEADER_INCLUDE
/* default headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include <ctype.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <linux/input.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "./main.h"
#include "./device/device.h"
#include "./io/io.h"
#include "./ipc/ipc.h"
#include "./merge/merge.h"

#endif