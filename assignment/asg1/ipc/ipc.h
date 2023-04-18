#ifndef IPC_HEADER_INCLUDE
#define IPC_HEADER_INCLUDE

/* mode */
typedef enum _ctl_mode { PUT, GET, MERGE } ctl_mode;
typedef enum _request_mode { REQ_NONE, REQ_FLUSH, REQ_MERGE } request_mode;
/* shared memory */
#define SHM_KEY_1 (key_t) 0x10
#define SHM_KEY_2 (key_t) 0x20
#define SHM_KEY_3 (key_t) 0x30


/* I/O -> main */
typedef struct _shmIOtoMain {
    ctl_mode mode;
    int key;
    char value[LCD_MAX_BUFF];
    int control_key;
    bool request;
} shmIOtoMain;
shmIOtoMain *shmIOtoMainBuffer;
int shmIOtoMain_id;

/* Key Value store */
#define MAX_KVS_NUMBER 3
typedef struct _shmKVS {
    int num;
    int keys[MAX_KVS_NUMBER];
    char values[MAX_KVS_NUMBER][LCD_MAX_BUFF];
    int get_request_idx;
    bool request_done;
    bool is_full;
} shmKVS;
shmKVS *kvs;
int kvs_id;

/* I/O <-> merge */
typedef struct _shmMerge {
    request_mode request;
    int num;
    int keys[MAX_KVS_NUMBER];
    char values[MAX_KVS_NUMBER][LCD_MAX_BUFF];
} shmMerge;
shmMerge *shm_merge;
int shmMerge_id;


/* semaphore */
#define SEM_KEY (key_t) 0x20
#define SEM_KEY_KVS (key_t) 0x50
#define SEM_NUMBER 2
#define READ 0
#define WRITE 1
#define KVS 2
int sem_id;
int sem_kvs_id;
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
struct sembuf p[SEM_NUMBER], v[SEM_NUMBER];
struct sembuf p_kvs[SEM_NUMBER], v_kvs[SEM_NUMBER];

/* function prototypes */
void init_shared_memory();
void init_semaphore();
#endif