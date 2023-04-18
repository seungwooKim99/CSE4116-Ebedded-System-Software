#ifndef IPC_HEADER_INCLUDE
#define IPC_HEADER_INCLUDE

/* mode */
typedef enum _ctl_mode { PUT, GET, MERGE } ctl_mode;

/* shared memory */
#define SHM_KEY_1 (key_t) 0x10
#define SHM_KEY_2 (key_t) 0x20

/* I/O -> main */
typedef struct _shmIOtoMain {
    ctl_mode mode;
    int key;
    char value[LCD_MAX_BUFF];
    int control_key;
    bool request;
} shmIOtoMain;

/* Key Value store */
#define MAX_KVS_NUMBER 3
typedef struct _shmKVS {
    int num;
    int keys[MAX_KVS_NUMBER];
    char values[MAX_KVS_NUMBER][LCD_MAX_BUFF];
    int get_request_idx;
} shmKVS;

shmKVS *kvs;
int kvs_id;
shmIOtoMain *shmIOtoMainBuffer;
int shmIOtoMain_id;


/* semaphore */
#define SEM_KEY (key_t) 0x20
#define SEM_NUMBER 2
#define READ 0
#define WRITE 1
int sem_id;
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
struct sembuf p[SEM_NUMBER], v[SEM_NUMBER];


/* function prototypes */
void init_shared_memory();
void init_semaphore();
#endif