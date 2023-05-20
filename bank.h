// #pragma once
#ifndef BANK
#define BANK

#include <sys/types.h>

// Structure representing an account
struct Account {
    int balance;
    int minBalance;
    int maxBalance;
    bool frozen;
};

// Bank class
class Bank {
public:
    Bank(int numAccounts, int maxBalance);
    ~Bank();

    void printBalance(int accountNum);
    void freezeAccount(int accountNum);
    void unfreezeAccount(int accountNum);
    void transfer(int fromAccount, int toAccount, int amount);
    void credit(int amount);
    void debit(int amount);
    void setMinBalance(int accountNum, int minBalance);
    void setMaxBalance(int accountNum, int maxBalance);

private:
    int numAccounts_;
    int maxBalance_;

    // Shared memory segment identifier
    int shmId_;

    // Pointer to the shared memory segment
    Account* bankData_;

    // Semaphore identifier
    int semId_;

    // Helper function to initialize shared memory
    void initializeSharedMemory();

    // Helper function to initialize semaphores
    void initializeSemaphores();

    // Helper function to perform semaphore P operation (Post)
    void semaphoreP();

    // Helper function to perform semaphore V operation (Wait)
    void semaphoreV();

    // Helper function to access account data safely using semaphores
    Account& getAccount(int accountNum);
};

#endif

// g++ -o deinit main_deinit.cpp bank.cpp -lrt -lpthread
