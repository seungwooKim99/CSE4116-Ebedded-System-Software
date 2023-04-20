#ifndef MERGE_HEADER_INCLUDE
#define MERGE_HEADER_INCLUDE

/* util */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MAX_TABLE_NAME_SIZE 20
#define MAX_STORAGE_DATA_NUM 10000
#define MAX_KEY_VALUE 10000

/* file */
typedef struct _stroage_file {
    int num;
    int keys[MAX_STORAGE_DATA_NUM];
    char values[MAX_STORAGE_DATA_NUM][LCD_MAX_BUFF];
    char file_name[MAX_TABLE_NAME_SIZE];
} storage_file;

/* merge process */
void merge_process();

/* util function */
int compare_asc(const void *a, const void *b);

/* storage table operations */
int get_storage_table_number();
int get_new_storage_table_index();
void create_new_storage_table_with_kvs();
void get_table_names(char files[3][MAX_TABLE_NAME_SIZE]);
void merge_storage_tables();

/* kvs operation */
void flush_kvs();


#endif