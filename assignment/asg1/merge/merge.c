#include "../default.h"

int get_storage_table_info(){
    DIR *dir;
    struct dirent *ent;
    mkdir( "storage_table", 0755);
    dir = opendir ("./storage_table");
    if (dir != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            printf ("%s\n", ent->d_name);
            if (strcmp(".", ent->d_name) == 0) {
                printf("its .\n");
            }
        }
        closedir (dir);
    } else {
         /* could not open directory */
         perror ("");
         printf("no open!\n");
        return EXIT_FAILURE;
    }
    return 0;
}

int get_storage_table_number() {
    /* storage table number in directory */
    struct dirent *ent;
    DIR *dir = opendir ("./storage_table");
    if (dir == NULL) {
        printf("cannot open storage_table\n");
        return 0;
    }
    int table_num = 0;
    while((ent = readdir(dir)) != NULL) {
        // . or .. is not storage table
        if ((strcmp(".", ent->d_name) == 0) || (strcmp("..", ent->d_name) == 0)) continue;
        table_num++;
    }
    return table_num;
}

int get_new_storage_table_index() {
    /* new storage index (ex. next index of 1.sst is 2)*/
    mkdir("storage_table", 0755);
    struct dirent *ent;
    DIR *dir;
    dir = opendir ("./storage_table");
    if (dir == NULL) {
        printf("cannot open storage_table\n");
        return 0;
    }
    int table_num = 0;
    int max_index = 0;
    char *ptr;
    while((ent = readdir(dir)) != NULL) {
        // . or .. is not storage table
        if ((strcmp(".", ent->d_name) == 0) || (strcmp("..", ent->d_name) == 0)) continue;
        table_num++;
        ptr = strtok(ent->d_name, ".");
        max_index = MAX(max_index, atoi(ptr));
    }
    return max_index+1;
}

void create_new_storage_table_with_kvs(){
    int new_table_idx = get_new_storage_table_index();
    char filename[MAX_TABLE_NAME_SIZE] = {'\0',};
    sprintf(filename, "./storage_table/%d.stt", new_table_idx);
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("cannot create file!\n");
        return;
    }
    int i;
    for(i=1;i<=shm_merge->num;i++){
        printf("write %d\n", i);
        fprintf(fp, "%d %d %s\n", i, shm_merge->keys[i-1], shm_merge->values[i-1]);
    }
    fclose(fp);
    return;
}

void flush_kvs() {
    kvs->num = 0;
    int i;
    for(i=0;i<MAX_KVS_NUMBER;i++){
        kvs->keys[i] = 0;
        memset(kvs->values[i], 0, sizeof(kvs->values[i]));
    }
    kvs->get_request_idx = -1;
    kvs->request_done = false;
    kvs->is_full =false;
    return;
}

void merge_process() {
    mkdir("storage_table", 0755);
    while(true) {
        switch(shm_merge->request){
            case REQ_NONE:
                break;
            case REQ_FLUSH:
                printf("REQ_FLUSH START\n");
                //get_storage_table_info();
                create_new_storage_table_with_kvs();
                printf("NOW FLUSH\n");
                flush_kvs();
                shm_merge->request = REQ_NONE;
                printf("REQ_FLUSH DONE[MERGE]\n");
                semop(sem_kvs_id, &v_kvs[0], 1);
                break;
            case REQ_MERGE:
                break;
            default:
                break;
        }
    }
    return;
}