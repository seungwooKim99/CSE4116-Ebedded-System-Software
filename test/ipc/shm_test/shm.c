#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <errno.h> 
#include <sys/shm.h> 
#include <time.h>

int main() { 
    key_t key; 
    int shmid, i;
    char *shmaddr; 
   
    shmid = shmget(IPC_PRIVATE, 10, IPC_CREAT|0644); 
    if(shmid == -1){ 
        perror("shmget"); 
        exit(1); 
    } 
  
    switch(fork()) { 
        case -1: 
            perror("fork"); 
            exit(1); 
            break; 
        case 0:
            usleep(1000000);
            shmaddr = (char *)shmat(shmid, (char *)NULL, 0); 
            printf("Child Process =====\n");
            for(i=0; i<10; i++) 
                shmaddr[i] = 'a' + i; 
            exit(0); 
            break; 
        default: 
            wait(0); 
            shmaddr = (char *)shmat(shmid, (char *)NULL, 0); 
            printf("Parent Process =====\n"); 
            for(i=0; i<10; i++) 
                printf("%c ", shmaddr[i]); 
            printf("\n"); 
            shmdt((char *)shmaddr); 
            shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL); 
            break; 
    }
}