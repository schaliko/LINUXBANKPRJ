#include "bank.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

int main() {
    std::string shmFile = "shm_file";
    key_t shmKey = ftok(shmFile.c_str(), 'A');
    if (shmKey == -1) {
        perror("ftok");
        exit(1);
    }

    int shmId = shmget(shmKey, 0, 0666);
    if (shmId == -1) {
        perror("shmget");
        exit(1);
    }


    int semId = semget(shmKey, 0, 0);
    if (semId == -1) {
        perror("semget");
        exit(1);
    }

    struct semid_ds semInfo;
    if (semctl(semId, 0, IPC_STAT, &semInfo) == -1) {
        perror("semctl");
        exit(1);
    }

    int numSemaphores = semInfo.sem_nsems;
    

    // Remove the shared memory segment
    if (shmctl(shmId, IPC_RMID, nullptr) < 0) {
        perror("shmctl");
        exit(errno);
    }


        // Remove the semaphores associated with the shared memory segment
        semId = semget(shmKey, numSemaphores, IPC_CREAT | 0666);
        if (semId == -1) {
            perror("semget");
            exit(1);
        }

        if (semctl(semId, 0, IPC_RMID) < 0) {
            perror("semctl");
            exit(errno);
        }

    // }
    return 0;
}
