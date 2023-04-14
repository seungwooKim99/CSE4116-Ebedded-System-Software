#include "../default.h"

void io_process() {
    int i;
    usleep(400000);
    read(device_fds[SWITCH], &sw_in_buf, sizeof(sw_in_buf));
    for(i=0;i<SW_MAX_BUTTON;i++) {
		printf("[%d] ",sw_in_buf[i]);
	}
    printf("\n");
    //write
    shmIOtoMainBuffer->control_key = 100;
    printf("write done\n");
    semop(sem_id, &v[0], 1);
    return;
}