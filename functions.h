#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "manolib.h"

string hashas(const std::string &input) {
    std::array<uint64_t, 4> state = {
        0xabcdef1234567890ULL,
        0xfedcba9876543210ULL,
        0xa12b34c56d78e90fULL,
        0xa1b2c3d4e5f60708ULL
    };
    // geresnis maisymas (su AI pagalba)
    for (size_t i = 0; i < input.size(); ++i) {
        uint8_t byte = static_cast<uint8_t>(input[i]);

        // basic mixing
        state[i % 4] ^= (static_cast<uint64_t>(byte) * 0x0101010101010101ULL);
        state[(i + 1) % 4] += (static_cast<uint64_t>(byte) << ((i * 7) % 56));

        // inline rotation left: (x << r) | (x >> (64 - r))
        int r = i % 63;
        uint64_t x = static_cast<uint64_t>(byte) * 0x9e3779b97f4a7c15ULL;
        state[(i + 2) % 4] ^= ((x << r) | (x >> (64 - r)));
    }

    // kiekvienas state paveikia kitus
    for (int i = 0; i < 4; i++) {
        state[i] ^= state[(i + 1) % 4];
        state[i] *= 3;
    }
    // dar daugiau sumaisymo tarp states
    for (int i = 0; i < 4; i++) {
        state[i] = state[i] + state[(i + 2) % 4];
        state[i] = state[i] ^ (state[i] >> 16);
    }

    // convert 4x64bit = 256bit i 64 simboliu hex stringa
    std::ostringstream of;
    for (uint64_t v : state) {
        of << std::hex << std::setw(16) << std::setfill('0') << v;
    }
    return of.str();
}

double generateAmount()
{
    return 100 + rand() % (1000000 - 100 + 1);
}

string generateName()
{
    string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string result = Names[rand() % 50];
    int x= rand()%20;
    
    for (int i = 0; i < x; i++) {
        int randomIndex = rand() % chars.length();  
        result += chars[randomIndex];
    }

    return result;
}

int generateUsers(vector<User> &users){
    const int target = 1000;
    for (int i = 0; i < target; ++i) {
        string name = generateName();
        double balance = generateAmount();
        string public_key = hashas(name);

        users.push_back(User(name, balance, public_key));
    }

    cout << "Total users generated: " << users.size() << endl;
    return static_cast<int>(users.size());
}

int generateTransactions(vector<User> &users, vector<Transaction> &transactions, int numTransactions)
{
    if (users.size() < 2) return 0;

    int created = 0;
    int n = static_cast<int>(users.size());

    for (int t = 0; t < numTransactions; ++t) {
        // pick random sender and receiver
        int sender = rand() % n;
        int receiver = rand() % n;
        if (receiver == sender) {
            receiver = (sender + 1 + (rand() % (n-1))) % n;
        }

        double senderBalance = users[sender].getBalance();
        if (senderBalance < 1.0) continue; // skip if no funds

        // choose amount: between 1 and min(senderBalance, some upper limit)
        double maxTransfer = std::min(senderBalance, 100000.0);
        double amount = 1 + (rand() % static_cast<int>(maxTransfer));

        // update balances
        //users[sender].setBalance(senderBalance - amount);
        //users[receiver].setBalance(users[receiver].getBalance() + amount);

        // create transaction id from hash of senderkey+receiverkey+amount+time
        std::ostringstream idin;
        idin << users[sender].getPublic_key() << users[receiver].getPublic_key() << amount << time(0) << t;
        string txid = hashas(idin.str());

        bool verified= false;
        transactions.emplace_back(txid, users[sender].getPublic_key(), users[receiver].getPublic_key(), amount, verified);
        ++created;
    }
return created;
}

#endif