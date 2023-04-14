#ifndef DEFAULT_HEADER_INCLUDE
#define DEFAULT_HEADER_INCLUDE
/* default headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>

#include "./main.h"
#include "./device/device.h"
#include "./io/io.h"
#include "./ipc/ipc.h"
#include "./merge/merge.h"

#endif