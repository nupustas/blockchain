#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include "manolib.h"
#include "transaction.h"
#include "functions.h"
#include <limits>

class Block {
private:
    int index;
    std::string previous_hash;
    std::string merkle_root;
    uint64_t nonce;
    uint64_t extraNonce;   
    uint64_t timestamp;
    std::vector<Transaction> transactions;
    std::string block_hash;
    // last mining result info
    bool lastSolved = false;
    double lastMineTime = 0.0;

public:
    // construct a block from given transactions 
    Block(int id, const std::string &prev_hash, const vector<Transaction> &txs, size_t maxTx = 100)
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

    ~Block() = default;

    // merkle root of transactions to "block"
    static string computeMerkleRoot(const vector<Transaction> &txs) {
        if (txs.empty()) return hashas("");

        vector<string> hashes;
        hashes.reserve(txs.size());
        for (const auto &tx : txs) {
            ostringstream ss;
            ss << tx.getTransaction_id() << tx.getSender() << tx.getReceiver() << tx.getAmount();
            hashes.push_back(hashas(ss.str()));
        }

        while (hashes.size() > 1) {
            if (hashes.size() % 2 == 1)
                hashes.push_back(hashes.back()); // duplicate last if odd
            vector<string> next;
            next.reserve(hashes.size() / 2);
            for (size_t i = 0; i < hashes.size(); i += 2) {
                string concat = hashes[i] + hashes[i + 1];
                next.push_back(hashas(concat));
            }
            hashes.swap(next);
        }

        return hashes.front();
    }

    // block header hash
    std::string headerHash() const {
        ostringstream ss;
        ss << previous_hash << merkle_root << timestamp << nonce << extraNonce << index;
        return hashas(ss.str());
    }

    // mine until hash meets difficulty
    bool mine(int difficulty) {
        if (difficulty < 0) difficulty = 0;
        string target(difficulty, '0');
        const uint64_t MAX_NONCE = std::numeric_limits<uint64_t>::max();

        auto t0 = std::chrono::high_resolution_clock::now();
        while (true) {
            block_hash = headerHash();

            // check if hash starts with x zeros
            if (block_hash.rfind(target, 0) == 0) {
                auto t1 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = t1 - t0;
                lastMineTime = elapsed.count();
                lastSolved = true;
                return true;
            }

            ++nonce;

            if (nonce >= MAX_NONCE) {
                nonce = 0;
                ++extraNonce;
            }
        }
    }

    // getters
    const string &getPreviousHash() const { return previous_hash; }
    const string &getMerkleRoot() const { return merkle_root; }
    const string &getHash() const { return block_hash; }
    uint64_t getNonce() const { return nonce; }
    uint64_t getExtraNonce() const { return extraNonce; }
    uint64_t getTimestamp() const { return timestamp; }
    const vector<Transaction> &getTransactions() const { return transactions; }
    int getIndex() const { return index; }
    bool isLastSolved() const { return lastSolved; }
    double getLastMineTime() const { return lastMineTime; }
};

// operator << for output
inline ostream& operator<<(ostream& os, const Block &b) {
    os << "\n========== Block " << b.getIndex() << " Result ==========" << "\n";
    os << "Mined:       " << (b.isLastSolved() ? "Success" : "Failed") << "\n";
    os << "Hash:        " << b.getHash() << "\n";
    os << "Nonce:       " << b.getNonce() << "\n";
    os << "ExtraNonce:  " << b.getExtraNonce() << "\n";
    os << "Merkle Root: " << b.getMerkleRoot() << "\n";
    os << "Time to mine:        " << b.getLastMineTime() << "s\n";
    os << "=================================================\n\n";
    return os;
}

#endif 
