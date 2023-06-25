#include <jni.h>
#include "android/log.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/ioctl.h>

// For Logging
#define LOG_TAG "MyTag"
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

// Driver Info
#define DEVICE_NAME_PATH "/dev/filemanager"
#define DEVICE_MAJOR_NUMBER 242
