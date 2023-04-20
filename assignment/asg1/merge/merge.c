#include "../default.h"

/* util functions */
int compare_asc(const void *a, const void *b) {
    /* compare filename in ascending order (ex. 1.stt < 2.stt)*/
    int num_a = atoi(*(char**)a);
    int num_b = atoi(*(char**)b);
    if (num_a < num_b) {
        return -1;
    } else if (num_a > num_b) {
        return 1;
    } else {
        return 0;
    }
}

/* storage table operations */
int get_storage_table_number() {
    mkdir("storage_table", 0755);
    /* storage table number in directory */
    struct dirent *ent;
    DIR *dir;
    dir = opendir ("./storage_table");
    if (dir == NULL) {
        printf("cannot open storage_table!!\n");
        return 0;
    }
    int table_num = 0;
    while((ent = readdir(dir)) != NULL) {
        // . or .. is not storage table
        if ((strcmp(".", ent->d_name) == 0) || (strcmp("..", ent->d_name) == 0)) continue;
        table_num++;
    }
    closedir(dir);
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
    closedir(dir);
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
    kvs->is_full =false;
    return;
}

void get_table_names(char files[3][MAX_TABLE_NAME_SIZE]){
    mkdir("storage_table", 0755);
    struct dirent *ent;
    DIR *dir;
    dir = opendir ("./storage_table");
    if (dir == NULL) {
        printf("cannot open storage_table\n");
        return;
    }
    char *ptr;
    int idx = 0;
    while((ent = readdir(dir)) != NULL) {
        // . or .. is not storage table
        if ((strcmp(".", ent->d_name) == 0) || (strcmp("..", ent->d_name) == 0)) continue;
        strcpy(files[idx++], ent->d_name);
    }
    closedir(dir);
    return;
}

void merge_storage_tables(){
    int i, j;
    int merged_table_idx = get_new_storage_table_index();
    int table_num = get_storage_table_number();

    /* get tables name */
    char files_arr[3][MAX_TABLE_NAME_SIZE] = {{'\0',},{'\0',},{'\0',}};
    get_table_names(files_arr);
    
    /* sort by table name number (ex. 1.stt < 2.stt) */
    char *files[3];
    for (i = 0 ; i < 3 ; i ++) {
        files[i] = files_arr[i];
    }
    qsort(files, table_num, sizeof(char *), compare_asc);

    /* save past 2 tables info in structure */
    storage_file targets[2];
    targets[0].num = targets[1].num = 0;
    FILE *fp;
    char read_buf[100];
    char *line, *word;
    for (i=0;i<2;i++){
        char curr_filename[MAX_TABLE_NAME_SIZE] = {'\0',};
        sprintf(curr_filename, "./storage_table/%s", files[i]);
        fp = fopen(curr_filename, "r");
        if (!fp) {
            printf("file open error : %s\n", files[i]);
            return;
        }
        strcpy(targets[i].file_name, files[i]);
        while (!feof(fp)) {
            int idx = targets[i].num;
            line = fgets(read_buf, 100, fp);
            if (!line) {
                break;
            }
            word = strtok(line, " "); // order of data
            word = strtok(NULL, " "); // key
            targets[i].keys[idx] = atoi(word);
            word = strtok(NULL, " "); // value
            strcpy(targets[i].values[idx], word);
            targets[i].num++;
        }
        fclose(fp);
    }

    /* merge two tables */
    storage_file merged;
    memset(merged.keys, 0, sizeof(merged.keys)); // initialize
    for(i=0;i<2;i++) {
        int num = targets[i].num;
        for(j=0;j<num;j++){
            int key = targets[i].keys[j];
            merged.keys[key] = key;
            strcpy(merged.values[key], targets[i].values[j]);
        }
    }

    /* write */
    int total_num = 0;
    char merged_filename[MAX_TABLE_NAME_SIZE] = {'\0',};
    sprintf(merged_filename, "./storage_table/%d.stt", merged_table_idx);
    fp = fopen(merged_filename, "w");
    int order = 1;
    for(i=0;i<MAX_KEY_VALUE;i++){
        if(merged.keys[i] == 0) continue;
        fprintf(fp,"%d %d %s", order++, merged.keys[i], merged.values[i]);
        total_num++;
    }
    fclose(fp);

    /* remove old 2 tables */
    int remove_res;
    char remove_filename[MAX_TABLE_NAME_SIZE] = {'\0',};
    for(i=0;i<2;i++){
        sprintf(remove_filename, "./storage_table/%s", files[i]);
        if ((remove_res = remove(remove_filename)) == -1) {
            printf("remove failed! : %s\n", remove_filename);
        }
    }

    /* show result */
    char merge_result_buf[LCD_MAX_BUFF] = {'\0'};
    sprintf(merge_result_buf, "%d.stt %d", merged_table_idx, total_num);
    usleep(500000); //wait for 0.5s
    write_motor(true);
    write_lcd(merge_result_buf);
    usleep(1000000); // show result for 1s
    write_motor(false);
    return;
}

void merge_process() {
    mkdir("storage_table", 0755);
    while(true) {
        switch(shm_merge->request){
            case REQ_NONE:
                break;
            case REQ_FLUSH:
                create_new_storage_table_with_kvs();
                flush_kvs();
                
                if (get_storage_table_number() == 3) {
                    /* merge required (table number == 3) */
                    shm_merge->request = REQ_MERGE;
                } else {
                    shm_merge->request = REQ_NONE;
                    semop(sem_kvs_id, &v_kvs[0], 1); //work done. unlock.
                    break;
                }
            case REQ_MERGE:
                if (get_storage_table_number() >= 2) {
                    /* merge possible if table is more then 2 */
                    merge_storage_tables();
                    shm_merge->request = REQ_NONE;
                }
                semop(sem_kvs_id, &v_kvs[0], 1); //work done. unlock.
                break;
            default:
                break;
        }
    }
    return;
}