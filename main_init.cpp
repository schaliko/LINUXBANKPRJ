#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include "bank.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <numAccounts> <maxBalance>" << std::endl;
        return 1;
    }
    

    int numAccounts = std::stoi(argv[1]);
    int maxBalance = std::stoi(argv[2]);

    std::string shmFile = "shm_file";
    key_t shmKey = ftok(shmFile.c_str(), 'A');
    if (shmKey == -1) {
        perror("ftok");
        exit(1);
    }

    int shmId = shmget(shmKey, numAccounts * sizeof(Account), IPC_CREAT | 0666);
    if (shmId == -1) {
        perror("shmget");
        exit(1);
    }

    Account* bankData = (Account*)shmat(shmId, nullptr, 0);
    if (bankData == (Account*)-1) {
        perror("shmat");
        exit(1);
    }

    Bank bank(bankData, numAccounts, maxBalance);

    // Detach from the shared memory segment
    shmdt(bankData);

    return 0;
}
