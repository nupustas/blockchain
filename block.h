#pragma once

#include "manolib.h"
#include "transaction.h"
#include "functions.h"
#include <limits>
#include "user.h"   
#include <algorithm>
#include <omp.h>

class Block {
private:
    int index;
    string previous_hash;
    string merkle_root;
    uint64_t nonce;
    uint64_t extraNonce;   
    uint64_t timestamp; // block objekto sukurimo laikas
    uint64_t blockMinedTime;
    vector<Transaction> transactions;
    string block_hash;

    double mineTime = 0.0; // kiek laiko kasamas block

public:
    // construct a block from given transactions 
    Block(int id, const string &prev_hash, const vector<Transaction> &txs, size_t maxTx)
        : index(id),
          previous_hash(prev_hash),
          nonce(0),
          extraNonce(0), 
          timestamp(static_cast<uint64_t>(time(0)))
    {
        // copy up to maxTx transactions
        size_t take = std::min(maxTx, txs.size());
        transactions.assign(txs.begin(), txs.begin() + take);

        merkle_root = computeMerkleRoot(transactions);
        block_hash = headerHash();
    }

    // genesis block constructor
    Block()
    : index(0),
      previous_hash(string(64, '0')), // 64 zeros
      nonce(0),
      extraNonce(0),
      timestamp(static_cast<uint64_t>(time(0))),
      blockMinedTime(0),
      mineTime(0.0)
{
    //genesis transaction
    Transaction genesisTx("0", "SYSTEM", "ALL", 0.0, true);
    transactions.push_back(genesisTx);

    merkle_root = computeMerkleRoot(transactions);
    block_hash = headerHash();
}

    ~Block() = default;

// merkle root of transactions to "block"
static string computeMerkleRoot(const vector<Transaction> &txs) {

    vector<string> hashes;
    hashes.reserve(txs.size());
    for (const auto &tx : txs) {
        hashes.push_back(tx.getTransaction_id());
    }

    while (hashes.size() > 1) {
        if (hashes.size() % 2 == 1)hashes.push_back(hashes.back()); // duplicate last if odd
        vector<string> next;
        next.reserve(hashes.size() / 2);
        for (size_t i = 0; i < hashes.size(); i += 2) {
            string combined = hashes[i] + hashes[i + 1];
            next.push_back(hashas(combined));
        }
        hashes.swap(next);
    }

    return hashes.front();
}

// block header hash
string headerHash() const {
    ostringstream ss;
    ss << previous_hash << timestamp << merkle_root << nonce << extraNonce << 4;
    return hashas(ss.str());
}

// OPENMP multi thread mine
bool mine(int difficulty) {
    if (difficulty < 0) difficulty = 0;
    string target(difficulty, '0');
    const uint64_t MAX_NONCE = numeric_limits<uint64_t>::max();

    auto t0 = chrono::high_resolution_clock::now();

    bool mined = false;       // to stop other threads after found
    uint64_t foundNonce = 0;  
    uint64_t foundExtra = 0;  
    string foundHash;         

    int numThreads = 6;//max8
    omp_set_num_threads(numThreads);

    #pragma omp parallel shared(mined, foundNonce, foundExtra, foundHash)
    {   
        unsigned int thread = omp_get_thread_num();
        uint64_t localNonce = nonce + thread * 1000000; // each thread different start
        uint64_t localExtra = extraNonce;

        while (!mined) {
            // critical - tik vienas thread gali vykdyt koda vienu metu 
            #pragma omp critical
            {
                nonce = localNonce;
                extraNonce = localExtra;
            }

            string hashCandidate = headerHash();

            if (hashCandidate.rfind(target, 0) == 0) { // hash matches difficulty
                #pragma omp critical
                {
                    if (!mined) {
                        mined = true;
                        foundNonce = localNonce;
                        foundExtra = localExtra;
                        foundHash = hashCandidate;
                        cout << "Block mined by thread " << thread << " with nonce " << foundNonce
                             << " and extraNonce " << foundExtra << "\n";
                    }
                }
                break;
            }

            ++localNonce;
            if (localNonce >= MAX_NONCE) {
                localNonce = 0;
                ++localExtra;
            }
        }
    }

    // commit the winning nonce and hash
    nonce = foundNonce;
    extraNonce = foundExtra;
    block_hash = foundHash;

    auto t1 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = t1 - t0;
    mineTime = elapsed.count();
    blockMinedTime = static_cast<uint64_t>(time(0));

    return mined;
}

    // getters
    const string &getPreviousHash() const { return previous_hash; }
    const string &getMerkleRoot() const { return merkle_root; }
    const string &getHash() const { return block_hash; }
    uint64_t getNonce() const { return nonce; }
    uint64_t getExtraNonce() const { return extraNonce; }
    uint64_t getTimestamp() const { return timestamp; }
    uint64_t getBlockMinedTime() const { return blockMinedTime; }
    const vector<Transaction> &getTransactions() const { return transactions; }
    int getIndex() const { return index; }
    double getMineTime() const { return mineTime; }

    
    // print transactions
    void printTransaction(size_t x) const {
        if (x >= transactions.size()) {
            std::cout << "Transaction index out of range.\n";
            return;
        }
        const auto &tx = transactions[x];
        std::cout << "Transaction " << x << ":\n";
        std::cout << "ID:       " << tx.getTransaction_id() << "\n";
        std::cout << "Sender:   " << tx.getSender() << "\n";
        std::cout << "Receiver: " << tx.getReceiver() << "\n";
        std::cout << "Amount:   " << tx.getAmount() << "\n";
        std::cout << "Verified: " << (tx.isVerified() ? "Yes" : "No") << "\n";
        std::cout << "---------------------------\n";
    }

// applies the blocks transactions after its mined
vector<string> applyTransactions(vector<User> &users) {
        vector<string> applied;
        applied.reserve(transactions.size());

        for (auto &tx : transactions) {
            const string &skey = tx.getSender();
            const string &rkey = tx.getReceiver();

            // find sender index
            int sidx = -1;
            int ridx = -1;
            for (size_t i = 0; i < users.size(); ++i) {
                if (users[i].getPublic_key() == skey) sidx = static_cast<int>(i);
                if (users[i].getPublic_key() == rkey) ridx = static_cast<int>(i);
                if (sidx != -1 && ridx != -1) break;
            }

            if (sidx == -1 || ridx == -1) {
                tx.setVerified(false);
                continue;
            }

            User &sender = users[static_cast<size_t>(sidx)];
            User &receiver = users[static_cast<size_t>(ridx)];
            double amount = tx.getAmount();
            if (sender.getBalance() >= amount) {
                sender.setBalance(sender.getBalance() - amount);
                receiver.setBalance(receiver.getBalance() + amount);
                tx.setVerified(true);
                applied.push_back(tx.getTransaction_id());
            } else {
                tx.setVerified(false);
            }
        }

        return applied;
    }
};


// operator << for output
inline ostream& operator<<(ostream& os, const Block &b) {
    os << "\n========== Block " << b.getIndex() << " Result ==========" << "\n";
    os << "Hash:        " << b.getHash() << "\n";
    os << "Nonce:       " << b.getNonce() << "\n";
    os << "ExtraNonce:  " << b.getExtraNonce() << "\n";
    os << "Merkle Root: " << b.getMerkleRoot() << "\n";
    os << "Time to mine block:        " << b.getMineTime() << "s\n";

    if (b.getBlockMinedTime() != 0) {
    time_t mined = static_cast<time_t>(b.getBlockMinedTime());
    os << "Block mined at: " << std::put_time(std::localtime(&mined), "%Y-%m-%d %H:%M:%S") << "\n";} // AI

    os << "=================================================\n\n\n";
    return os;
}

