#include "bank.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

// Constants for shared memory and semaphore
// const int SHM_KEY = 1234;   // Shared memory key
const int SEM_KEY = 5678;   // Semaphore key

// Bank constructor
Bank::Bank(int numAccounts, int maxBalance) : numAccounts_(numAccounts), maxBalance_(maxBalance) {
    initializeSharedMemory();
    initializeSemaphores();
}

// Bank destructor
Bank::~Bank() {
    // Detach from the shared memory segment
    shmdt(bankData_);

    // Destroy the shared memory segment
    shmctl(shmId_, IPC_RMID, nullptr);

    // Destroy the semaphore
    semctl(semId_, 0, IPC_RMID);
}


// Helper function to initialize shared memory
void Bank::initializeSharedMemory() {
    // Generate a unique key for shared memory using ftok
    key_t shmKey = ftok("shm_file", 'A');
    if (shmKey == -1) {
        perror("ftok");
        exit(1);
    }

    // Create a new shared memory segment
    shmId_ = shmget(shmKey, numAccounts_ * sizeof(Account), IPC_CREAT | 0666);
    if (shmId_ == -1) {
        perror("shmget");
        exit(1);
    }

    // Attach to the shared memory segment
    bankData_ = (Account*)shmat(shmId_, nullptr, 0);
    if (bankData_ == (Account*)-1) {
        perror("shmat");
        exit(1);
    }

    // Initialize the account data
    for (int i = 0; i < numAccounts_; ++i) {
        bankData_[i].balance = 0;
        bankData_[i].minBalance = 0;
        bankData_[i].maxBalance = maxBalance_;
        bankData_[i].frozen = false;
    }
}

// Helper function to initialize semaphores
void Bank::initializeSemaphores() {
    // Generate a key using ftok
    key_t semKey = ftok(".", 'S');
    if (semKey == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Create a new semaphore
    semId_ = semget(semKey, 1, IPC_CREAT | 0666);
    if (semId_ == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // Initialize the semaphore value to 1
    if (semctl(semId_, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
}

// Helper function to perform semaphore P operation
void Bank::semaphoreP() {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop(semId_, &sb, 1);
}

// Helper function to perform semaphore V operation
void Bank::semaphoreV() {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(semId_, &sb, 1);
}

// Helper function to access account data safely using semaphores
Account& Bank::getAccount(int accountNum) {
    semaphoreP();
    Account& account = bankData_[accountNum];
    semaphoreV();
    return account;
}

// Print the balance of an account
void Bank::printBalance(int accountNum) {
    Account& account = getAccount(accountNum);
    std::cout << "Account " << accountNum << std::endl << "Balance: " << account.balance << std::endl << "MinBalance: " << account.minBalance << std::endl << "MaxBalance: " << account.maxBalance << std::endl << "Status: " << account.frozen;
}

// Freeze an account
void Bank::freezeAccount(int accountNum) {
    Account& account = getAccount(accountNum);
    account.frozen = true;
}

// Unfreeze an account
void Bank::unfreezeAccount(int accountNum) {
    Account& account = getAccount(accountNum);
    account.frozen = false;
}

// Transfer amount from one account to another
void Bank::transfer(int fromAccount, int toAccount, int amount) {
    Account& from = getAccount(fromAccount);
    Account& to = getAccount(toAccount);

    if (from.frozen || to.frozen) {
        std::cout << "Error: Account is frozen." << std::endl;
        return;
    }

    if (from.balance < amount) {
        std::cout << "Error: Insufficient balance." << std::endl;
        return;
    }

    from.balance -= amount;
    to.balance += amount;
    std::cout << "Transfer successful." << std::endl;
}

// Credit amount to all accounts
void Bank::credit(int amount) {
    for (int i = 0; i < numAccounts_; ++i) {
        Account& account = getAccount(i);
        account.balance += amount;
    }
    std::cout << "Credit successful." << std::endl;
}

// Debit amount from all accounts
void Bank::debit(int amount) {
    for (int i = 0; i < numAccounts_; ++i) {
        Account& account = getAccount(i);
        if (account.balance < amount) {
            std::cout << "Error: Insufficient balance in account " << i << std::endl;
            return;
        }
        account.balance -= amount;
    }
    std::cout << "Debit successful." << std::endl;
}

// Set the minimum balance for an account
void Bank::setMinBalance(int accountNum, int minBalance) {
    Account& account = getAccount(accountNum);
    account.minBalance = minBalance;
}

// Set the maximum balance for an account
void Bank::setMaxBalance(int accountNum, int maxBalance) {
    Account& account = getAccount(accountNum);
    account.maxBalance = maxBalance;
}