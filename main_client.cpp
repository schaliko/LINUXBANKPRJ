#include "bank.h"
#include <iostream>
#include <sstream>
#include <vector>

int main() {
    Bank bank(10, 1000);  // Create a bank with 10 accounts and maximum balance of 1000

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) {
            continue;
        }

        std::string command = tokens[0];
        if (command == "balance") {
            if (tokens.size() < 2) {
                std::cout << "Invalid syntax: balance <accountNum>" << std::endl;
                continue;
            }
            int accountNum = std::stoi(tokens[1]);
            bank.printBalance(accountNum);
        } else if (command == "freeze") {
            if (tokens.size() < 2) {
                std::cout << "Invalid syntax: freeze <accountNum>" << std::endl;
                continue;
            }
            int accountNum = std::stoi(tokens[1]);
            bank.freezeAccount(accountNum);
            std::cout << "Account " << accountNum << " frozen." << std::endl;
        } else if (command == "unfreeze") {
            if (tokens.size() < 2) {
                std::cout << "Invalid syntax: unfreeze <accountNum>" << std::endl;
                continue;
            }
            int accountNum = std::stoi(tokens[1]);
            bank.unfreezeAccount(accountNum);
            std::cout << "Account " << accountNum << " unfrozen." << std::endl;
        } else if (command == "transfer") {
            if (tokens.size() < 4) {
                std::cout << "Invalid syntax: transfer <fromAccount> <toAccount> <amount>" << std::endl;
                continue;
            }
            int fromAccount = std::stoi(tokens[1]);
            int toAccount = std::stoi(tokens[2]);
            int amount = std::stoi(tokens[3]);
            bank.transfer(fromAccount, toAccount, amount);
        } else if (command == "credit") {
            if (tokens.size() < 2) {
                std::cout << "Invalid syntax: credit <amount>" << std::endl;
                continue;
            }
            int amount = std::stoi(tokens[1]);
            bank.credit(amount);
        } else if (command == "debit") {
            if (tokens.size() < 2) {
                std::cout << "Invalid syntax: debit <amount>" << std::endl;
                continue;
            }
            int amount = std::stoi(tokens[1]);
            bank.debit(amount);
        } else if (command == "setmin") {
            if (tokens.size() < 3) {
                std::cout << "Invalid syntax: setmin <accountNum> <minBalance>" << std::endl;
                continue;
            }
            int accountNum = std::stoi(tokens[1]);
            int minBalance = std::stoi(tokens[2]);
            bank.setMinBalance(accountNum, minBalance);
            std::cout << "Minimum balance for account " << accountNum << " set to " << minBalance << std::endl;
        } else if (command == "setmax") {
            if (tokens.size() < 3) {
                std::cout << "Invalid syntax: setmax <accountNum> <maxBalance>" << std::endl;
                continue;
            }
            int accountNum = std::stoi(tokens[1]);
            int maxBalance = std::stoi(tokens[2]);
            bank.setMaxBalance(accountNum, maxBalance);
            std::cout << "Maximum balance for account " << accountNum << " set to " << maxBalance << std::endl;
        } else if (command == "quit") {
            break;
        } else {
            std::cout << "Invalid command: " << command << std::endl;
        }
    }

    return 0;
}

// g++ -o client main_client.cpp bank.cpp -lrt -lpthread
