#include "../default.h"

void init_shared_memory(){
    /* IO to Main shm */
    shmIOtoMain_id = shmget(SHM_KEY_1, sizeof(shmIOtoMain), IPC_CREAT);
    if (shmIOtoMain_id == -1) {
        printf("shmget error\n");
        exit(0);
    }
    shmIOtoMainBuffer = shmat(shmIOtoMain_id, 0, 0);

    /* init */
    shmIOtoMainBuffer->mode = PUT;
    memset(shmIOtoMainBuffer->key, 0, sizeof(shmIOtoMainBuffer->key));
    memset(shmIOtoMainBuffer->value, 0, sizeof(shmIOtoMainBuffer->value));
    shmIOtoMainBuffer->control_key = 0;
    shmIOtoMainBuffer->request = false;
    return;
}

void init_semaphore(){
    int i;
    sem_id = semget(SEM_KEY, SEM_NUMBER, IPC_CREAT);
    if (shmIOtoMain_id == -1) {
        printf("semget error\n");
        exit(0);
    }
    union semun semunarg;
    semunarg.val = 0;

    for(i = 0 ; i < SEM_NUMBER; i++) {
        semctl(sem_id, i, SETVAL, semunarg);
        p[i].sem_num = v[i].sem_num = i;
        p[i].sem_flg = v[i].sem_flg = SEM_UNDO;
        p[i].sem_op = -1;
        v[i].sem_op = 1;
    }
    return;
}