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


int compare_asc(const void *a, const void *b);
#endif