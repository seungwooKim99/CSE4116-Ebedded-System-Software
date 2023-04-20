#include "default.h"

pid_t pid_io, pid_merge;

int main(void) {
    printf("hi\n");

    /* open and initialize devices */
    open_devices();

    /* initialize IPCs */
    init_shared_memory();
    init_semaphore();

    /* create process with fork */

    if((pid_io = fork()) == -1){
        printf("fork error\n");
        exit(0);
    }
    
    /* I/O process */
    if (pid_io == 0) {
        io_process();
        printf("io process done\n");
    }

    if((pid_merge = fork()) == -1){
        printf("fork error\n");
        exit(0);
    }

    /* Merge process */
    if (pid_merge == 0) {
        merge_process();
        printf("merge process done\n");
    }

    //main process
    while(true) {
        semop(sem_id, &p[READ], 1);
        if(main_process()) break;
        semop(sem_id, &v[WRITE], 1);
    }

    /* kill child processes */
    if (kill(pid_io, SIGKILL) == -1) {
        printf("error kill child process\n");
    } else {
        waitpid(pid_io, NULL, 0);
    }

    if (kill(pid_merge, SIGKILL) == -1) {
        printf("error kill child process\n");
    } else {
        waitpid(pid_merge, NULL, 0);
    }

    /* remove IPC */
    free_shared_memory();
    free_semaphore();
    printf("main done\n");
    return 0;
}

void resolve_put() {
    int num = kvs->num;
    if (num == MAX_KVS_NUMBER) {
        return;
    }
    kvs->keys[num] = shmIOtoMainBuffer->key;
    strcpy(kvs->values[num], shmIOtoMainBuffer->value);
    kvs->num++;
    return;
}

void resolve_get() {
    int num = kvs->num;
    int requested_key = shmIOtoMainBuffer->key;
    int i, j;
    int kvs_idx = -1;
    /* search in KVS */
    for(i=0;i<num;i++){
        if (kvs->keys[i] == requested_key){
            kvs_idx = i;
        }
    }
    if (kvs_idx != -1) {
        shmIOtoMainBuffer->get_order = kvs_idx + 1;
        shmIOtoMainBuffer->get_key = requested_key;
        strcpy(shmIOtoMainBuffer->get_value, kvs->values[kvs_idx]);
        return;
    }
    printf("not found in kvs\n");

    /* not found in KVS. search in storage tables */
    int table_num = get_storage_table_number();
    char files_arr[2][MAX_TABLE_NAME_SIZE] = {{'\0',},{'\0',}};
    get_table_names(files_arr);

    char *files[3];
    for (i = 0 ; i < 3 ; i ++) {
        files[i] = files_arr[i];
    }
    /* sort filename in ascending order */
    qsort(files, table_num, sizeof(char *), compare_asc);
    FILE *fp;
    char read_buf[100];
    char *line, *word;
    int f_order = -1, f_key = 0;
    int tmp_order;
    char f_value[LCD_MAX_BUFF] = {'\0'};
    for (i=0;i<table_num;i++){
        char curr_filename[MAX_TABLE_NAME_SIZE] = {'\0',};
        sprintf(curr_filename, "./storage_table/%s", files[i]);
        printf("search : %s\n", curr_filename);
        fp = fopen(curr_filename, "r");
        if (!fp) {
            printf("file open error : %s\n", files[i]);
            return;
        }
        while (!feof(fp)) {
            line = fgets(read_buf, 100, fp);
            if (!line) {
                break;
            }
            word = strtok(line, " "); // order of data
            tmp_order = atoi(word);
            word = strtok(NULL, " "); // key
            printf("(req key : %d) / key : %d\n", requested_key, atoi(word));
            if (atoi(word) == requested_key) { // found the matching key
                printf("found!\n");
                f_order = tmp_order;
                f_key = atoi(word);
                word = strtok(NULL, " "); // value
                strcpy(f_value, word);
            } else {
                // skip
                word = strtok(NULL, " "); // value
            }
        }
        fclose(fp);
    }
    /* if found the key */
    printf("f_key: %d / requested_key : %d // %d", f_key, requested_key, f_key == requested_key);
    if (f_key == requested_key) {
        shmIOtoMainBuffer->get_order = f_order;
        shmIOtoMainBuffer->get_key = f_key;
        f_value[strlen(f_value) - 1] = '\0';
        strcpy(shmIOtoMainBuffer->get_value, f_value);
        return;
    }
    shmIOtoMainBuffer->get_order = -1;
    return;
}

int main_process() {
    /* if request arrived from I/O process */
    printf("num: %d\n", kvs->num);
    bool quit_main = false;
    if (shmIOtoMainBuffer->quit_program) {
        /* quit the program */
        printf("QUIT THE PROGRAM\n\n");
        //free memory
        quit_main = true;
        return quit_main;
    }
    if (shmIOtoMainBuffer->request) {
        switch(shmIOtoMainBuffer->mode) {
            case PUT:
                if (kvs->num == MAX_KVS_NUMBER) {
                    // KVS is full. merge before PUT.
                    printf("kvs is full\n");
                    kvs->is_full = true;
                    break;
                }
                resolve_put();
                break;
            case GET:
                resolve_get();
                break;
            default:
                break;
        }
    }
    return quit_main;
}