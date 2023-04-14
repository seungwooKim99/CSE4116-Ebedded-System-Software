#ifndef IPC_HEADER_INCLUDE
#define IPC_HEADER_INCLUDE

/* mode */
typedef enum _ctl_mode { PUT, GET, MERGE } ctl_mode;

/* shared memory */
#define SHM_KEY_1 (key_t) 0x10
#define SHM_KEY_2 (key_t) 0x20
// I/O -> main
typedef struct _shmIOtoMain {
    ctl_mode mode;
    unsigned char key[FND_MAX_DIGIT];
    unsigned char value[SW_MAX_BUTTON];
    int control_key;
} shmIOtoMain;

shmIOtoMain *shmIOtoMainBuffer;
int shmIOtoMain_id;


/* semaphore */
#define SEM_KEY (key_t) 0x20
#define SEM_NUMBER 2
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