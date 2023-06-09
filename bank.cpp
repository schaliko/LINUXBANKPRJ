#include "bank.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

// Constants for shared memory and semaphore
// const int SHM_KEY = 1234;   // Shared memory key
// const int SEM_KEY = 5678;   // Semaphore key


// Bank constructor
Bank::Bank(Account* bankData, int numAccounts, int maxBalance) : bankData_(bankData), numAccounts_(numAccounts), maxBalance_(maxBalance) {
    // Initialize the account data
    for (int i = 0; i < numAccounts_; ++i) {
        bankData_[i].balance = 0;
        bankData_[i].minBalance = 0;
        bankData_[i].maxBalance = maxBalance_;
        bankData_[i].frozen = false;
    }

    // Initialize semaphores
    initializeSemaphores();
}

// Helper function to initialize semaphores
void Bank::initializeSemaphores() {
    std::string shmFile = "shm_file";
    // Generate a key using ftok
    key_t semKey = ftok(shmFile.c_str(), 'A');
    if (semKey == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // create semaphores
    // int sem_id = semget(IPC_PRIVATE, numAccounts_, IPC_CREAT | 0644);
    int sem_id = semget(semKey, numAccounts_, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // Initialize the semaphore value to 1
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
}

// Bank destructor
Bank::~Bank() {
    int shmId_;

    // Detach from the shared memory segment
    shmdt(bankData_);

    // Destroy the shared memory segment
    shmctl(shmId_, IPC_RMID, nullptr);

    // Destroy the semaphore
    semctl(semId_, 0, IPC_RMID);
}


// Helper function to perform semaphore P operation
void Bank::semaphoreP(int accountNum) {
    struct sembuf sb;
    sb.sem_num = accountNum;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(semId_, &sb, 1);
}

// Helper function to perform semaphore V operation
void Bank::semaphoreV(int accountNum) {
    struct sembuf sb;
    sb.sem_num = accountNum;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop(semId_, &sb, 1);
}

// Helper function to access account data safely using semaphores
Account& Bank::getAccount(int accountNum) {
    semaphoreV(accountNum);
    Account& account = bankData_[accountNum];
    semaphoreP(accountNum);
    return account;
}

int Bank::getNumAccounts(){
        return numAccounts_;
    }

int Bank::getMaxBalance(){
        return numAccounts_;
}

// Helper function to validate account number
bool Bank::isValidAccount(int accountNum) {
    return (accountNum >= 0) && (accountNum < numAccounts_);
}

// Print the balance of an account
void Bank::printBalance(int accountNum) {
    semaphoreV(accountNum);
    
    if (!isValidAccount(accountNum)) {
        std::cout << "Invalid account number." << std::endl;
        return;
    }
    system("clear");
    Account& account = getAccount(accountNum);
    std::cout << "Account " << accountNum << std::endl << "Balance: " << account.balance << std::endl << "MinBalance: " << account.minBalance << std::endl << "MaxBalance: " << account.maxBalance << std::endl << "Status: ";
    if(account.frozen == true){
        std::cout << "frozen\n";
    }
    else{
        std::cout << "not frozen\n";
    }
    semaphoreP(accountNum);
}

// Freeze an account
void Bank::freezeAccount(int accountNum) {
    semaphoreV(accountNum);
    if (!isValidAccount(accountNum)) {
        std::cout << "Invalid account number." << std::endl;
        return;
    }

    Account& account = getAccount(accountNum);
    if(account.frozen == true){
                system("clear");
                std::cout << "Account already frozen\n";
                return;
    }
    account.frozen = true;
    system("clear");
    std::cout << "Account has been frozen\n";
    semaphoreP(accountNum);
}

// Unfreeze an account
void Bank::unfreezeAccount(int accountNum) {
    semaphoreV(accountNum);
    if (!isValidAccount(accountNum)) {
        std::cout << "Invalid account number." << std::endl;
        return;
    }
    Account& account = getAccount(accountNum);
    if(account.frozen == false){
        system("clear");
        std::cout << "*Account already unfrozen*\n";
        return;
    }
    account.frozen = false;
    system("clear");
    std::cout << "*Account has been unfrozen*\n";
    semaphoreP(accountNum);
}

// Transfer amount from one account to another
void Bank::transfer(int fromAccount, int toAccount, int amount) {
    
    semaphoreV(fromAccount);
    semaphoreV(toAccount);
    if (!isValidAccount(fromAccount) || !isValidAccount(toAccount)) {
        std::cout << "Invalid account number." << std::endl;
        return;
    }
    Account& from = getAccount(fromAccount);
    Account& to = getAccount(toAccount);

    if (from.frozen || to.frozen) {
        std::cout << "Error: One or both accounts are frozen." << std::endl;
        return;
    }

    if (from.balance < amount) {
        std::cout << "Error: Insufficient balance." << std::endl;
        return;
    }


    from.balance -= amount;
    to.balance += amount;
    system("clear");
    std::cout << "Transferred: " << amount << "$ from account: "<< fromAccount << " to account: " << toAccount << std::endl;
    semaphoreP(fromAccount);
    semaphoreP(toAccount);
}


// Credit amount to all accounts
void Bank::credit(int amount) {
    for(int i = 0; i < numAccounts_; ++i){
        semaphoreV(i);
    }
    system("clear");
    for (int i = 0; i < numAccounts_; ++i) {
        Account& account = getAccount(i);
        if(account.frozen == true){
            std::cout << "*Account: " << i << " frozen, SKIPPING\n";
            continue;
        }
        if(account.balance + amount > account.maxBalance && account.frozen != true){ // second argument for additional protection
                std::cout << "Reached maximum balance limit for account:" << i << "  SKIPPING\n";
                continue;
        }
        account.balance += amount;
    }
    std::cout << "Credit successful." << std::endl;
    for(int i = 0; i < numAccounts_; ++i){
        semaphoreP(i);
    }
}

// Debit amount from all accounts
void Bank::debit(int amount) {
    for(int i = 0; i < numAccounts_; ++i){
        semaphoreV(i);
    }
    system("clear");
    for (int i = 0; i < numAccounts_; ++i) {
        Account& account = getAccount(i);
        if(account.frozen == true){
            std::cout << "Account: " << i << " frozen, SKIPPING\n";
            continue;
        }
        if(account.balance - amount < account.maxBalance && account.frozen != true){ // second argument for additional protection
                std::cout << "Reached minimum balance limit for account:" << i << "  SKIPPING\n";
                continue;
        }
        account.balance -= amount;
    }
    std::cout << "Debit successful." << std::endl;
    for(int i = 0; i < numAccounts_; ++i){
        semaphoreP(i);
    }
}

// Set the minimum balance for an account
void Bank::setMinBalance(int accountNum, int minBalance) {
    semaphoreV(accountNum);
    if (!isValidAccount(accountNum)) {
        std::cout << "Invalid account number." << std::endl;
        return;
    }
    Account& account = getAccount(accountNum);
    if(account.balance < minBalance){
        std::cout << "FAILED: balance is less than new minimum balance\n";
        return;
    }
    account.minBalance = minBalance;
    system("clear");
    std::cout << "MAXIMUM BALANCE SET SUCCESSFULLY\n";
    semaphoreP(accountNum);
}

// Set the maximum balance for an account
void Bank::setMaxBalance(int accountNum, int maxBalance) {
    semaphoreV(accountNum);
    if (!isValidAccount(accountNum)) {
        std::cout << "Invalid account number." << std::endl;
        return;
    }
    Account& account = getAccount(accountNum);
    if(account.balance > maxBalance){
        std::cout << "FAILED: balance is more than new minimum balance\n";
        return;
    }
    account.maxBalance = maxBalance;
    system("clear");
    std::cout << "MAXIMUM BALANCE SET SUCCESSFULLY\n";
    semaphoreP(accountNum);
}