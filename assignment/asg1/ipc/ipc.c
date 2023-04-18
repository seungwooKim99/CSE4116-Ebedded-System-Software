#include "../default.h"

void init_shared_memory(){
    /* IO to Main shm */
    shmIOtoMain_id = shmget(SHM_KEY_1, sizeof(shmIOtoMain), IPC_CREAT);
    if (shmIOtoMain_id == -1) {
        printf("shmget io shm error\n");
        exit(0);
    }
    shmIOtoMainBuffer = shmat(shmIOtoMain_id, 0, 0);

    /* init */
    shmIOtoMainBuffer->mode = PUT;
    shmIOtoMainBuffer->key = 0;
    memset(shmIOtoMainBuffer->value, 0, sizeof(shmIOtoMainBuffer->value));
    shmIOtoMainBuffer->control_key = 0;
    shmIOtoMainBuffer->request = false;

    /* KVS */
    kvs_id = shmget(SHM_KEY_2, sizeof(shmKVS), IPC_CREAT);
    if (kvs_id == -1) {
        printf("shmget kvs error\n");
        exit(0);
    }
    kvs = shmat(kvs_id, 0, 0);

    /* init */
    int i;
    kvs->num = 0;
    for(i=0;i<MAX_KVS_NUMBER;i++){
        kvs->keys[i] = 0;
        memset(kvs->values[i], 0, sizeof(kvs->values[i]));
    }
    kvs->get_request_idx = -1;
    kvs->request_done = false;
    kvs->is_full = false;

    /* Merge */
    shmMerge_id = shmget(SHM_KEY_3, sizeof(shmMerge), IPC_CREAT);
    if (shmMerge_id == -1) {
        printf("shmget merge error\n");
        exit(0);
    }
    shm_merge = shmat(shmMerge_id, 0, 0);

    /* init */
    shm_merge->request = NONE;
    shm_merge->num = 0;
    for(i=0;i<MAX_KVS_NUMBER;i++){
        shm_merge->keys[i] = 0;
        memset(shm_merge->values[i], 0, sizeof(shm_merge->values[i]));
    }

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
    // union semun semunarg_2;
    // semunarg_2.val = 1;

    for(i = 0 ; i < SEM_NUMBER; i++) {
        semctl(sem_id, i, SETVAL, semunarg);
        p[i].sem_num = v[i].sem_num = i;
        p[i].sem_flg = v[i].sem_flg = SEM_UNDO;
        p[i].sem_op = -1;
        v[i].sem_op = 1;
    }

    /////////////////////////////////////////////
    sem_kvs_id = semget(SEM_KEY_KVS, SEM_NUMBER, IPC_CREAT);
    if (sem_kvs_id == -1) {
        printf("semget error\n");
        exit(0);
    }
    union semun semunarg_kvs;
    semunarg_kvs.val = 0;
    // union semun semunarg_2;
    // semunarg_2.val = 1;

    for(i = 0 ; i < SEM_NUMBER; i++) {
        semctl(sem_kvs_id, i, SETVAL, semunarg_kvs);
        p_kvs[i].sem_num = v_kvs[i].sem_num = i;
        p_kvs[i].sem_flg = v_kvs[i].sem_flg = SEM_UNDO;
        p_kvs[i].sem_op = -1;
        v_kvs[i].sem_op = 1;
    }

    return;
}