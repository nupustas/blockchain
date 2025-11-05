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

int generateUsers(vector<User> &users, int Ucount){
    for (int i = 0; i < Ucount; ++i) {
        string name = generateName();
        double balance = generateAmount();
        string public_key = hashas(name);

        users.push_back(User(name, balance, public_key));
    }

    return static_cast<int>(users.size());
}

int generateTransactions(vector<User> &users, vector<Transaction> &transactions, int numTransactions)
{
    int n = static_cast<int>(users.size());

    vector<double> tempBalances;// available user balances 
    tempBalances.reserve(n);
    for (const auto &u : users)
        tempBalances.push_back(u.getBalance());

    for (int t = 0; t < numTransactions; ++t) {
        // choose sender with enough funds
        int sender = -1;
        for (int attempts = 0; attempts < 10; ++attempts) {
            int idx = rand() % n;
            if (tempBalances[idx] > 1.0) { // require at least $1
                sender = idx;
                break;
            }
        }
        if (sender == -1) continue; // no valid sender found

        // choose receiver different from sender
        int receiver = rand() % n;
        if (receiver == sender) receiver = (receiver + 1 + rand() % (n - 1)) % n;

        double senderBalance = tempBalances[sender];
        double maxTransfer = std::max(1.0, senderBalance / 2); // max 1/2 balance
        double amount = (rand() % 1000) / 1000.0 * maxTransfer; 
        if (amount < 1.0) amount = 1.0; // min 1.0

        // "spend" from sender in temporary balance
        tempBalances[sender] -= amount;
        tempBalances[receiver] +=amount;

        // create transaction id
        std::ostringstream idin;
        idin << users[sender].getPublic_key()
             << users[receiver].getPublic_key()
             << amount << time(0) << t;
        string txid = hashas(idin.str());

        transactions.emplace_back(
            txid,
            users[sender].getPublic_key(),
            users[receiver].getPublic_key(),
            amount,
            false
        );
    }
    return static_cast<int>(transactions.size());
}


#endif