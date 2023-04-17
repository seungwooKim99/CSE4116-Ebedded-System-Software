#include "default.h"

int main(void) {
    printf("hi\n");

    /* open and initialize devices */
    open_devices();

    /* initialize IPCs */
    init_shared_memory();
    init_semaphore();

    /* create process with fork */
    pid_t pid;

    if((pid = fork()) == -1){
        printf("fork error\n");
        exit(0);
    }
    
    if (pid == 0) { // I/O Process
        printf("I/O process foked\n");
        io_process();
        printf("I/O process done\n");
        exit(1);
    }
    //main process
    printf("Main process Running\n");
    while(true) {
        semop(sem_id, &p[READ], 1);
        main_process();
        semop(sem_id, &v[WRITE], 1);
    }
    printf("Main process done\n");
    return 0;
}

int main_process() {
    int i;
    if (shmIOtoMainBuffer->request) {
        printf("request recieved!: ");
        switch(shmIOtoMainBuffer->mode) {
            case PUT:
                printf("PUT\n");
                break;
            case GET:
                printf("GET\n");
                break;
            case MERGE:
                printf("MERGE\n");
                break;
            default:
                printf("something's wrong!\n");
                break;
        }
    }
    return;
}