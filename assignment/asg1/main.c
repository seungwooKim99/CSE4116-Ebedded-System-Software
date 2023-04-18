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

void resolve_put() {
    int num = kvs->num;
    if (num == MAX_KVS_NUMBER) {
        printf("somethings wrong! kvs is full\n");
        return;
    }
    kvs->keys[num] = shmIOtoMainBuffer->key;
    strcpy(kvs->values[num], shmIOtoMainBuffer->value);
    kvs->num++;
    return;
}

void resolve_get() {
    int num = kvs->num;
    int i, j;
    if (num == 0) {
        // kvs is empty. key not found.
        printf("empty\n");
        kvs->get_request_idx = -1;
        return;
    }

    // key not found.
    for(i=0;i<num;i++){
        if (kvs->keys[i] == shmIOtoMainBuffer->key){
            kvs->get_request_idx = i;
            return;
        }

    }
    kvs->get_request_idx = -1;
    return;
}

void resolve_merge() {
    return;
}

int main_process() {
    int i;
    if (shmIOtoMainBuffer->request) {
        printf("request recieved!: ");
        switch(shmIOtoMainBuffer->mode) {
            case PUT:
                if (kvs->num == MAX_KVS_NUMBER) {
                    // KVS is full. merge before PUT.
                    printf("KVS is full\n");
                    resolve_merge();
                }
                resolve_put();
                break;
            case GET:
                resolve_get();
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