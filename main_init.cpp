#include "bank.h"
#include "iostream"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <numAccounts> <maxBalance>" << std::endl;
        return 1;
    }

    int numAccounts = std::stoi(argv[1]);
    int maxBalance = std::stoi(argv[2]);

    Bank bank(numAccounts, maxBalance);

    return 0;
}