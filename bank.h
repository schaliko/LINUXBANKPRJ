// #pragma once
#ifndef BANK
#define BANK

// Structure of an account
struct Account {
    int balance;
    int minBalance;
    int maxBalance;
    bool frozen;
};

// Bank class
class Bank {
public:
    Bank(Account* bankData, int numAccounts, int maxBalance);
    ~Bank();

    void printBalance(int accountNum);
    void freezeAccount(int accountNum);
    void unfreezeAccount(int accountNum);
    void transfer(int fromAccount, int toAccount, int amount);
    void credit(int amount);
    void debit(int amount);
    void setMinBalance(int accountNum, int minBalance);
    void setMaxBalance(int accountNum, int maxBalance);
    bool isValidAccount(int accountNum);
    // Pointer to the shared memory segment
    Account* bankData_;

    int getNumAccounts();

    int getMaxBalance();


private:
    int numAccounts_;
    int maxBalance_;

    // Semaphore identifier
    int semId_;

    // Helper function to initialize semaphores
    void initializeSemaphores();

    // Helper function to perform semaphore P operation (Post)
    void semaphoreP(int accountNum);

    // Helper function to perform semaphore V operation (Wait)
    void semaphoreV(int accountNum);

    // Helper function to access account data safely using semaphores
    Account& getAccount(int accountNum);
};

#endif


