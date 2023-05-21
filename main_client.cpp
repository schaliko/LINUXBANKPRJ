#include "bank.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sstream>
#include <vector>

int main() {

    std::string shmFile = "shm_file";
    key_t shmKey = ftok(shmFile.c_str(), 'A');
    if (shmKey == -1) {
        perror("ftok");
        exit(1);
    }

    int shmId = shmget(shmKey, sizeof(Bank), IPC_CREAT | 0666);
    if (shmId == -1) {
        perror("shmget");
        exit(1);
    }

    Account* bankData = (Account*)shmat(shmId, nullptr, 0);
    if (bankData == (Account*)-1) {
        perror("shmat");
        exit(1);
    }

    Bank bank(bankData, 10, 1000);  // Create a bank with 10 accounts and maximum balance of 1000

//     Bank* bank = static_cast<Bank*>(shmat(shmId, nullptr, 0));
//     if (bank == reinterpret_cast<Bank*>(-1)) {
//         perror("shmat");
//         exit(errno);
//     }

    std::string line;
    while (true) {
        std::cout << "\n1) Display the current/minimum/maximum account balance\n";
        std::cout << "2) Freeze an account\n";
        std::cout << "3) Unfreeze an account\n";
        std::cout << "4) Transfer X amount of money from account A to account B (X > 0)\n";
        std::cout << "5) Add X units from all accounts\n";
        std::cout << "6) Remove X units from all accounts\n";
        std::cout << "7) Set the minimum possible balance X for account A\n";
        std::cout << "8) Set the maximum possible balance X for account A\n";
        std::cout << "9) Exit\n";
        std::cout << "Enter option: \n";
        std::cin >> line;
        std::cout << std::endl;
        if (line == "1") {
            int account_number;
            std::cout << "Enter the account number:\n";
            std::cin >> account_number;
            bank.printBalance(account_number);


        }   else if(line == "2"){
                int account_number;
                std::cout << "Enter the account number:\n";
                std::cin >> account_number;
                bank.freezeAccount(account_number);     

        }   else if(line == "3"){
                int account_number;
                std::cout << "Enter the account number:\n";
                std::cin >> account_number;
                bank.unfreezeAccount(account_number);     

        }   else if(line == "4"){
                int from_account, to_account, amount;
                std::cout << "Enter from, to and the amount:\n";
                std::cin >> from_account >> to_account >> amount;
                bank.transfer(from_account, to_account, amount);
        }   else if(line == "5"){
                int amount;
                std::cin >> amount;
                bank.credit(amount);
        }   else if(line == "6"){
                int amount;
                std::cin >> amount;
                bank.debit(amount);
        }   else if(line == "7"){
                int account_number;
                int min_balance;
                std::cin >> account_number;
                std::cin >> min_balance;
                bank.setMinBalance(account_number, min_balance);
        }   else if(line == "8"){
                int account_number;
                int max_balance;
                std::cin >> account_number;
                std::cin >> max_balance;
                bank.setMaxBalance(account_number, max_balance);
        }   else if(line == "9"){
                break;
        }
        else {
            std::cout << "Invalid command: " << line << std::endl;
        }
    }
    return 0;
}

