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

    // Read numAccounts from shared memory
    int* numAccountsPtr = (int*)shmat(shmId, nullptr, 0);
    if (numAccountsPtr == (int*)-1) {
        perror("shmat");
        exit(1);
    }


    int numAccounts = *numAccountsPtr;
    // Detach from the shared memory segment
    shmdt(numAccountsPtr);

    // Remove the shared memory segment
    if (shmctl(shmId, IPC_RMID, nullptr) < 0) {
        perror("shmctl");
        exit(errno);
    }

    for(int i = 0; i < numAccounts; ++i){
        // Remove the semaphores associated with the shared memory segment
        int semId = semget(shmKey + i, 1, IPC_CREAT | 0666);
        if (semId == -1) {
            perror("semget");
            exit(1);
        }

        if (semctl(semId, 0, IPC_RMID) < 0) {
            perror("semctl");
            exit(errno);
        }

    }
    return 0;
}
