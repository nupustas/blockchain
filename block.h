#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include "manolib.h"
#include "transaction.h"
#include "functions.h"
#include <limits>
#include "user.h"
#include <algorithm>

class Block {
private:
    int index;
    string previous_hash;
    string merkle_root;
    uint64_t nonce;
    uint64_t extraNonce;   
    uint64_t timestamp;
    vector<Transaction> transactions;
    string block_hash;
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
string headerHash() const {
    ostringstream ss;
    ss << previous_hash << timestamp << merkle_root << nonce << extraNonce << 4;
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

    // Apply this block's transactions to the provided users vector.
    // Uses only vector scans (no map). Returns applied transaction ids.
vector<string> applyTransactions(vector<User> &users) {
        vector<string> applied_ids;
        applied_ids.reserve(transactions.size());

        for (auto &tx : transactions) {
            const string &skey = tx.getSender();
            const string &rkey = tx.getReceiver();

            // find sender index by linear scan
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
                applied_ids.push_back(tx.getTransaction_id());
            } else {
                tx.setVerified(false);
            }
        }

        return applied_ids;
    }
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
    os << "=================================================\n\n\n";
    return os;
}

#endif 
